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
void Attributes::Init( TokenManager *pTManager )
{
	mpTManager	= pTManager;
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
void Attributes::cmdGeometricApproximation(RtToken typeApproximation,
										   float valueApproximation )
{
	mpTypeApproximationTok	= mpTManager->FindVoid( typeApproximation );
	mValueApproximation		= valueApproximation;
}

//==================================================================
void Attributes::cmdOrientation( RtToken orientation )
{
	mpOrientationTok	= mpTManager->FindVoid( orientation );
}

//==================================================================
void Attributes::cmdSides( int sides )
{
	mSides	= sides;
}

//==================================================================
void Attributes::cmdBasis( RtToken ubasis, int ustep, RtToken vbasis, int vstep )
{
	mpUBasisTok = mpTManager->FindBasis( ubasis );
	mpVBasisTok = mpTManager->FindBasis( vbasis );
	mUSteps	= ustep;
	mVSteps	= vstep;
}

//==================================================================
}