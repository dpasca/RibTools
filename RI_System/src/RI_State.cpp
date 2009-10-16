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

	Symbol::CtorParams symPar;
	symPar.mType		= Symbol::TYP_VOIDD;
	symPar.mDetail		= 0;
	symPar.mStorage		= Symbol::STOR_CONSTANT;

	mGlobalSyms.Add(	RI_FRAMEBUFFER		);
	mGlobalSyms.Add( 	RI_FILE				);
	mGlobalSyms.Add( 	RI_RGB				);
	mGlobalSyms.Add( 	RI_RGBA				);
	mGlobalSyms.Add( 	RI_RGBZ				);
	mGlobalSyms.Add( 	RI_RGBAZ			);
	mGlobalSyms.Add( 	RI_A				);
	mGlobalSyms.Add( 	RI_Z				);
	mGlobalSyms.Add( 	RI_AZ				);
	mGlobalSyms.Add( 	RI_PERSPECTIVE		);
	mGlobalSyms.Add( 	RI_ORTHOGRAPHIC		);
	mGlobalSyms.Add( 	RI_HIDDEN			);
	mGlobalSyms.Add( 	RI_PAINT			);
	mGlobalSyms.Add( 	RI_CONSTANT			);
	mGlobalSyms.Add( 	RI_SMOOTH			);
	mGlobalSyms.Add( 	RI_FLATNESS			);
	mGlobalSyms.Add( 	RI_FOV				);
	mGlobalSyms.Add( 	RI_AMBIENTLIGHT		);
	mGlobalSyms.Add( 	RI_POINTLIGHT		);
	mGlobalSyms.Add( 	RI_DISTANTLIGHT		);
	mGlobalSyms.Add( 	RI_SPOTLIGHT 		);
	mGlobalSyms.Add( 	RI_INTENSITY		, "uniform float" );
	mGlobalSyms.Add( 	RI_LIGHTCOLOR		, "uniform color" );
	mGlobalSyms.Add( 	RI_FROM				, "uniform point" );
	mGlobalSyms.Add( 	RI_TO				, "uniform point" );
	mGlobalSyms.Add( 	RI_CONEANGLE		, "uniform float" );
	mGlobalSyms.Add( 	RI_CONEDELTAANGLE	, "uniform float" );
	mGlobalSyms.Add( 	RI_BEAMDISTRIBUTION	, "uniform float" );
	mGlobalSyms.Add( 	RI_MATTE			);
	mGlobalSyms.Add( 	RI_METAL			);
	mGlobalSyms.Add( 	RI_SHINYMETAL		);
	mGlobalSyms.Add( 	RI_PLASTIC			);
	mGlobalSyms.Add( 	RI_PAINTEDPLASTIC	);
	mGlobalSyms.Add( 	RI_KA				, "uniform float" );
	mGlobalSyms.Add( 	RI_KD				, "uniform float" );
	mGlobalSyms.Add( 	RI_KS				, "uniform float" );
	mGlobalSyms.Add( 	RI_ROUGHNESS		, "uniform float" );
	mGlobalSyms.Add( 	RI_KR				, "uniform float" );
	mGlobalSyms.Add( 	RI_TEXTURENAME		, "uniform string" );
	mGlobalSyms.Add( 	RI_SPECULARCOLOR	, "uniform color" );
	mGlobalSyms.Add( 	RI_DEPTHCUE			);
	mGlobalSyms.Add( 	RI_FOG				);
	mGlobalSyms.Add( 	RI_BUMPY			);
	mGlobalSyms.Add( 	RI_MINDISTANCE		, "uniform float" );
	mGlobalSyms.Add( 	RI_MAXDISTANCE		, "uniform float" );
	mGlobalSyms.Add( 	RI_BACKGROUND		, "uniform color" );
	mGlobalSyms.Add( 	RI_DISTANCE			, "uniform float" );
	mGlobalSyms.Add( 	RI_AMPLITUDE		, "uniform float" );
	mGlobalSyms.Add( 	RI_RASTER			);
	mGlobalSyms.Add( 	RI_SCREEN			);
	mGlobalSyms.Add( 	RI_CAMERA			);
	mGlobalSyms.Add( 	RI_WORLD			);
	mGlobalSyms.Add( 	RI_OBJECT			);
	mGlobalSyms.Add( 	RI_INSIDE			);
	mGlobalSyms.Add( 	RI_OUTSIDE			);
	mGlobalSyms.Add( 	RI_LH				);
	mGlobalSyms.Add( 	RI_RH				);
	mGlobalSyms.Add( 	RI_P				, "vertex point" );		// verified
	mGlobalSyms.Add( 	RI_PZ				, "vertex float" );		// verified
	mGlobalSyms.Add( 	RI_PW				, "vertex hpoint" );		// verified
	mGlobalSyms.Add( 	RI_N				, "varying normal" );		// verified
	mGlobalSyms.Add( 	RI_NP				, "uniform point" );
	mGlobalSyms.Add( 	RI_CS				, "varying color" );		// verified
	mGlobalSyms.Add( 	RI_OS				, "varying color" );		// verified
	mGlobalSyms.Add( 	RI_S				, "varying float" );		// verified
	mGlobalSyms.Add( 	RI_T				, "varying float" );		// verified
	//mGlobalSyms.Add( 	RI_ST				, "varying float[2]" );		// verified
	mGlobalSyms.Add( 	RI_BILINEAR			);
	mGlobalSyms.Add( 	RI_BICUBIC			);
	mGlobalSyms.Add( 	RI_PRIMITIVE		);
	mGlobalSyms.Add( 	RI_INTERSECTION		);
	mGlobalSyms.Add( 	RI_UNION			);
	mGlobalSyms.Add( 	RI_DIFFERENCE		);
	mGlobalSyms.Add( 	RI_PERIODIC			);
	mGlobalSyms.Add( 	RI_NONPERIODIC		);
	mGlobalSyms.Add( 	RI_CLAMP			);
	mGlobalSyms.Add( 	RI_BLACK			);
	mGlobalSyms.Add( 	RI_IGNORE			);
	mGlobalSyms.Add( 	RI_PRINT			);
	mGlobalSyms.Add( 	RI_ABORT			);
	mGlobalSyms.Add( 	RI_HANDLER			);
	mGlobalSyms.Add( 	RI_EMPTY_TOKEN		);

	mGlobalSyms.Add( RI_BEZIERBASIS			, "constant matrix" , (const void *)&BezierBasis	 );
	mGlobalSyms.Add( RI_BSPLINEBASIS		, "constant matrix" , (const void *)&BSplineBasis	 );
	mGlobalSyms.Add( RI_CATMULLROMBASIS		, "constant matrix" , (const void *)&CatmullRomBasis );
	mGlobalSyms.Add( RI_HERMITEBASIS		, "constant matrix" , (const void *)&HermiteBasis	 );
	mGlobalSyms.Add( RI_POWERBASIS			, "constant matrix" , (const void *)&PowerBasis		 );	
	
	// add shader-specific globals that for some reasons are not listed in ri.h
	mGlobalSyms.Add( "dPdu",	"varying point"		);
	mGlobalSyms.Add( "dPdv",	"varying point"		);
	mGlobalSyms.Add( "_oodu",	"varying float"		);
	mGlobalSyms.Add( "_oodv",	"varying float"		);
	mGlobalSyms.Add( "Ng",		"varying normal"	);
	mGlobalSyms.Add( "u",		"varying float"		);
	mGlobalSyms.Add( "v",		"varying float"		);
	mGlobalSyms.Add( "du",		"varying float"		);
	mGlobalSyms.Add( "dv",		"varying float"		);
	mGlobalSyms.Add( "L",		"varying vector"	);
	mGlobalSyms.Add( "Cl",		"varying color"		);
	mGlobalSyms.Add( "I",		"varying vector"	);
	mGlobalSyms.Add( "Ci",		"varying color"		);
	mGlobalSyms.Add( "Oi",		"varying color"		);
	mGlobalSyms.Add( "E",		"uniform point"		);


	makeDefaultShaders( mParams.mDefaultShadersDir.c_str() );

	mOptionsStack.top().Init( &mGlobalSyms, &mOptionsRevTrack );
	mAttributesStack.top().Init( this, &mGlobalSyms, &mResManager, &mAttribsRevTrack );
	mTransformOpenStack.top().Init( &mTransOpenRevTrack );
	mTransformCloseStack.top().Init( &mTransCloseRevTrack );
	
	mParams.mpFramework->SetGlobalSyms( &mGlobalSyms );
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
