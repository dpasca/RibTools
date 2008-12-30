/*
 *  RI_Options.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "DTypes.h"
#include "RI_Base.h"
#include "RI_Options.h"

//==================================================================
namespace RI
{

//==================================================================
Options::Options() :
	mXRes(640), mYRes(480),
	mPixelAspectRatio(1.0f),
	mFrameAspectRatio(4.0f/3),	
	mLeft(-4.0f/3), mRight(4.0f/3), mBottom(-1), mTop(1),
	mXMin(0), mXMax(1), mYMin(0), mYMax(1),
	mProjection("orthographic"),
	mNearClip(RI_EPSILON), mFarClip(RI_INFINITY),
	mFStop(RI_INFINITY),
	mFocalLength(0),
	mFocalDistance(0),
	mOpenShutter(0),
	mCloseShutter(0)
{
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
}

//==================================================================
void Options::cmdFrameAspectRatio( float ratio )
{
	mFrameAspectRatio	= ratio;
}

//==================================================================
void Options::cmdScreenWindow( float left, float right, float bottom, float top )
{
	mLeft	= left;
	mRight	= right;
	mBottom	= bottom;
	mTop	= top;
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
}

//==================================================================
const char *RI_TOK_PERSPECTIVE	= "perspective";
const char *RI_TOK_ORTHOGONAL	= "orthogonal";

//==================================================================
void Options::cmdProjection( ParamList &params )
{
	if ( params.size() >= 1 )
	{
		if ( params[0].u.stringVal == RI_TOK_PERSPECTIVE )
		{
			mProjection	= RI_TOK_PERSPECTIVE;
		}
		else
		if ( params[0].u.stringVal == RI_TOK_ORTHOGONAL )
		{
			mProjection	= RI_TOK_ORTHOGONAL;
		}
	}
}

//==================================================================
void Options::cmdClipping( float near, float farr )
{
	DASSERT( near < farr );

	mNearClip	= near;
	mFarClip	= farr;
}

//==================================================================
void Options::cmdDepthOfField( float fStop, float focalLength, float focalDistance )
{
	mFStop			= fStop;
	mFocalLength	= focalLength;
	mFocalDistance	= focalDistance;
}

//==================================================================
void Options::cmdShutter( float openShutter, float closeShutter )
{
	mOpenShutter	= openShutter;
	mCloseShutter	= closeShutter;
}

//==================================================================
}