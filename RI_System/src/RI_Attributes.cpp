/*
 *  RI_Attribute.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_State.h"
#include "RI_Attributes.h"

//==================================================================
namespace RI
{

//==================================================================
/// Attributes
//==================================================================
Attributes::Attributes()
{
	mpCustomUBasis	= NULL;
	mpCustomVBasis	= NULL;
	mpResManager	= NULL;
}

//==================================================================
Attributes::Attributes( const Attributes &attributes )
{
	copyFrom( attributes );
}

//==================================================================
Attributes& Attributes::operator=(const Attributes& rhs)
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );
	
	copyFrom( rhs );
	
	return *this;
}

//==================================================================
void Attributes::copyFrom(const Attributes& rhs)
{
	mpState				= rhs.mpState;
	mpStatics			= rhs.mpStatics;
	mpResManager		= rhs.mpResManager;
	
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
	mOrientationFlipped	= rhs.mOrientationFlipped	;
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

	mActiveLights		= rhs.mActiveLights;
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
void Attributes::Init(
				  State				*pState,
				  SymbolList		*pStatics,
				  ResourceManager	*pResManager,
				  RevisionTracker	*pRevision )
{
	mpState		= pState;
	mpStatics	= pStatics;
	mpResManager= pResManager;
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
	
	mShaderInstance.mpShader = (SlShader *)pResManager->FindResource( "dbg_normal_col" );
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
	if ( 0 == _stricmp( orientation, RI_INSIDE ) )
	{
		mOrientationFlipped = true;
	}
	else
	if ( 0 == _stricmp( orientation, RI_OUTSIDE ) )
	{
		mOrientationFlipped = false;
	}
	else
	if ( 0 == _stricmp( orientation, RI_LH ) )
	{
		printf( "WARNING: Unsupported LH orientation\n" );	// TODO
	}
	else
	if ( 0 == _stricmp( orientation, RI_RH ) )
	{
		printf( "WARNING: Unsupported RH orientation\n" );	// TODO
	}

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
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdOpacity( const Color &color )
{
	mOpacity = color;
	mpRevision->BumpRevision();
}

//==================================================================
bool Attributes::cmdLightSource( ParamList &params, const Transform &xform, const Matrix44 &mtxWorldCam )
{
	const char	*pLightTypeName = "";

	// get type and ID
	if ( params.size() < 2 || !params[0].IsString() || !params[1].IsIntVal() )
	{
		printf( "Error: bad LightSource definition !\n" );
		return false;
	}

	LightSourceT	*pLight = new LightSourceT();

	if ( 0 == strcasecmp( "ambientlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_AMBIENT;
	}
	else
	if ( 0 == strcasecmp( "distantlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_DISTANT;
	}

	pLight->mID = params[1].Int();

	for (size_t i=2; i < params.size(); ++i)
	{
		if NOT( params[i].IsString() )
			continue;

		const char *pName = params[i].PChar();

		// check for strings that require no extra param here.. if any !

		// follow for params that require a following param
		bool	hasNextParam = ((i+1) < params.size());
		if NOT( hasNextParam )
		{
			printf( "Expecting parameter !" );
			return false;
		}

		if ( 0 == strcasecmp( pName, "from" ) )
		{
			pLight->mLocFromPos = params[i+1].PFlt( 3 );
		}
		else
		if ( 0 == strcasecmp( pName, "to" ) )
		{
			pLight->mLocToPos = params[i+1].PFlt( 3 );
		}
		else
		if ( 0 == strcasecmp( pName, "intensity" ) )
		{
			pLight->mIntesity = params[i+1].Flt();
		}
		else
		if ( 0 == strcasecmp( pName, "lightcolor" ) )
		{
			pLight->mColor = Color( params[i+1].PFlt( 3 ) );
		}

		if NOT( hasNextParam )
			i += 1;
	}

	pLight->UpdateRend( xform, mtxWorldCam );

	size_t listIdx = mpState->AddLightSource( pLight );
	mActiveLights.find_or_push_back( listIdx );

	mpRevision->BumpRevision();

	return true;
}

//==================================================================
void Attributes::cmdSurface( ParamList &params )
{
	if ( mShaderInstance.mpShader )
	{
		mShaderInstance.mpShader->SubRef();
	}
	mShaderInstance.mpShader = (SlShader *)mpResManager->FindResource( "matte" );
	mShaderInstance.mpShader->AddRef();

	mpRevision->BumpRevision();
}

//==================================================================
}