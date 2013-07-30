//==================================================================
/// DUtils_MemFile.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DUTILS_MEMFILE_H
#define DUTILS_MEMFILE_H

#if !defined(NACL)
# include <memory.h>
#endif
#include "DTypes.h"
#include "DStr.h"
#include "DContainers.h"
#include "DExceptions.h"

//==================================================================
namespace DUT
{

//==================================================================
/// MemWriter
//==================================================================
class MemWriter
{
	U8		*mpDest;
	size_t	mIdx;
	size_t	mMaxSize;

public:
	MemWriter( void *pDest, size_t maxSize ) :
		mpDest((U8 *)pDest),
		mIdx(0),
		mMaxSize(maxSize)
	{
	}

	template <class T>
	void WriteValue( const T &from )
	{
		*((T *)(mpDest + mIdx)) = from;
		mIdx += sizeof(T);
	}

	template <class T>
	void WriteArray( const T *pFrom, size_t cnt )
	{
		for (size_t i=0; i < cnt; ++i)
		{
			*((T *)(mpDest + mIdx)) = pFrom[i];
			mIdx += sizeof(T);
		}
	}

	U8 *Grow( size_t cnt )
	{
		U8	*ptr = mpDest + mIdx;

		mIdx += cnt;

		return ptr;
	}

	const U8 *GetDataBegin() const { return mpDest; }

	size_t GetCurSize() const { return mIdx; }
};

//==================================================================
/// MemWriterDynamic
//==================================================================
class MemWriterDynamic
{
	friend class MemFile;

	DVec<U8>	mDest;
	U32			mBits;
	int			mBitsCnt;

public:
	MemWriterDynamic() :
				mBits(0),
				mBitsCnt(0)
	{
	}

	void Resize( size_t size )
	{
		DASSTHROW(
			size <= mDest.size(),
			"MemWriteDynamic::Resize() can only shorten" );

		mBits = 0;
		mBitsCnt = 0;
		mDest.resize( size );
	}

	template <class T>
	void WriteValue( const T &from )
	{
		size_t	idx = mDest.size();
		mDest.resize( idx + sizeof(T) );
		memcpy( &mDest[idx], &from, sizeof(T) );
	}

	template <class T>
	void WriteArray( const T *pFrom, size_t cnt )
	{
		size_t	idx = mDest.size();
		mDest.resize( idx + sizeof(T)*cnt );
		memcpy( &mDest[idx], pFrom, sizeof(T)*cnt );
	}

	void WriteBits( U16 bits, size_t cnt )
	{
		// max 16 bits at once
		DASSERT( cnt <= 16 );

		// push the new bits at the back
		mBits <<= cnt;
		// mask the bits to make sure that
		// the output doesn't go beyond cnt-bits
		mBits |= bits & ((1 << cnt) - 1);
		// increase the count of bits
		mBitsCnt += cnt;

		while ( mBitsCnt >= 8 )
		{
			// write out the topmost 8 bits
			WriteValue( (U8)(mBits >> (mBitsCnt - 8)) );
			// update the number of bits stored
			mBitsCnt -= 8;
		}
	}

	void WriteBitsEnd()
	{
		// WriteBits shouldn't have let this happen
		DASSERT( mBitsCnt < 8 );

		// flush out the remaining bits with zeros
		if ( mBitsCnt )
			WriteBits( 0x00, 8 - mBitsCnt );

		DASSERT( mBitsCnt == 0 );
	}

	void WriteString( const DStr &str );
	void WriteString( const char *pStr );
	void WritePStr16( const DStr &str );

	void PrintF( const char *pFmt, ... );

	U8 *Grow( size_t cnt )
	{
		return mDest.grow( cnt );
	}

	void Reserve( size_t siz )
	{
		mDest.force_reserve( siz );
	}

	const U8 *GetDataBegin() const { return &mDest[0]; }

	size_t GetCurSize() const { return mDest.size(); }
};

//==================================================================
/// MemReader
//==================================================================
class MemReader
{
	DVec<U8>	mOwnedData;
	const U8	*mpSrc;
	size_t		mIdx;
	size_t		mMaxSize;
	U32			mBits;
	int			mBitsCnt;

public:
	MemReader( const void *pDest, size_t maxSize ) :
		mpSrc((const U8 *)pDest),
		mIdx(0),
		mMaxSize(maxSize),
		mBits(0),
		mBitsCnt(0)
	{
	}

	MemReader( const DVec<U8> &vec ) :
		mpSrc((const U8 *)&vec[0]),
		mIdx(0),
		mMaxSize(vec.size()),
		mBits(0),
		mBitsCnt(0)
	{
	}

	MemReader( DVec<U8> &vec ) :
		mpSrc(NULL),
		mIdx(0),
		mMaxSize(0),
		mBits(0),
		mBitsCnt(0)
	{
		InitOwnVec( vec );
	}

	void InitOwnVec( DVec<U8> &vec )
	{
		mOwnedData.get_ownership( vec );
		mpSrc = (const U8 *)&mOwnedData[0];
		mMaxSize = mOwnedData.size();
	}

	template <class T>
	T PeekValue() const
	{
		if ( (mIdx + sizeof(T)) > mMaxSize )
			DEX_OUT_OF_RANGE( "ReadValue" );

	#if defined(D_UNALIGNED_MEM_ACCESS)
		return *((T *)(mpSrc + mIdx));
	#else
		// make a buffer long enough AND aligned
		U64 tmp[ (sizeof(T) + sizeof(U64)-1) / sizeof(U64) ];
		// copy into the temporary buffer
		memcpy( tmp, mpSrc + mIdx, sizeof(T) );

		return *(const T *)tmp;
	#endif
	}

	template <class T>
	T ReadValue()
	{
		T val = PeekValue<T>();
		mIdx += sizeof(T);

		return val;
	}

	template <class T>
	void ReadArray( T *pDest, size_t cnt )
	{
		DASSTHROW( (mIdx+cnt) <= mMaxSize, "Out of bounds!" );

	#if defined(D_UNALIGNED_MEM_ACCESS)
		for (size_t i=0; i < cnt; ++i)
		{
			pDest[i] = *((T *)(mpSrc + mIdx));
			mIdx += sizeof(T);
		}
	#else
		memcpy( pDest, mpSrc + mIdx, cnt * sizeof(T) );
		mIdx += sizeof(T) * cnt;
	#endif
	}

	DStr ReadPStr16()
	{
		size_t len = ReadValue<U16>();

		if NOT( len )
			return DStr();

		const U8 *pData = GetDataPtr( len );

		return DStr( (const char *)pData, len );
	}

	U32 ReadBits( size_t cnt )
	{
		U32	bits = 0;

		// does it have enough bits ?
		while ( mBitsCnt < (int)cnt )
		{
			// ..if not, get a new byte
			U8 val = ReadValue<U8>();

			// add the new byte at the back of the bits stream
			mBits <<= 8;
			mBits |= val;
			mBitsCnt += 8;
		}

		bits = mBits;
		bits >>= mBitsCnt - cnt;
		bits &= (1 << cnt) - 1;

		mBitsCnt -= cnt;

		return bits;
	}

	void ReadBitsEnd()
	{
		mBits		= 0;
		mBitsCnt	= 0;
	}

	const U8 *GetDataPtr( size_t cnt )
	{
		if ( (mIdx+cnt) > mMaxSize )
			DEX_OUT_OF_RANGE( "GetDataPtr" );

		size_t	idx = mIdx;
		mIdx += cnt;

		return (const U8 *)(mpSrc + idx);
	}

	void SkipBytes( size_t cnt )
	{
		if ( (mIdx+cnt) > mMaxSize )
			DEX_OUT_OF_RANGE( "SkipBytes" );

		mIdx += cnt;
	}

	void SeekFromStart( size_t offset )
	{
		if ( offset > mMaxSize )
			DEX_OUT_OF_RANGE( "SeekFromStart" );

		mIdx = offset;
	}

	void SeekFromCurPos( ptrdiff_t offset )
	{
		if ( (mIdx+offset) > mMaxSize )
			DEX_OUT_OF_RANGE( "SeekFromCurPos" );

		mIdx += offset;
	}

	size_t GetCurPos() const
	{
		return mIdx;
	}

	bool IsEOF() const
	{
		return mIdx >= mMaxSize;
	}
};

//==================================================================
/// MemFile
//==================================================================
class MemFile
{
	DVec<U8>	mOwnData;
	const U8	*mpData;
	size_t		mDataSize;
	size_t		mReadPos;
	bool		mIsReadOnly;

public:
	MemFile();
	MemFile( const void *pDataSrc, size_t dataSize );
	MemFile( const char *pFileName, bool prefs = false );
	MemFile( MemWriterDynamic &mw );
	~MemFile();

	void Init( const void *pDataSrc, size_t dataSize );
	void Init( const char *pFileName, bool prefs = false );
	bool InitNoThrow( const char *pFileName, bool prefs = false );
	void InitExclusiveOwenership( DVec<U8> &fromData );
	void InitExclusiveOwenership( MemWriterDynamic &mw );

	const U8 *GetData() const	{	return mpData;		}
	size_t GetDataSize() const	{	return mDataSize;	}

	bool ReadTextLine( char *pDestStr, size_t destStrMaxSize );
	void ReadData( void *pDest, size_t readSize );
	const void *ReadDataPtr( size_t readSize );

	void SeekSet( size_t pos );
	void SeekFromCur( ptrdiff_t offset );
	void SeekFromEnd( ptrdiff_t offset );

	size_t GetCurPos() const { return mReadPos; }
};

//==================================================================
}

#endif
