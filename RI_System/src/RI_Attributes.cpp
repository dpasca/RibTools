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
	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;
}

//==================================================================
Attributes::~Attributes()
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );
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
	cmdBasis( RI_BEZIERBASIS, NULL, 3, RI_BEZIERBASIS, NULL, 3 );
	
	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;
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
void Attributes::cmdBasis(
				RtToken ubasis, const float *pCustomUBasis, int ustep,
				RtToken vbasis, const float *pCustomVBasis, int vstep )
{
	if ( ubasis )
		mpyUBasis = mpStatics->FindBasis( ubasis );
	else
		mpCustomUBasis = new Matrix44( pCustomUBasis );

	if ( vbasis )
		mpyVBasis = mpStatics->FindBasis( vbasis );
	else
		mpCustomVBasis = new Matrix44( pCustomVBasis );

	mUSteps	= ustep;
	mVSteps	= vstep;
	BumpRevision();
}

//==================================================================
}