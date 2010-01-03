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
	// normalize the directories in input...
	// to simplify the usage later on, the current directory
	// needs to be explicitly "."
	if ( mParams.mBaseDir == "" )
		mParams.mBaseDir = ".";

	if ( mParams.mDefaultShadersDir == "" )
		mParams.mDefaultShadersDir = ".";

	mModeStack.push( MD_UNDEFINED );

	mMtxWorldCamera.Identity();

	mGlobalSyms.AddGVoid(	RI_FRAMEBUFFER		);
	mGlobalSyms.AddGVoid( 	RI_FILE				);
	mGlobalSyms.AddGVoid( 	RI_RGB				);
	mGlobalSyms.AddGVoid( 	RI_RGBA				);
	mGlobalSyms.AddGVoid( 	RI_RGBZ				);
	mGlobalSyms.AddGVoid( 	RI_RGBAZ			);
	mGlobalSyms.AddGVoid( 	RI_A				);
	mGlobalSyms.AddGVoid( 	RI_Z				);
	mGlobalSyms.AddGVoid( 	RI_AZ				);
	mGlobalSyms.AddGVoid( 	RI_PERSPECTIVE		);
	mGlobalSyms.AddGVoid( 	RI_ORTHOGRAPHIC		);
	mGlobalSyms.AddGVoid( 	RI_HIDDEN			);
	mGlobalSyms.AddGVoid( 	RI_PAINT			);
	mGlobalSyms.AddGVoid( 	RI_CONSTANT			);
	mGlobalSyms.AddGVoid( 	RI_SMOOTH			);
	mGlobalSyms.AddGVoid( 	RI_FLATNESS			);
	mGlobalSyms.AddGVoid( 	RI_FOV				);
	mGlobalSyms.AddGVoid( 	RI_AMBIENTLIGHT		);
	mGlobalSyms.AddGVoid( 	RI_POINTLIGHT		);
	mGlobalSyms.AddGVoid( 	RI_DISTANTLIGHT		);
	mGlobalSyms.AddGVoid( 	RI_SPOTLIGHT 		);
	mGlobalSyms.AddGlob( 	"uniform float ", RI_INTENSITY		 );
	mGlobalSyms.AddGlob( 	"uniform color ", RI_LIGHTCOLOR		 );
	mGlobalSyms.AddGlob( 	"uniform point ", RI_FROM			 );
	mGlobalSyms.AddGlob( 	"uniform point ", RI_TO				 );
	mGlobalSyms.AddGlob( 	"uniform float ", RI_CONEANGLE		 );
	mGlobalSyms.AddGlob( 	"uniform float ", RI_CONEDELTAANGLE	 );
	mGlobalSyms.AddGlob( 	"uniform float ", RI_BEAMDISTRIBUTION );
	mGlobalSyms.AddGVoid( 	RI_MATTE			);
	mGlobalSyms.AddGVoid( 	RI_METAL			);
	mGlobalSyms.AddGVoid( 	RI_SHINYMETAL		);
	mGlobalSyms.AddGVoid( 	RI_PLASTIC			);
	mGlobalSyms.AddGVoid( 	RI_PAINTEDPLASTIC	);
	mGlobalSyms.AddGlob( 	"uniform float  ",  RI_KA			);
	mGlobalSyms.AddGlob( 	"uniform float  ",  RI_KD			);
	mGlobalSyms.AddGlob( 	"uniform float  ",  RI_KS			);
	mGlobalSyms.AddGlob( 	"uniform float  ",  RI_ROUGHNESS		);
	mGlobalSyms.AddGlob( 	"uniform float  ",  RI_KR			);
	mGlobalSyms.AddGlob( 	"uniform string ",  RI_TEXTURENAME	);
	mGlobalSyms.AddGlob( 	"uniform color  ",  RI_SPECULARCOLOR	);
	mGlobalSyms.AddGVoid( 	RI_DEPTHCUE			);
	mGlobalSyms.AddGVoid( 	RI_FOG				);
	mGlobalSyms.AddGVoid( 	RI_BUMPY			);
	mGlobalSyms.AddGlob( 	"uniform float ", RI_MINDISTANCE );
	mGlobalSyms.AddGlob( 	"uniform float ", RI_MAXDISTANCE );
	mGlobalSyms.AddGlob( 	"uniform color ", RI_BACKGROUND  );
	mGlobalSyms.AddGlob( 	"uniform float ", RI_DISTANCE	);
	mGlobalSyms.AddGlob( 	"uniform float ", RI_AMPLITUDE   );
	mGlobalSyms.AddGVoid( 	RI_RASTER			);
	mGlobalSyms.AddGVoid( 	RI_SCREEN			);
	mGlobalSyms.AddGVoid( 	RI_CAMERA			);
	mGlobalSyms.AddGVoid( 	RI_WORLD			);
	mGlobalSyms.AddGVoid( 	RI_OBJECT			);
	mGlobalSyms.AddGVoid( 	RI_INSIDE			);
	mGlobalSyms.AddGVoid( 	RI_OUTSIDE			);
	mGlobalSyms.AddGVoid( 	RI_LH				);
	mGlobalSyms.AddGVoid( 	RI_RH				);
	mGlobalSyms.AddGlob( 	"vertex point     ", RI_P	);		// verified
	mGlobalSyms.AddGlob( 	"vertex float     ", RI_PZ	);		// verified
	mGlobalSyms.AddGlob( 	"vertex hpoint    ", RI_PW	);		// verified
	mGlobalSyms.AddGlob( 	"varying normal   ", RI_N	);		// verified
	mGlobalSyms.AddGlob( 	"uniform point    ", RI_NP	);
	mGlobalSyms.AddGlob( 	"varying color    ", RI_CS	);		// verified
	mGlobalSyms.AddGlob( 	"varying color    ", RI_OS	);		// verified
	mGlobalSyms.AddGlob( 	"varying float    ", RI_S	);		// verified
	mGlobalSyms.AddGlob( 	"varying float    ", RI_T    );		// verified
	//mGlobalSyms.AddGlob( 	"varying float[2] ", RI_ST	);		// verified
	mGlobalSyms.AddGVoid( 	RI_BILINEAR			);
	mGlobalSyms.AddGVoid( 	RI_BICUBIC			);
	mGlobalSyms.AddGVoid( 	RI_PRIMITIVE		);
	mGlobalSyms.AddGVoid( 	RI_INTERSECTION		);
	mGlobalSyms.AddGVoid( 	RI_UNION			);
	mGlobalSyms.AddGVoid( 	RI_DIFFERENCE		);
	mGlobalSyms.AddGVoid( 	RI_PERIODIC			);
	mGlobalSyms.AddGVoid( 	RI_NONPERIODIC		);
	mGlobalSyms.AddGVoid( 	RI_CLAMP			);
	mGlobalSyms.AddGVoid( 	RI_BLACK			);
	mGlobalSyms.AddGVoid( 	RI_IGNORE			);
	mGlobalSyms.AddGVoid( 	RI_PRINT			);
	mGlobalSyms.AddGVoid( 	RI_ABORT			);
	mGlobalSyms.AddGVoid( 	RI_HANDLER			);
	mGlobalSyms.AddGVoid( 	RI_EMPTY_TOKEN		);

	mGlobalSyms.AddGlob( "constant matrix ", RI_BEZIERBASIS		, (const void *)&BezierBasis	);
	mGlobalSyms.AddGlob( "constant matrix ", RI_BSPLINEBASIS	, (const void *)&BSplineBasis	);
	mGlobalSyms.AddGlob( "constant matrix ", RI_CATMULLROMBASIS	, (const void *)&CatmullRomBasis);
	mGlobalSyms.AddGlob( "constant matrix ", RI_HERMITEBASIS	, (const void *)&HermiteBasis	);
	mGlobalSyms.AddGlob( "constant matrix ", RI_POWERBASIS		, (const void *)&PowerBasis		);
	
	// add shader-specific globals that for some reasons are not listed in ri.h
	mGlobalSyms.AddGlob( "varying point  dPdu"	);
	mGlobalSyms.AddGlob( "varying point  dPdv"	);
	mGlobalSyms.AddGlob( "varying float  _oodu"	);
	mGlobalSyms.AddGlob( "varying float  _oodv"	);
	mGlobalSyms.AddGlob( "varying normal Ng"	);
	mGlobalSyms.AddGlob( "varying float  u"		);
	mGlobalSyms.AddGlob( "varying float  v"		);
	mGlobalSyms.AddGlob( "varying float  du"	);
	mGlobalSyms.AddGlob( "varying float  dv"	);
	mGlobalSyms.AddGlob( "varying vector L"		);
	mGlobalSyms.AddGlob( "varying color  Cl"	);
	mGlobalSyms.AddGlob( "varying vector I"		);
	mGlobalSyms.AddGlob( "varying color  Ci"	);
	mGlobalSyms.AddGlob( "varying color  Oi"	);
	mGlobalSyms.AddGlob( "uniform point  E"		);

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
		sprintf( buff, "%s/%s.sl", pBasePath, params.pName );
		if ( mParams.mpFileManager->FileExists( buff ) )
		{
			pShader = DNEW SlShader( params, *mParams.mpFileManager );
			mResManager.AddResource( pShader );
		}
		else
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
	//addDefShader( pBasePath, "ambientlight"		);
	//addDefShader( pBasePath, "distantlight"		);
	//addDefShader( pBasePath, "arealight"		);
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

	mAttributesStack.top().cmdLightSource( params );
}

//==================================================================
void State::LightSource( ParamList &params )
{
	if NOT( verifyOpType( OPTYPE_ATRB ) )
		return;

	mAttributesStack.top().cmdLightSource( params );
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
void State::PixelSamples( int samplesX, int samplesY )
{
	if NOT( verifyOpType( OPTYPE_OPTS ) )
		return;

	mOptionsStack.top().cmdPixelSamples( samplesX, samplesY );
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
