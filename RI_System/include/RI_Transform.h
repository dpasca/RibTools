/*
 *  RI_Transform.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RI_TRANSFORM_H
#define RI_TRANSFORM_H

#include "RI_Base.h"
#include "DMatrix44.h"

//==================================================================
namespace RI
{

//==================================================================
/// Transform
//==================================================================
class Transform
{
public:
	Matrix44	mMatrix;

public:
	Transform() : mMatrix(true)	{ }

	// avoid initialization of default values and just copy..
	Transform( const Transform &fromObj ) {	*this = fromObj; }

	~Transform() {}
};

//==================================================================
}

#endif
