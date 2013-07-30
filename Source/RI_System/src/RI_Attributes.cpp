//==================================================================
/// RI_Attributes.cpp
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include <stdarg.h>
#include <stdexcept>
#include "RI_State.h"
#include "RI_Symbol.h"
#include "RI_Attributes.h"
#include "DSystem/include/DUtils_Files.h"

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
	mpGlobalSyms		= rhs.mpGlobalSyms;
	mpResManager		= rhs.mpResManager;

	mpRevision			= rhs.mpRevision			;
	mBound				= rhs.mBound				;
	mDetail				= rhs.mDetail				;
	mMinVisible			= rhs.mMinVisible			;
	mLowerTransition	= rhs.mLowerTransition		;
	mUpperTransition	= rhs.mUpperTransition		;
	mMaxVisible			= rhs.mMaxVisible			;
	mpTypeApproximation	= rhs.mpTypeApproximation	;
	mValueApproximation	= rhs.mValueApproximation	;
	mOrientationFlipped	= rhs.mOrientationFlipped	;
	mSides				= rhs.mSides				;
	mpUBasis			= rhs.mpUBasis				;
	mpVBasis			= rhs.mpVBasis				;

	mpCustomUBasis = NULL;
	mpCustomVBasis = NULL;

	if ( rhs.mpCustomUBasis )
		mpCustomUBasis = DNEW Matrix44( *rhs.mpCustomUBasis );

	if ( rhs.mpCustomVBasis )
		mpCustomVBasis = DNEW Matrix44( *rhs.mpCustomVBasis );

	mUSteps				= rhs.mUSteps				;
	mVSteps				= rhs.mVSteps				;
	mColor				= rhs.mColor				;
	mOpacity			= rhs.mOpacity				;
	moSurfaceSHI		= rhs.moSurfaceSHI 			;
	moDisplaceSHI		= rhs.moDisplaceSHI			;
	mActiveLights		= rhs.mActiveLights			;
}

//==================================================================
Attributes::~Attributes()
{
	DSAFE_DELETE( mpCustomUBasis );
	DSAFE_DELETE( mpCustomVBasis );
}

//==================================================================
void Attributes::Init(
				  State				*pState,
				  SymbolList		*pGlobalSyms,
				  ResourceManager	*pResManager,
				  RevisionTracker	*pRevision )
{
	mpState		= pState;
	mpGlobalSyms= pGlobalSyms;
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

	SVM::Shader	*pShader =
		(SVM::Shader *)pResManager->FindResource(
										"dbg_normal_col",
										ResourceBase::TYPE_SHADER );

	DASSERT( pShader != NULL );

	moSurfaceSHI = DNEW SVM::ShaderInst( pShader );
}

//==================================================================
const LightSourceT * Attributes::GetLight( size_t actLightIdx ) const
{
	size_t	lightIdx = (size_t)mActiveLights[ actLightIdx ];

	return mpState->GetLightSources()[ lightIdx ];
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
	mpTypeApproximation	= mpGlobalSyms->FindSymbol( typeApproximation );
	mValueApproximation		= valueApproximation;
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdOrientation( RtToken orientation )
{
	if ( 0 == strcasecmp( orientation, RI_INSIDE ) )
	{
		mOrientationFlipped = true;
	}
	else
	if ( 0 == strcasecmp( orientation, RI_OUTSIDE ) )
	{
		mOrientationFlipped = false;
	}
	else
	if ( 0 == strcasecmp( orientation, RI_LH ) )
	{
		mOrientationFlipped = false;
	}
	else
	if ( 0 == strcasecmp( orientation, RI_RH ) )
	{
		mOrientationFlipped = true;
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
		mpUBasis = mpGlobalSyms->FindSymbol( ubasis );
	else
	{
		DSAFE_DELETE( mpCustomUBasis );
		mpCustomUBasis = DNEW Matrix44( pCustomUBasis );
	}

	if ( vbasis )
		mpVBasis = mpGlobalSyms->FindSymbol( vbasis );
	else
	{
		DSAFE_DELETE( mpCustomVBasis );
		mpCustomVBasis = DNEW Matrix44( pCustomVBasis );
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
static void onError( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );

	throw std::runtime_error( buff );
}

//==================================================================
static void onException( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );
}

/*
//==================================================================
static void onWarning( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	puts( buff );
}
*/

//==================================================================
void Attributes::cmdLightSource( ParamList &params )
{
	const char	*pLightTypeName = "";

	// get type and ID
	if ( params.size() < 2 || !params[0].IsString() || !params[1].IsIntVal() )
	{
		throw Exception( "Bad LightSource definition !" );
		//return;
	}

	const char *pShaderName = params[0].PChar();

	SVM::Shader	*pShader = mpState->GetShader( pShaderName, NULL );

	if NOT( pShader )
	{
		throw Exception( "Could not find the light shader '%s' !", params[0].PChar() );
		//return;
	}

	LightSourceT	*pLight = DNEW LightSourceT();

/*
	if ( 0 == strcasecmp( "ambientlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_AMBIENT;
	}
	else
	if ( 0 == strcasecmp( "distantlight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_DISTANT;
	}
	else
	if ( 0 == strcasecmp( "arealight", params[0].PChar() ) )
	{
		pLight->mType = LightSourceT::TYPE_DISTANT;
	}
*/

	pLight->moShaderInst = DNEW SVM::ShaderInst( pShader, MP_GRID_MAX_SIZE );
	//pLight->moShaderInst->mMtxLocalCam = xform.GetMatrix() * mtxWorldCam;

	pLight->mID = params[1].Int();

	pLight->mIsAmbient = !pShader->mHasDirPosInstructions;

	Matrix44 mtxLocalCam = mpState->GetCurTransformOpenMtx() * mpState->GetWorldCameraMtx();
	getShaderParams( params, 2, *pLight->moShaderInst.get(), mtxLocalCam );

	size_t listIdx = mpState->AddLightSource( pLight );

	DASSERT( listIdx < 65536 );	// 16 bit limit..

	mActiveLights.find_or_push_back( (U16)listIdx );

	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdSurface( ParamList &params )
{
	if ( params.size() < 1 )
	{
		throw Exception( "Missing parameters" );
	}

	const char *pShaderName;

	if ( mpState->mParams.mForcedSurfaceShader.length() )
		pShaderName = mpState->mParams.mForcedSurfaceShader.c_str();
	else
		pShaderName = params[0].PChar();

	SVM::Shader	*pShader = mpState->GetShader( pShaderName, "matte" );

	if ( pShader )
	{
		moSurfaceSHI = DNEW SVM::ShaderInst( pShader );

		Matrix44 mtxLocalCam = mpState->GetCurTransformOpenMtx() * mpState->GetWorldCameraMtx();
		getShaderParams( params, 1, *moSurfaceSHI.get(), mtxLocalCam );
	}

	// $$$ should really check if the shader or any params really changed
	mpRevision->BumpRevision();
}

//==================================================================
void Attributes::cmdDisplacement( ParamList &params )
{
	if ( params.size() < 1 )
	{
		throw Exception( "Missing parameters" );
	}

	const char *pShaderName = params[0].PChar();

	SVM::Shader	*pShader = mpState->GetShader( pShaderName, NULL );

	if ( pShader )
	{
		moDisplaceSHI = DNEW SVM::ShaderInst( pShader );

		Matrix44 mtxLocalCam = mpState->GetCurTransformOpenMtx() * mpState->GetWorldCameraMtx();
		getShaderParams( params, 1, *moDisplaceSHI.get(), mtxLocalCam );

		// $$$ should really check if the shader or any params really changed
		mpRevision->BumpRevision();
	}
	else
	{
		throw Exception( "Failed getting the shader '%s'", pShaderName );
	}
}

//==================================================================
}
