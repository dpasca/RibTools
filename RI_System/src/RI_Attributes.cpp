/*
 *  RI_Attribute.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Attributes.h"

//==================================================================
namespace RI
{

//==================================================================
/// Attributes
//==================================================================
Attributes::Attributes()
{
	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;
}

//==================================================================
Attributes::Attributes( const Attributes &attributes )
{
	copyFrom( attributes );
}

//==================================================================
const Attributes& Attributes::operator=(const Attributes& rhs)
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );
	
	copyFrom( rhs );
	
	return *this;
}
	
//==================================================================
void Attributes::copyFrom(const Attributes& rhs)
{
	mpStatics			= rhs.mpStatics;
	
	mpRevision			= rhs.mpRevision			;
	mSymbols			= rhs.mSymbols				;
	mBound				= rhs.mBound				;
	mDetail				= rhs.mDetail				;
	mMinVisible			= rhs.mMinVisible			;
	mLowerTransition	= rhs.mLowerTransition		;
	mUpperTransition	= rhs.mUpperTransition		;
	mMaxVisible			= rhs.mMaxVisible			;
	mpyTypeApproximation= rhs.mpyTypeApproximation	;
	mValueApproximation	= rhs.mValueApproximation	;
	mpyOrientation		= rhs.mpyOrientation		;
	mSides				= rhs.mSides				;
	mpyUBasis			= rhs.mpyUBasis				;
	mpyVBasis			= rhs.mpyVBasis				;

	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;

	if ( rhs.mpCustomUBasis )
		mpCustomUBasis = new Matrix44( *rhs.mpCustomUBasis );

	if ( rhs.mpCustomVBasis )
		mpCustomVBasis = new Matrix44( *rhs.mpCustomVBasis );	

	
	mUSteps				= rhs.mUSteps				;
	mVSteps				= rhs.mVSteps				;
	mColor				= rhs.mColor				;
	mOpacity			= rhs.mOpacity				;
	mShaderInstance		= rhs.mShaderInstance		;
		
	if ( mShaderInstance.mpShader )
		mShaderInstance.mpShader->AddRef();
}

//==================================================================
Attributes::~Attributes()
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );

	if ( mShaderInstance.mpShader )
	{
		mShaderInstance.mpShader->SubRef();
		mShaderInstance.mpShader = NULL;
	}
}

//==================================================================
void Attributes::Init( SymbolList *pStatics, ResourceManager *pResManager, RevisionTracker *pRevision )
{
	mpStatics	= pStatics;
	mpRevision	= pRevision;

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
	
	mColor.Set( 1, 1, 1 );
	mOpacity.Set( 1, 1, 1 );
	
	DASSERT( mShaderInstance.mpShader == NULL );
	
	mShaderInstance.mpShader = (SlShader *)pResManager->FindResource( "matte" );
	mShaderInstance.mpShader->AddRef();
}

//==================================================================
void Attributes::cmdBound( const Bound &bound )
{
	mBound = bound;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdDetail( const Bound &detail )
{
	mDetail = detail;
	mpRevision->BumpRevision();
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
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdGeometricApproximation(RtToken typeApproximation,
										   float valueApproximation )
{
	mpyTypeApproximation	= mpStatics->FindVoid( typeApproximation );
	mValueApproximation		= valueApproximation;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdOrientation( RtToken orientation )
{
	mpyOrientation	= mpStatics->FindVoid( orientation );
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdSides( int sides )
{
	mSides	= sides;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdBasis(
				RtToken ubasis, const float *pCustomUBasis, int ustep,
				RtToken vbasis, const float *pCustomVBasis, int vstep )
{
	if ( ubasis )
		mpyUBasis = mpStatics->FindBasis( ubasis );
	else
	{
		DSAFE_DELETE( mpCustomUBasis );
		mpCustomUBasis = new Matrix44( pCustomUBasis );
	}

	if ( vbasis )
		mpyVBasis = mpStatics->FindBasis( vbasis );
	else
	{
		DSAFE_DELETE( mpCustomVBasis );
		mpCustomVBasis = new Matrix44( pCustomVBasis );
	}

	mUSteps	= ustep;
	mVSteps	= vstep;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdColor( const Color &color )
{
	mColor = color;
}

//==================================================================
void Attributes::cmdOpacity( const Color &color )
{
	mOpacity = color;
}

//==================================================================
bool Attributes::cmdLightSource( ParamList &params )
{
	const char	*pLightTypeName = "";

	LightSource	light;

	// get type and ID
	if ( params.size() < 2 || !params[0].IsString() || !params[1].IsIntVal() )
	{
		printf( "Error: bad LightSource definition !\n" );
		return false;
	}

	if ( 0 == strcasecmp( "ambientlight", params[0].PChar() ) )
	{
		light.mType = LightSource::TYPE_AMBIENT;
	}
	else
	if ( 0 == strcasecmp( "distantlight", params[0].PChar() ) )
	{
		light.mType = LightSource::TYPE_DISTANT;
	}

	light.mID = params[1].Int();

	return true;
}
//==================================================================
}