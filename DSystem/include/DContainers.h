//==================================================================
/// DContainers.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DCONTAINERS_H
#define DCONTAINERS_H

#include <memory.h>
#include "DTypes.h"
#include "DMemory.h"
#include "DUtils_Base.h"

//==================================================================
static const size_t	NPOS = (size_t)-1;

#if 1
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
	DVec() :
		mpData(NULL),
		mSize(0),
		mSizeAlloc(0)
	{
	}

	DVec( const DVec &from ) :
		mpData(NULL),
		mSize(0),
		mSizeAlloc(0)
	{
		copyFrom( from );
	}

	// virtual
	~DVec()
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
			DDELETE_ARRAY( (u_char *)mpData );
		mpData = NULL;
	}

public:
	size_t size() const { return mSize; }

	iterator		begin()			{ return mpData;	}
	const_iterator	begin()	const	{ return mpData;	}

	iterator		end()			{ return mpData + mSize;	}
	const_iterator	end()	const	{ return mpData + mSize;	}

	void clear()
	{
		resize( 0 );
	}

	void reserve( size_t newSizeAlloc )
	{
		if ( newSizeAlloc <= mSizeAlloc )
			return;

		T *newPData = (T *)DNEW u_char [ sizeof(T) * newSizeAlloc ];

		// return silently on failure
		if NOT( newPData )
			return;

		if ( mpData )
		{
			memcpy( newPData, mpData, mSize * sizeof(T) );

			DDELETE_ARRAY( (u_char *)mpData );
		}

		mpData = newPData;
		mSizeAlloc = newSizeAlloc;
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
				size_t dude = sizeof(T);
				newSizeAlloc = (sizeof(T) < 128 ? 128 : sizeof(T)) / sizeof(T);
			}

			if ( newSizeAlloc < newSize )
				newSizeAlloc = newSize;

			T *newPData = (T *)DNEW u_char [ sizeof(T) * newSizeAlloc ];

			if ( mpData )
			{
				memcpy( newPData, mpData, mSize * sizeof(T) );

				DDELETE_ARRAY( (u_char *)mpData );
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
		DASSTHROW( it >= mpData && it < (mpData+mSize), ("Out of bounds !") );
		size_t	idx = it - mpData;
		mpData[idx].~T();
		for (size_t i=idx; i < mSize-1; ++i)
		{
			mpData[i] = mpData[i+1];
		}
		mSize -= 1;
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
		T	tmp = val;	// TODO: why needs this ?!
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

	void find_or_push_back( const T &val )
	{
		if ( find( val ) != end() )
			return;

		T	tmp = val;	// TODO: why needs this ?!
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
#else
//==================================================================
template<class T>
class DVec : public std::vector<T>
{
public:
	T *grow( size_t n=1 )
	{
		size_t	fromIdx = this->size();
		this->resize( fromIdx + n );
		return &(*this)[fromIdx];
	}
};
#endif

//==================================================================
template <class T>
class Stack
{
	DVec<T>	mVec;

public:
	void push( const T &val )
	{
		mVec.push_back( val );
	}

	void pop()
	{
		mVec.pop_back();
	}

	const T &top() const
	{
		return mVec.back();
	}

	T &top()
	{
		return mVec.back();
	}

	void clear()
	{
		mVec.clear();
	}
};

//==================================================================
template <class T>
class CopyStack
{
	DVec<T>	mVec;

public:
	CopyStack()
	{
		mVec.resize(1);
	}
	void push()
	{
		mVec.push_back( top() );
	}

	void pop()
	{
		mVec.pop_back();
	}

	const T &top() const
	{
		return mVec.back();
	}

	T &top()
	{
		return mVec.back();
	}

	void clear()
	{
		mVec.clear();
	}
};

//==================================================================
template <class T, size_t MAX>
class CopyStackMax
{
	U8		mVec[ sizeof(T) * MAX ];
	size_t	mSize;

public:
	CopyStackMax()
	{
		T	*p = (T *)&mVec[0];
		new ( p ) T;
		mSize = 1;
	}

	void push()
	{
		DASSTHROW( mSize < MAX, ("Out of bounds !") );

		*(T *)&mVec[mSize * sizeof(T)] = top();

		mSize += 1;
	}

	void pop()
	{
		DASSTHROW( mSize >= 0, ("Out of bounds !") );
		mSize -= 1;
	}

	const T &top() const
	{
		return *(const T *)&mVec[(mSize-1) * sizeof(T)];
	}

	T &top()
	{
		return *(T *)&mVec[(mSize-1) * sizeof(T)];
	}

	void clear()
	{
		for (size_t i=0; i < mSize; ++i)
			mVec[i].~T();

		mSize = 0;
	}
};

#endif
