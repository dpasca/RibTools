/*
 *  RI_Attribute.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Attributes.h"

//==================================================================
namespace RI
{

//==================================================================
/// Attributess
//==================================================================
void Attributes::cmdBound( const BoundType &bound )
{
	mBound = bound;
}

//==================================================================
void Attributes::cmdDetail( const BoundType &detail )
{
	mDetail = detail;
}

//==================================================================
void Attributes::cmdDetailRange(float	minVisible,
								float	lowerTransition,
								float	upperTransition,
								float	maxVisible )
{
	mMinVisible			= minVisible;
	mLowerTransition	= lowerTransition;
	mUpperTransition	= upperTransition;
	mMaxVisible			= maxVisible;
}

//==================================================================
void Attributes::cmdGeometricApproximation(Token typeApproximation,
										   float valueApproximation )
{
	mTypeApproximation	= typeApproximation;
	mValueApproximation	= valueApproximation;
}

//==================================================================
void Attributes::cmdOrientation( Token orientation )
{
	mOrientation	= orientation;
}

//==================================================================
void Attributes::cmdSides( int sides )
{
	mSides	= sides;
}

//==================================================================
}