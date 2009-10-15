//==================================================================
/// RI_State.cpp
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_State.h"
#include "RI_Primitive.h"

//==================================================================
namespace RI
{

//==================================================================
/// State
//==================================================================
State::State( const Params &params ) :
	mParams(params)
{
	mModeStack.push( MD_UNDEFINED );

	mMtxWorldCamera.Identity();

	SymbolList::SymbolParams symPar;
	symPar.mType		= Symbol::TYP_VOIDD;
	symPar.mDetail		= 0;
	symPar.mStorage		= Symbol::STOR_CONSTANT;

	symPar.mpName =		RI_FRAMEBUFFER				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_FILE						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RGB						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RGBA						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RGBZ						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RGBAZ					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_A						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_Z						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_AZ						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PERSPECTIVE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_ORTHOGRAPHIC				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_HIDDEN					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PAINT					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CONSTANT					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_SMOOTH					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_FLATNESS					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_FOV						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_AMBIENTLIGHT				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_POINTLIGHT				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_DISTANTLIGHT				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_SPOTLIGHT 				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_INTENSITY				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_LIGHTCOLOR				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_FROM						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_TO						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CONEANGLE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CONEDELTAANGLE			; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BEAMDISTRIBUTION			; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_MATTE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_METAL					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_SHINYMETAL				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PLASTIC					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PAINTEDPLASTIC			; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_KA						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_KD						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_KS						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_ROUGHNESS				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_KR						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_TEXTURENAME				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_SPECULARCOLOR			; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_DEPTHCUE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_FOG						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BUMPY					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_MINDISTANCE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_MAXDISTANCE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BACKGROUND				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_DISTANCE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_AMPLITUDE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RASTER					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_SCREEN					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CAMERA					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_WORLD					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_OBJECT					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_INSIDE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_OUTSIDE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_LH						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_RH						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_P						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PZ						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PW						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_N						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_NP						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CS						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_OS						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_S						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_T						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_ST						; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BILINEAR					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BICUBIC					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PRIMITIVE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_INTERSECTION				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_UNION					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_DIFFERENCE				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PERIODIC					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_NONPERIODIC				; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_CLAMP					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_BLACK					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_IGNORE					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_PRINT					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_ABORT					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_HANDLER					; mGlobalSyms.Add( symPar );
	symPar.mpName = 	RI_EMPTY_TOKEN				; mGlobalSyms.Add( symPar );

	symPar.mType		= Symbol::TYP_MATRIX;
	symPar.mDetail		= 0;
	symPar.mStorage		= Symbol::STOR_CONSTANT;

	symPar.mpName = RI_BEZIERBASIS				; mGlobalSyms.Add( symPar, (const void *)&BezierBasis	);
	symPar.mpName = RI_BSPLINEBASIS				; mGlobalSyms.Add( symPar, (const void *)&BSplineBasis	);
	symPar.mpName = RI_CATMULLROMBASIS			; mGlobalSyms.Add( symPar, (const void *)&CatmullRomBasis);
	symPar.mpName = RI_HERMITEBASIS				; mGlobalSyms.Add( symPar, (const void *)&HermiteBasis	);
	symPar.mpName = RI_POWERBASIS				; mGlobalSyms.Add( symPar, (const void *)&PowerBasis	);	
	
	makeDefaultShaders( mParams.mDefaultShadersDir.c_str() );

	mOptionsStack.top().Init( &mGlobalSyms, &mOptionsRevTrack );
	mAttributesStack.top().Init( this, &mGlobalSyms, &mResManager, &mAttribsRevTrack );
	mTransformOpenStack.top().Init( &mTransOpenRevTrack );
	mTransformCloseStack.top().Init( &mTransCloseRevTrack );
	
	mParams.mpFramework->SetStatics( &mGlobalSyms );
}

//==================================================================
void State::addDefShader( const char *pBasePath, const char *pSName )
{
	char	buff[1024];

	SlShader::CtorParams	params;
	params.pName			= pSName;
	params.pSourceFileName	= buff;
	params.pAppResDir		= mParams.mDefaultShadersDir.c_str();

	SlShader *pShader = NULL;
	try 
	{
#if !defined(DISABLE_SL_SHADERS)
		sprintf( buff, "%s/%s.sl", pBasePath, params.pName );
		if ( mParams.mpFileManager->FileExists( buff ) )
		{
			pShader = DNEW SlShader( params, *mParams.mpFileManager );
			mResManager.AddResource( pShader );
		}
		else
#endif
		{
			sprintf( buff, "%s/%s.rrasm", pBasePath, params.pName );
			if ( mParams.mpFileManager->FileExists( buff ) )
			{
				pShader = DNEW SlShader( params, *mParams.mpFileManager );
				mResManager.AddResource( pShader );
			}
		}
	}
	catch ( ... )
	{
		printf( "Error: Failed to load the shader %s\n", params.pName );
	}
}

//==================================================================
void State::makeDefaultShaders( const char *pBasePath )
{
	addDefShader( pBasePath, "constant"			);
	addDefShader( pBasePath, "matte"			);
	addDefShader( pBasePath, "dbg_normal_col"	);
	addDefShader( pBasePath, "test"				);
	addDefShader( pBasePath, "ambientlight"		);
	addDefShader( pBasePath, "distantlight"		);
	addDefShader( pBasePath, "arealight"		);
}

//==================================================================
State::~State()
{
	mModeStack.clear();
	mOptionsStack.clear();
	mAttributesStack.clear();
	mTransformOpenStack.clear();
	mTransformCloseStack.clear();
}

//==================================================================
void State::pushStacks( const u_int flags )
{
	if ( flags & SF_OPTS )	mOptionsStack.push();
	if ( flags & SF_ATRB )	mAttributesStack.push();
	if ( flags & SF_TRAN )	mTransformOpenStack.push(), mTransformCloseStack.push();
}

//==================================================================
void State::popStacks( const u_int flags )
{
	if ( flags & SF_OPTS )	mOptionsStack.pop();
	if ( flags & SF_ATRB )	mAttributesStack.pop();
	if ( flags & SF_TRAN )	mTransformOpenStack.pop(), mTransformCloseStack.pop();
}

//==================================================================
void State::Begin( RtToken name )
{
	pushMode( MD_BASE );
}
//==================================================================
void State::End()
{
	popMode( MD_BASE );
}
//==================================================================
void State::FrameBegin( int frame )
{
	pushMode( MD_FRAME );
	pushStacks( SF_OPTS | SF_ATRB | SF_TRAN );
}
//==================================================================
void State::FrameEnd()
{
	popStacks( SF_OPTS | SF_ATRB | SF_TRAN );
	popMode( MD_FRAME );
}
//==================================================================
void State::WorldBegin()
{
	mOptionsStack.top().Finalize();

	pushMode( MD_WORLD );

	// store the current (camera) transformation
	mMtxWorldCamera = mTransformOpenStack.top().GetMatrix();

	pushStacks( SF_OPTS | SF_ATRB | SF_TRAN );

	// initialize the world transformation
	mTransformOpenStack.top().SetIdentity();
	mTransformCloseStack.top().SetIdentity();

	mParams.mpFramework->WorldBegin( mOptionsStack.top(), mMtxWorldCamera );
}
//==================================================================
void State::WorldEnd()
{
	mParams.mpFramework->WorldEnd();

	popStacks( SF_OPTS | SF_ATRB | SF_TRAN );
	popMode( MD_WORLD );
}
//==================================================================
void State::AttributeBegin()
{
	pushMode( MD_ATTRIBUTE );
	pushStacks( SF_ATRB | SF_TRAN );
}
//==================================================================
void State::AttributeEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_ATTRIBUTE );
}
//==================================================================
void State::TransformBegin()
{
	//printf( ">> " );
	//mTransformOpenStack.top().mMatrix.PrintOut();

	pushMode( MD_TRANSFORM );
	pushStacks( SF_TRAN );
}
//==================================================================
void State::TransformEnd()
{
	popStacks( SF_TRAN );
	popMode( MD_TRANSFORM );

	//printf( "<< " );
	//mTransformOpenStack.top().mMatrix.PrintOut();
}
//==================================================================
void State::SolidBegin( RtToken operation )
{
	pushMode( MD_SOLID );
	pushStacks( SF_ATRB | SF_TRAN );
}
//==================================================================
void State::SolidEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_SOLID );
}
//==================================================================
ObjectHandle State::ObjectBegin()
{
	pushMode( MD_OBJECT );
	pushStacks( SF_ATRB | SF_TRAN );

	return NULL;
}
//==================================================================
void State::ObjectEnd()
{
	popStacks( SF_ATRB | SF_TRAN );
	popMode( MD_OBJECT );
}
//==================================================================
void State::MotionBegin( int n, const float times[] )
{
	pushMode( MD_MOTION );
}
//==================================================================
void State::MotionEnd()
{
	popMode( MD_MOTION );
}

// setting attributes
//==================================================================
void State::DoBound( const Bound &bound )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdBound( bound );
}
//==================================================================
void State::Detail( const Bound &detail )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdDetail( detail );
}
//==================================================================
void State::DetailRange(float	minVisible,
				 float	lowerTransition,
				 float	upperTransition,
				 float	maxVisible )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdDetailRange(
					minVisible,
					lowerTransition,
					upperTransition,
					maxVisible );
}
//==================================================================
void State::GeometricApproximation(RtToken typeApproximation,
							   float valueApproximation )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdGeometricApproximation(
							typeApproximation,
							valueApproximation );
}
//==================================================================
void State::Orientation( RtToken orientation )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdOrientation( orientation );
}
//==================================================================
void State::Sides( int sides )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;
			
	mAttributesStack.top().cmdSides( sides );
}

//==================================================================
void State::Basis( RtToken ubasis, const float *pCustomUBasis, int ustep,
				   RtToken vbasis, const float *pCustomVBasis, int vstep )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	DASSERT( (ubasis || pCustomUBasis) && (vbasis || pCustomVBasis) );

	if ( (ubasis && !verifyBasis( ubasis, ustep )) ||
		 (vbasis && !verifyBasis( vbasis, vstep )) )
		return;

	mAttributesStack.top().cmdBasis(
						ubasis, pCustomUBasis, ustep,
						vbasis, pCustomVBasis, vstep );
}

//==================================================================
void State::ColorSet( const Color &col )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdColor( col );
}

//==================================================================
void State::Opacity( const Color &col )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdOpacity( col );
}

//==================================================================
void State::AreaLightSource( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdLightSource(
								params,
								mTransformOpenStack.top(),
								mMtxWorldCamera );
}

//==================================================================
void State::LightSource( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdLightSource(
								params,
								mTransformOpenStack.top(),
								mMtxWorldCamera );
}

//==================================================================
void State::Declare( ParamList &params )
{
	//mGlobalSyms.Add( );
	//mAttributesStack.top().cmdSurface( params );
}

//==================================================================
void State::Surface( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdSurface( params );
}

//==================================================================
void State::Displacement( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdDisplacement( params );
}

// options
//==================================================================
void State::Format( int xRes, int yRes, float pixelRatio )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdFormat( xRes, yRes, pixelRatio );
}
//==================================================================
void State::FrameAspectRatio( float ratio )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdFrameAspectRatio( ratio );
}
//==================================================================
void State::ScreenWindow( float left, float right, float bottom, float top )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdScreenWindow( left, right, bottom, top );
}
//==================================================================
void State::CropWindow( float xMin, float xMax, float yMin, float yMax )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdCropWindow( xMin, xMax, yMin, yMax );
}
//==================================================================
void State::Projection( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdProjection( params );
}
//==================================================================
void State::Clipping( float near, float farr )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdClipping( near, farr );
}
//==================================================================
void State::DepthOfField( float fStop, float focalLength, float focalDistance )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdDepthOfField( fStop, focalLength, focalDistance );
}
//==================================================================
void State::Shutter( float openShutter, float closeShutter )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdShutter( openShutter, closeShutter );
}

//==================================================================
void State::Identity()
{
	mTransformOpenStack.top().SetIdentity();
}

//==================================================================
void State::ConcatTransform( const Matrix44 &mtxLeft )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	mTransformOpenStack.top().ConcatTransform( mtxLeft );
}

//==================================================================
void State::TransformCmd( const float *pMtx )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	mTransformOpenStack.top().CopyRowMajor( pMtx );
}

//==================================================================
void State::Scale( float sx, float sy, float sz )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	mTransformOpenStack.top().ConcatTransform( Matrix44::Scale( sx, sy, sz ) );
}

//==================================================================
void State::Rotate( float angDeg, float ax, float ay, float az )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	mTransformOpenStack.top().ConcatTransform( Matrix44::Rot( angDeg * DEG2RAD, ax, ay, az ) );
}

//==================================================================
void State::Translate( float tx, float ty, float tz )
{
	if NOT( verifyOpType( OPTYPE_STD_XFORM ) )
		return;

	mTransformOpenStack.top().ConcatTransform( Matrix44::Translate( tx, ty, tz ) );
}

//==================================================================
size_t State::AddLightSource( LightSourceT *pLSource )
{
	for (size_t i=0; i < mpLightSources.size(); ++i)
	{
		if ( pLSource->mID == mpLightSources[i]->mID )
		{
			mpLightSources[i] = pLSource;
			return i;
		}
	}

	// add if didn't replace any existing light
	mpLightSources.push_back( pLSource );
	return mpLightSources.size() - 1;
}

//==================================================================
}
