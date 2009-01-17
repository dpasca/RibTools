/*
 *  RI_Options.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_OPTIONS_H
#define RI_OPTIONS_H

#include "RI_Base.h"
#include "RI_Param.h"
#include "DMath.h"
#include "RI_Symbol.h"

//==================================================================
namespace RI
{

//==================================================================
/// Options
//==================================================================
class Options : public RevisionTracker
{
	SymbolList	*mpStatics;

public:
	Options();
	void Init( SymbolList *pTManager );

	// avoid initialization of default values and just copy..
	Options( const Options &options ) {	*this = options; }

	~Options();

	//Options &operator=(const Options &options);
public:
	// Camera
	int		mXRes, mYRes;
	float	mPixelAspectRatio;
	float	mFrameAspectRatio;
	float	mLeft, mRight, mBottom, mTop;
	float	mXMin, mXMax, mYMin, mYMax;
	
	CPSymVoid	mpyProjection;
	Matrix44	mMtxViewHomo;
	
	float	mNearClip, mFarClip;
	float	mFStop, mFocalLength, mFocalDistance;
	float	mOpenShutter, mCloseShutter;
	
	// Display
	// ...
public:
	// Functions
	void cmdFormat( int xRes, int yRes, float pixelRatio );
	void cmdFrameAspectRatio( float ratio );
	void cmdScreenWindow( float left, float right, float bottom, float top );
	void cmdCropWindow( float xMin, float xMax, float yMin, float yMax );
	void cmdProjection( ParamList &params );
	void cmdClipping( float near, float farr );
	void cmdDepthOfField( float fStop, float focalLength, float focalDistance );
	void cmdShutter( float openShutter, float closeShutter );

};
	
//==================================================================
}

#endif