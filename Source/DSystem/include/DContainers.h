//==================================================================
/// DContainers.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DCONTAINERS_H
#define DCONTAINERS_H

#if !defined(NACL)
# include <memory.h>
#endif
#include "DTypes.h"
#include "DMemory.h"
#include "DUtils_Base.h"
#include "DExceptions.h"

#if defined(ANDROID) || defined(MACOSX) || (defined(TARGET_OS_IPHONE) && !defined(__clang__))
# include <tr1/unordered_map>
# include <tr1/unordered_set>
#else
# include <unordered_map>
# include <unordered_set>
#endif

#include <string>

#if defined(NACL) || (defined(__APPLE__) && !defined(MACOSX) && defined(__clang__))
# define DUNORD_MAP	std::unordered_map
# define DUNORD_SET	std::unordered_set
#else
# define DUNORD_MAP	std::tr1::unordered_map
# define DUNORD_SET	std::tr1::unordered_set
#endif

//==================================================================
template <class T>
class DVec
{
	T		*mpData;
	size_t	mSize;
	size_t	mSizeAlloc;

public:
	typedef const T	*const_iterator;
	typedef T		*iterator;

public:
	DVec() : mpData(NULL), mSize(0), mSizeAlloc(0) { }

    DVec( size_t initSize ) : mpData(NULL), mSize(0), mSizeAlloc(0)
    {
        resize( initSize );
    }

    DVec( size_t initSize, const T &initVal ) : mpData(NULL), mSize(0), mSizeAlloc(0)
    {
        resize( initSize );
        for (size_t i=0; i != initSize; ++i)
            (*this)[i] = initVal;
    }

	DVec( const DVec &from ) : mpData(NULL), mSize(0), mSizeAlloc(0)
	{
		copyFrom( from );
	}

	virtual ~DVec()
	{
		clear();
		freeAll();
	}

	DVec &operator=(const DVec& rhs)
	{
		clear();
		freeAll();
		copyFrom( rhs );

		return *this;
	}

private:
	void copyFrom( const DVec& from )
	{
		resize( from.mSize );

		for (size_t i=0; i < mSize; ++i)
			mpData[i] = from.mpData[i];
	}

	void freeAll()
	{
		mSizeAlloc = 0;
		if ( mpData )
			DDELETE_ARRAY( (u_char *)(void*)mpData );
		mpData = NULL;
	}

public:
	size_t size() const { return mSize; }
	size_t size_bytes() const { return mSize * sizeof(T); }

	size_t capacity() const { return mSizeAlloc; }

	iterator		begin()			{ return mpData;	}
	const_iterator	begin()	const	{ return mpData;	}

	iterator		end()			{ return mpData + mSize;	}
	const_iterator	end()	const	{ return mpData + mSize;	}

	void clear()
	{
		resize( 0 );
	}

	void clear_free()
	{
		resize( 0 );
		freeAll();
	}

	void force_reserve( size_t reserveSize )
	{
		size_t newSizeAlloc = reserveSize;

		if ( newSizeAlloc <= mSizeAlloc )
			return;

		T *newPData = (T *)DNEW u_char [ sizeof(T) * newSizeAlloc ];

        if ( mpData )
        {
            for (size_t i=0; i < mSize; ++i)
            {
                new (&newPData[i]) T;
                newPData[i] = mpData[i];
                mpData[i].~T();
            }

            DDELETE_ARRAY( (u_char *)(void*)mpData );
        }

		mpData = newPData;
		mSizeAlloc = newSizeAlloc;
	}

	void reserve( size_t newSizeAlloc )
	{
		DTRY {
			force_reserve( newSizeAlloc );
		} DCATCH_ALL {
			return;
		}
	}

	void resize( size_t newSize )
	{
		if ( newSize == mSize )
			return;

		if ( newSize > mSizeAlloc )
		{
			size_t newSizeAlloc = mSizeAlloc * 2;

			if ( newSizeAlloc == 0 )
			{
				newSizeAlloc = (sizeof(T) < 128 ? 128 : sizeof(T)) / sizeof(T);
			}

			if ( newSizeAlloc < newSize )
				newSizeAlloc = newSize;

			T *newPData = (T *)DNEW u_char [ sizeof(T) * newSizeAlloc ];

			if ( mpData )
			{
				//memcpy( newPData, mpData, mSize * sizeof(T) );

				for (size_t i=0; i < mSize; ++i)
				{
					new (&newPData[i]) T;
					newPData[i] = mpData[i];
					mpData[i].~T();
				}

				DDELETE_ARRAY( (u_char *)(void*)mpData );
			}

			mpData = newPData;
			mSizeAlloc = newSizeAlloc;
		}

		if ( newSize < mSize )
		{
			for (size_t i=newSize; i < mSize; ++i)
				mpData[i].~T();
		}
		else
		{
			for (size_t i=mSize; i < newSize; ++i)
				new (&mpData[i]) T;
		}

		mSize = newSize;
	}

	T *grow( size_t n=1 )
	{
		size_t	fromIdx = size();
		resize( fromIdx + n );
		return mpData + fromIdx;
	}

	void erase( iterator it )
	{
		if NOT( it >= mpData && it < (mpData+mSize) )
			DEX_OUT_OF_RANGE( "Out of bounds !" );

		size_t	idx = it - mpData;
		// mpData[idx].~T(); -- DON'T !!!
		for (size_t i=idx; i < mSize-1; ++i)
		{
			mpData[i] = mpData[i+1];
		}
		mSize -= 1;
		if ( mSize )
			mpData[mSize].~T();
	}

	void insert( iterator itBefore, T &val )
	{
		if NOT( itBefore >= mpData && itBefore < (mpData+mSize) )
			DEX_OUT_OF_RANGE( "Out of bounds !" );

		size_t	idx = itBefore - mpData;

		resize( mSize + 1 );

		for (size_t i=mSize; i > (idx+1); --i)
			mpData[i-1] = mpData[i-2];

		mpData[idx] = val;
	}

	void push_front( const T &val )
	{
		grow();
		for (size_t i=mSize; i > 1; --i)
		{
			mpData[i-1] = mpData[i-2];
		}

		mpData[0] = val;
	}

	void push_back( const T &val )
	{
		// necessary for things such as push_back( back() );
		T	tmp = val;
		*grow() = tmp;
	}

	void pop_back()
	{
		DASSERT( mSize >= 1 );
		resize( mSize - 1 );
	}

	iterator find( const T &val )
	{
		for (size_t i=0; i < mSize; ++i)
		{
			if ( mpData[i] == val )
			{
				return mpData + i;
			}
		}

		return end();
	}

	const_iterator find( const T &val ) const
	{
		for (size_t i=0; i < mSize; ++i)
		{
			if ( mpData[i] == val )
			{
				return mpData + i;
			}
		}

		return end();
	}

	size_t find_idx( const T &val, size_t from=0 )
	{
		for (size_t i=from; i < mSize; ++i)
		{
			if ( mpData[i] == val )
			{
				return i;
			}
		}

		return DNPOS;
	}

	void find_or_push_back( const T &val )
	{
		if ( find( val ) != end() )
			return;

		// necessary for things such as push_back( back() );
		T	tmp = val;
		*grow() = tmp;
	}

	void append_array( const T *pSrc, size_t cnt )
	{
		T	*pDes = grow( cnt );
		for (size_t i=0; i < cnt; ++i)
		{
			pDes[i] = pSrc[i];
		}
	}

	void append( const DVec<T> &srcVec )
	{
		append_array( &srcVec[0], srcVec.size() );
	}

	void get_ownership( DVec<T> &from )
	{
		freeAll();

		mpData		= from.mpData;
		mSize		= from.mSize;
		mSizeAlloc	= from.mSizeAlloc;

		from.mpData		= NULL;
		from.mSize		= 0;
		from.mSizeAlloc	= 0;
	}

	const	T &back() const { DASSERT( mSize >= 1 ); return mpData[mSize-1]; }
			T &back()		{ DASSERT( mSize >= 1 ); return mpData[mSize-1]; }

	const T &operator[]( size_t idx ) const { DASSERT( idx < mSize ); return mpData[ idx ]; }

#if defined(__GNUC__)	// WTF ?!
		  T &operator[]( size_t idx )		{ return mpData[ idx ]; }
#else
		  T &operator[]( size_t idx )		{ DASSERT( idx < mSize ); return mpData[ idx ]; }
#endif
};

//==================================================================

// android headers define _N (and others), so that can't be used as a
// template parameter.

template <class _T, size_t _SIZE>
class DArray
{
	U32		mBuff[ (sizeof(_T) * _SIZE + 3) / 4 ];
	size_t	mSize;

public:
	typedef const _T	*const_iterator;
	typedef _T			*iterator;

public:
	DArray() :
		mSize(0)
	{
	}

	DArray( const DArray &from ) :
		mSize(0)
	{
		copyFrom( from );
	}

	DArray &operator=(const DArray& rhs)
	{
		copyFrom( rhs );

		return *this;
	}

private:
	void copyFrom( const DArray& from )
	{
		for (size_t i=0; i < mSize; ++i)
			((_T *)mBuff)[i] = ((const _T *)from.mBuff)[i];
	}

public:
	size_t size() const { return mSize; }
	size_t size_bytes() const { return mSize * sizeof(_T); }

	size_t capacity() const { return _SIZE; }

	void clear()	{ return resize( 0 ); }

    bool is_full() const { return size() == capacity(); }

	iterator		begin()			{ return (_T *)(void*)mBuff;	}
	const_iterator	begin()	const	{ return (const _T *)(void*)mBuff;	}

	iterator		end()			{ return (_T *)(void*)mBuff + mSize;	}
	const_iterator	end()	const	{ return (const _T *)(void*)mBuff + mSize;	}

	void resize( size_t newSize )
	{
		if ( newSize == mSize )
			return;

		if ( newSize > _SIZE )
			DEX_BAD_ALLOC( "Failed to resize() a DArray. Max allowed is " SIZE_T_FMT " requested is " SIZE_T_FMT ".", _SIZE, newSize );

		if ( newSize < mSize )
		{
			for (size_t i=newSize; i < mSize; ++i)
				((_T *)(void*)mBuff)[i].~_T();
		}
		else
		{
			for (size_t i=mSize; i < newSize; ++i)
				new (&((_T *)(void*)mBuff)[i]) _T;
		}

		mSize = newSize;
	}

	_T *grow( size_t n=1 )
	{
		size_t	fromIdx = size();
		resize( fromIdx + n );
		return (_T *)(void*)mBuff + fromIdx;
	}

	void erase( iterator it )
	{
		if NOT( it >= (_T *)(void*)mBuff && it < ((_T *)(void*)mBuff+mSize) )
			DEX_OUT_OF_RANGE( "Out of bounds !" );

		size_t	idx = it - (_T *)(void*)mBuff;
		((_T *)(void*)mBuff)[idx].~_T();
		for (size_t i=idx; i < mSize-1; ++i)
		{
			((_T *)(void*)mBuff)[i] = ((_T *)(void*)mBuff)[i+1];
		}
		mSize -= 1;
	}

	void insert( iterator itBefore, _T &val )
	{
        if ( mSize == 0 ) // being stl-like.. I hope !
        {
    		resize( 1 );
    		((_T *)(void*)mBuff)[0] = val;
            return;
        }
        else
        if ( itBefore == ((_T *)(void*)mBuff+mSize) )
        {
            push_back( val );
            return;
        }

		if NOT( itBefore >= (_T *)(void*)mBuff && itBefore < ((_T *)(void*)mBuff+mSize) )
			DEX_OUT_OF_RANGE( "Out of bounds !" );

		size_t	idx = itBefore - (_T *)(void*)mBuff;

		resize( mSize + 1 );

		for (size_t i=mSize; i > (idx+1); --i)
			((_T *)(void*)mBuff)[i-1] = ((_T *)(void*)mBuff)[i-2];

		((_T *)(void*)mBuff)[idx] = val;
	}

	void push_front( const _T &val )
	{
		grow();
		for (size_t i=mSize; i > 1; --i)
		{
			((_T *)(void*)mBuff)[i-1] = ((_T *)(void*)mBuff)[i-2];
		}

		((_T *)(void*)mBuff)[0] = val;
	}

	void push_back( const _T &val )
	{
		// necessary for things such as push_back( back() );
		_T	tmp = val;
		*grow() = tmp;
	}

	void pop_back()
	{
		DASSERT( mSize >= 1 );
		resize( mSize - 1 );
	}

	iterator find( const _T &val )
	{
		for (size_t i=0; i < mSize; ++i)
		{
			if ( ((_T *)(void*)mBuff)[i] == val )
			{
				return ((_T *)(void*)mBuff) + i;
			}
		}

		return end();
	}

	size_t find_idx( const _T &val, size_t from=0 )
	{
		for (size_t i=from; i < mSize; ++i)
		{
			if ( ((_T *)(void*)mBuff)[i] == val )
			{
				return i;
			}
		}

		return DNPOS;
	}

	void find_or_push_back( const _T &val )
	{
		if ( find( val ) != end() )
			return;

		// necessary for things such as push_back( back() );
		_T	tmp = val;
		*grow() = tmp;
	}

	void append_array( const _T *pSrc, size_t cnt )
	{
		_T	*pDes = grow( cnt );
		for (size_t i=0; i < cnt; ++i)
		{
			pDes[i] = pSrc[i];
		}
	}

	void append( const DVec<_T> &srcVec )
	{
		append_array( &srcVec[0], srcVec.size() );
	}

	const	_T &back() const { DASSERT( mSize >= 1 ); return ((const _T*)(void*)mBuff)[mSize-1]; }
			_T &back()		{ DASSERT( mSize >= 1 ); return ((_T*)(void*)mBuff)[mSize-1]; }

	const _T &operator[]( size_t idx ) const { DASSERT( idx < mSize ); return ((const _T*)(void*)mBuff)[ idx ]; }

#if defined(__GNUC__)	// WTF ?!
		  _T &operator[]( size_t idx )		{ return ((_T*)(void*)mBuff)[ idx ]; }
#else
		  _T &operator[]( size_t idx )		{ DASSERT( idx < mSize ); return ((_T*)(void*)mBuff)[ idx ]; }
#endif

};

//==================================================================
template <class _T>
void Dpush_back_unique( DVec<_T> &vec, const _T &val )
{
	if ( vec.find( val ) == vec.end() )
		vec.push_back( val );
}

//==================================================================
template <class _T>
_T &Dgrow( DVec<_T> &vec )
{
    vec.resize( vec.size() + 1 );
    return vec.back();
}

//==================================================================
template <size_t _NUM>
class DVecBits
{
	U8	mData[_NUM];

public:
	DVecBits()
	{
		clear();
	}

	void clear()					{ memset( &mData[0], 0, sizeof(mData) ); }
	void Set( size_t i )			{ DASSERT( i < _NUM ); mData[ i >> 3 ] |= 1 << (i & 7); }
	void Reset( size_t i )			{ DASSERT( i < _NUM ); mData[ i >> 3 ] &= ~(1 << (i & 7)); }
	bool IsSet( size_t i ) const	{ DASSERT( i < _NUM ); return !!(mData[ i >> 3 ] & (1 << (i & 7))); }
};

#endif
