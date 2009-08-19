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
State::State(
		FrameworkREYES *pFramework,
		FileManagerBase	*pFileManager,
		const char *pBaseDir,
		const char *pDefaultShadersDir ) :
	mpFramework(pFramework),
	mpFileManager(pFileManager),
	mBaseDir(pBaseDir),
	mDefaultShadersDir(pDefaultShadersDir)
{
	mModeStack.push( MD_UNDEFINED );

	mMtxWorldCamera.Identity();

	mStatics.FindOrAdd(	RI_FRAMEBUFFER				);
	mStatics.FindOrAdd(	RI_FILE						);
	mStatics.FindOrAdd(	RI_RGB						);
	mStatics.FindOrAdd(	RI_RGBA						);
	mStatics.FindOrAdd(	RI_RGBZ						);
	mStatics.FindOrAdd(	RI_RGBAZ					);
	mStatics.FindOrAdd(	RI_A						);
	mStatics.FindOrAdd(	RI_Z						);
	mStatics.FindOrAdd(	RI_AZ						);
	mStatics.FindOrAdd(	RI_PERSPECTIVE				);
	mStatics.FindOrAdd(	RI_ORTHOGRAPHIC				);
	mStatics.FindOrAdd(	RI_HIDDEN					);
	mStatics.FindOrAdd(	RI_PAINT					);
	mStatics.FindOrAdd(	RI_CONSTANT					);
	mStatics.FindOrAdd(	RI_SMOOTH					);
	mStatics.FindOrAdd(	RI_FLATNESS					);
	mStatics.FindOrAdd(	RI_FOV						);
	mStatics.FindOrAdd(	RI_AMBIENTLIGHT				);
	mStatics.FindOrAdd(	RI_POINTLIGHT				);
	mStatics.FindOrAdd(	RI_DISTANTLIGHT				);
	mStatics.FindOrAdd(	RI_SPOTLIGHT 				);
	mStatics.FindOrAdd(	RI_INTENSITY				);
	mStatics.FindOrAdd(	RI_LIGHTCOLOR				);
	mStatics.FindOrAdd(	RI_FROM						);
	mStatics.FindOrAdd(	RI_TO						);
	mStatics.FindOrAdd(	RI_CONEANGLE				);
	mStatics.FindOrAdd(	RI_CONEDELTAANGLE			);
	mStatics.FindOrAdd(	RI_BEAMDISTRIBUTION			);
	mStatics.FindOrAdd(	RI_MATTE					);
	mStatics.FindOrAdd(	RI_METAL					);
	mStatics.FindOrAdd(	RI_SHINYMETAL				);
	mStatics.FindOrAdd(	RI_PLASTIC					);
	mStatics.FindOrAdd(	RI_PAINTEDPLASTIC			);
	mStatics.FindOrAdd(	RI_KA						);
	mStatics.FindOrAdd(	RI_KD						);
	mStatics.FindOrAdd(	RI_KS						);
	mStatics.FindOrAdd(	RI_ROUGHNESS				);
	mStatics.FindOrAdd(	RI_KR						);
	mStatics.FindOrAdd(	RI_TEXTURENAME				);
	mStatics.FindOrAdd(	RI_SPECULARCOLOR			);
	mStatics.FindOrAdd(	RI_DEPTHCUE					);
	mStatics.FindOrAdd(	RI_FOG						);
	mStatics.FindOrAdd(	RI_BUMPY					);
	mStatics.FindOrAdd(	RI_MINDISTANCE				);
	mStatics.FindOrAdd(	RI_MAXDISTANCE				);
	mStatics.FindOrAdd(	RI_BACKGROUND				);
	mStatics.FindOrAdd(	RI_DISTANCE					);
	mStatics.FindOrAdd(	RI_AMPLITUDE				);
	mStatics.FindOrAdd(	RI_RASTER					);
	mStatics.FindOrAdd(	RI_SCREEN					);
	mStatics.FindOrAdd(	RI_CAMERA					);
	mStatics.FindOrAdd(	RI_WORLD					);
	mStatics.FindOrAdd(	RI_OBJECT					);
	mStatics.FindOrAdd(	RI_INSIDE					);
	mStatics.FindOrAdd(	RI_OUTSIDE					);
	mStatics.FindOrAdd(	RI_LH						);
	mStatics.FindOrAdd(	RI_RH						);
	mStatics.FindOrAdd(	RI_P						);
	mStatics.FindOrAdd(	RI_PZ						);
	mStatics.FindOrAdd(	RI_PW						);
	mStatics.FindOrAdd(	RI_N						);
	mStatics.FindOrAdd(	RI_NP						);
	mStatics.FindOrAdd(	RI_CS						);
	mStatics.FindOrAdd(	RI_OS						);
	mStatics.FindOrAdd(	RI_S						);
	mStatics.FindOrAdd(	RI_T						);
	mStatics.FindOrAdd(	RI_ST						);
	mStatics.FindOrAdd(	RI_BILINEAR					);
	mStatics.FindOrAdd(	RI_BICUBIC					);
	mStatics.FindOrAdd(	RI_PRIMITIVE				);
	mStatics.FindOrAdd(	RI_INTERSECTION				);
	mStatics.FindOrAdd(	RI_UNION					);
	mStatics.FindOrAdd(	RI_DIFFERENCE				);
	mStatics.FindOrAdd(	RI_PERIODIC					);
	mStatics.FindOrAdd(	RI_NONPERIODIC				);
	mStatics.FindOrAdd(	RI_CLAMP					);
	mStatics.FindOrAdd(	RI_BLACK					);
	mStatics.FindOrAdd(	RI_IGNORE					);
	mStatics.FindOrAdd(	RI_PRINT					);
	mStatics.FindOrAdd(	RI_ABORT					);
	mStatics.FindOrAdd(	RI_HANDLER					);
	mStatics.FindOrAdd(	RI_EMPTY_TOKEN				);
	mStatics.FindOrAdd(	RI_BEZIERBASIS				, BezierBasis		);
	mStatics.FindOrAdd(	RI_BSPLINEBASIS				, BSplineBasis		);
	mStatics.FindOrAdd(	RI_CATMULLROMBASIS			, CatmullRomBasis	);
	mStatics.FindOrAdd(	RI_HERMITEBASIS				, HermiteBasis		);
	mStatics.FindOrAdd(	RI_POWERBASIS				, PowerBasis		);	
	
	makeDefaultShaders( mDefaultShadersDir.c_str() );

	mOptionsStack.top().Init( &mStatics, &mOptionsRevTrack );
	mAttributesStack.top().Init( this, &mStatics, &mResManager, &mAttribsRevTrack );
	mTransformOpenStack.top().Init( &mTransOpenRevTrack );
	mTransformCloseStack.top().Init( &mTransCloseRevTrack );
	
	mpFramework->SetStatics( &mStatics );
}

//==================================================================
void State::addDefShader( const char *pBasePath, const char *pSName )
{
	char	buff[1024];

	SlShader::CtorParams	params;
	params.pName			= pSName;
	params.pSourceFileName	= buff;
	params.pAppResDir		= mDefaultShadersDir.c_str();

	SlShader *pShader = NULL;
	try 
	{
		sprintf( buff, "%s/%s.sl", pBasePath, params.pName );
		
		if ( mpFileManager->FileExists( buff ) )
		{
			pShader = DNEW SlShader( params, *mpFileManager );
			mResManager.AddResource( pShader );
		}
		else
		{
			sprintf( buff, "%s/%s.rrasm", pBasePath, params.pName );
			if ( mpFileManager->FileExists( buff ) )
			{
				pShader = DNEW SlShader( params, *mpFileManager );
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

	mpFramework->WorldBegin( mOptionsStack.top(), mMtxWorldCamera );
}
//==================================================================
void State::WorldEnd()
{
	mpFramework->WorldEnd();

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
