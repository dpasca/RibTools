//==================================================================
/// RI_Options.cpp
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Base.h"
#include "RI_Options.h"
#include "RI_Tokens.h"
#include <stdarg.h>

//==================================================================
#define DEFAULT_ASPECT	1.0f //(4.0f/3)
#define NOTSET_FLOAT	FLT_MAX

//==================================================================
namespace RI
{

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
Options::Options() :
	mpGlobalSyms(NULL),
	mpRevision(NULL)
{
}

//==================================================================
void Options::Init( const SymbolList *pGlobalSyms, RevisionTracker *pRevision )
{
	mpGlobalSyms = pGlobalSyms;
	mpRevision = pRevision;
	mXRes = 640;
	mYRes = 480;
	mPixelAspectRatio = 1.0f;
	
	mFrameAspectRatio = NOTSET_FLOAT;
	mLeft	= NOTSET_FLOAT;
	mRight	= NOTSET_FLOAT;
	mBottom = NOTSET_FLOAT;
	mTop	= NOTSET_FLOAT;

	mXMin = 0;
	mXMax = 1;
	mYMin = 0;
	mYMax = 1;
	mMtxCamProj = true;
	mNearClip = RI_EPSILON;
	mFarClip = RI_INFINITY;
	mFStop = RI_INFINITY;
	mFocalLength = 0;
	mFocalDistance = 0;
	mOpenShutter = 0;
	mCloseShutter = 0;

	ParamList	plist;
	plist.Add( RI_ORTHOGRAPHIC );
	cmdProjection( plist );

	mPixSamples[0] = 2;
	mPixSamples[1] = 2;
}

//==================================================================
Options::~Options()
{
}

/*
//==================================================================
Options &Options::operator=(const Options &options)
{
	*this = options;
}
*/

//==================================================================
void Options::cmdFormat( int xRes, int yRes, float pixelRatio )
{
	mXRes			= xRes;
	mYRes			= yRes;
	mPixelAspectRatio= pixelRatio;
	
	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdFrameAspectRatio( float ratio )
{
	mFrameAspectRatio	= ratio;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdScreenWindow( float left, float right, float bottom, float top )
{
	mLeft	= left;
	mRight	= right;
	mBottom	= bottom;
	mTop	= top;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdCropWindow( float xMin, float xMax, float yMin, float yMax )
{
	mXMin	= xMin;
	mXMax	= xMax;
	mYMin	= yMin;
	mYMax	= yMax;
/*	
	rxmin = clamp (ceil ( xresolution*xmin ), 0, xresolution-1); 
	rxmax = clamp (ceil ( xresolution*xmax -1 ), 0, xresolution-1); 
	rymin = clamp (ceil ( yresolution*ymin ), 0, yresolution-1); 
	rymax = clamp (ceil ( yresolution*ymax -1 ), 0, yresolution-1); 	
*/

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdProjection( ParamList &params )
{
	mProjectionParams = params;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdClipping( float nearr, float farr )
{
	DASSERT( nearr < farr );

	mNearClip	= nearr;
	mFarClip	= farr;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdDepthOfField( float fStop, float focalLength, float focalDistance )
{
	mFStop			= fStop;
	mFocalLength	= focalLength;
	mFocalDistance	= focalDistance;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdShutter( float openShutter, float closeShutter )
{
	mOpenShutter	= openShutter;
	mCloseShutter	= closeShutter;

	mpRevision->BumpRevision();
}

//==================================================================
void Options::cmdDisplay( const char *pName, const char *pType, const char *pMode, ParamList &params )
{
	size_t len = strlen( pName );

	bool appending = false;

	if ( len && pName[0] == '+' )
	{
		appending = true;
		pName += 1;
		len -= 1;
	}

	// assign a dummy name if necessary
	if NOT( len )
		pName = "no_name";

	if NOT( appending )
	{
		for (size_t i=0; i < mpDisplays.size(); ++i)
			DSAFE_DELETE( mpDisplays[i] );

		mpDisplays.clear();
	}

	Display	*pNewDisp = DNEW Display();
	mpDisplays.push_back( pNewDisp );

	if ( 0 == strcmp( pType, RI_FRAMEBUFFER ) )
	{
		pNewDisp->mType = RI_FRAMEBUFFER;
	}
	else
	if ( 0 == strcmp( pType, RI_FILE ) || 0 == strcmp( pType, "tiff" ) )
	{
		pNewDisp->mType = RI_FILE;
	}
	else
	{
		onError( "Unknown display type '%s'", pType );
	}

	pNewDisp->mMode = pMode;	// setup the mode (rgb, rgba, ...)
	pNewDisp->mName = pName;	// setup the name (dude.tif, ...)
}

//==================================================================
void Options::cmdPixelSamples( int samplesX, int samplesY )
{
	// minimum value is 1 as per RISpec.. but we also set a
	// maximum value.. just in case !
	mPixSamples[0] = D::Clamp( samplesX, 1, 64 );
	mPixSamples[1] = D::Clamp( samplesY, 1, 64 );
}

//==================================================================
void Options::Finalize(
					bool fallbackFileDisp,
					bool fallbackFbuffDisp )
{
	size_t	viableDisplaysN = 0;

	if NOT( mpDisplays.size() )
	{
		if ( fallbackFileDisp )
		{
			ParamList	empyList;
			cmdDisplay( "frame0001.tif", RI_FILE, "rgba", empyList );
		}

		if ( fallbackFbuffDisp )
		{
			ParamList	empyList;
			cmdDisplay( "frame0001.tif", RI_FRAMEBUFFER, "rgba", empyList );
		}
	}

	// TODO: also check Display() for mPixelAspectRatio ?

	if ( mFrameAspectRatio == NOTSET_FLOAT )
		mFrameAspectRatio = mPixelAspectRatio * mXRes / mYRes;

	if ( mLeft == NOTSET_FLOAT )
	{
		// as per RenderMan Companion page 165
		if ( mFrameAspectRatio >= 1.0f )	
		{
			mLeft	= -mFrameAspectRatio;
			mRight	=  mFrameAspectRatio;
			mBottom = -1.0f;
			mTop	=  1.0f;
		}
		else
		{
			mLeft	= -1.0f;
			mRight	=  1.0f;
			mBottom = -1.0f / mFrameAspectRatio;
			mTop	=  1.0f / mFrameAspectRatio;
		}
	}

	if NOT( mProjectionParams.size() )
		return;

	if ( mProjectionParams[0].u.stringVal == RI_PERSPECTIVE )
	{
		mpyProjection	= mpGlobalSyms->FindSymbol( RI_PERSPECTIVE );
		
		for (size_t i=1; i < mProjectionParams.size(); ++i)
		{
			if ( mProjectionParams[i].type == Param::STR )
			{
				if ( mProjectionParams[i].u.stringVal == RI_FOV )
				{
					if ( (i+1) >= mProjectionParams.size() )
					{
						printf( "Error missing parameter !\n" );
						mpRevision->BumpRevision();
						return;
					}

					float fov = mProjectionParams[++i].Flt();

					mMtxCamProj =
						Matrix44::Perspective( DEG2RAD( fov ),
											  mFrameAspectRatio,
											  mNearClip, mFarClip );
				}
			}
		}
	}
	else
	if ( mProjectionParams[0].u.stringVal == RI_ORTHOGRAPHIC )
	{
		mpyProjection	= mpGlobalSyms->FindSymbol( RI_ORTHOGRAPHIC );
	}
}

//==================================================================
void Options::FreeDisplays()
{
	for (size_t i=0; i < mpDisplays.size(); ++i)
		DSAFE_DELETE( mpDisplays[i] );

	mpDisplays.clear();
}

//==================================================================
}