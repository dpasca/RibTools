/*
 *  DContainers.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef DCONTAINERS_H
#define DCONTAINERS_H

#include "DTypes.h"

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

#endif