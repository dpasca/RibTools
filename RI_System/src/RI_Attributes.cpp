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
Attributes::Attributes()
{
}

//==================================================================
void Attributes::Init( SymbolList *pTManager )
{
	mpStatics	= pTManager;
	mBound		= RI_INFINITY;
	mDetail		= RI_INFINITY;

	mMinVisible			= 0;
	mLowerTransition	= 0;
	mUpperTransition	= RI_INFINITY;
	mMaxVisible			= RI_INFINITY;

	cmdGeometricApproximation( RI_EMPTY_TOKEN, 0 );
	cmdOrientation( RI_OUTSIDE );
	cmdSides( 2 );
	cmdBasis( RI_BEZIERBASIS, 3, RI_BEZIERBASIS, 3 );
}


//==================================================================
void Attributes::cmdBound( const Bound &bound )
{
	mBound = bound;
	BumpRevision();
}

//==================================================================
void Attributes::cmdDetail( const Bound &detail )
{
	mDetail = detail;
	BumpRevision();
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
	BumpRevision();
}

//==================================================================
void Attributes::cmdGeometricApproximation(RtToken typeApproximation,
										   float valueApproximation )
{
	mpyTypeApproximation	= mpStatics->FindVoid( typeApproximation );
	mValueApproximation		= valueApproximation;
	BumpRevision();
}

//==================================================================
void Attributes::cmdOrientation( RtToken orientation )
{
	mpyOrientation	= mpStatics->FindVoid( orientation );
	BumpRevision();
}

//==================================================================
void Attributes::cmdSides( int sides )
{
	mSides	= sides;
	BumpRevision();
}

//==================================================================
void Attributes::cmdBasis( RtToken ubasis, int ustep, RtToken vbasis, int vstep )
{
	mpyUBasis = mpStatics->FindBasis( ubasis );
	mpyVBasis = mpStatics->FindBasis( vbasis );
	mUSteps	= ustep;
	mVSteps	= vstep;
	BumpRevision();
}

//==================================================================
}