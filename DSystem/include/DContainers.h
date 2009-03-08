/*
 *  DContainers.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef DCONTAINERS_H
#define DCONTAINERS_H

#include "DTypes.h"
#include "DUtils.h"

//==================================================================
static const size_t	NPOS = (size_t)-1;

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
	u_char	mVec[ sizeof(T) * MAX ];
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