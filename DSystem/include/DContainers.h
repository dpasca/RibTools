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
};

//==================================================================
template <class T, size_t MAX>
class CopyStackMax
{
	T		mVec[MAX];
	size_t	mSize;

public:
	CopyStackMax()
	{
		mSize = 1;
	}
	void push()
	{
		DASSTHROW( mSize < MAX, ("Out of bounds !") );
		mVec[mSize] = mVec[mSize-1];
		mSize += 1;
	}
	
	void pop()
	{
		DASSTHROW( mSize >= 0, ("Out of bounds !") );
		mSize -= 1;
	}

	const T &top() const
	{
		return mVec[mSize-1];
	}

	T &top()
	{
		return mVec[mSize-1];
	}
};

#endif