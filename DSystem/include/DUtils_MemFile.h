//==================================================================
/// DMemFile.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMEMFILE_H
#define DMEMFILE_H

#include "DTypes.h"
#include "DContainers.h"

//==================================================================
namespace DUT
{

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
	MemFile( const char *pFileName );
	~MemFile();

	void Init( const void *pDataSrc, size_t dataSize );
	void Init( const char *pFileName );
	void InitExclusiveOwenership( DVec<U8> &fromData );

	const U8 *GetData() const	{	return mpData;		}
	size_t GetDataSize() const	{	return mDataSize;	}

	bool ReadTextLine( char *pDestStr, size_t destStrMaxSize );
};

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

	U8 *GetDataPtr( size_t cnt )
	{
		U8	*ptr = mpDest + mIdx;

		mIdx += cnt;

		return ptr;
	}

	size_t GetCurSize() const { return mIdx; }
};

//==================================================================
/// MemReader
//==================================================================
class MemReader
{
	const U8	*mpSrc;
	size_t		mIdx;
	size_t		mMaxSize;

public:
	MemReader( void *pDest, size_t maxSize ) :
		mpSrc((U8 *)pDest),
		mIdx(0),
		mMaxSize(maxSize)
	{
	}

	template <class T>
	T ReadValue()
	{
		size_t	idx = mIdx;
		mIdx += sizeof(T);
		return *((T *)(mpSrc + idx));
	}

	template <class T>
	void ReadArray( T *pDest, size_t cnt )
	{
		for (size_t i=0; i < cnt; ++i)
		{
			pDest[i] = *((T *)(mpSrc + mIdx));
			mIdx += sizeof(T);
		}
	}

	const U8 *GetDataPtr( size_t cnt )
	{
		size_t	idx = mIdx;
		mIdx += cnt;
		return (const U8 *)(mpSrc + idx);
	}
};

//==================================================================
}

#endif
