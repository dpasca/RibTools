//==================================================================
/// RI_Options.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_OPTIONS_H
#define RI_OPTIONS_H

#include "RI_Base.h"
#include "RI_Param.h"
#include "DMath/include/DMath.h"
#include "RI_Symbol.h"

//==================================================================
namespace RI
{

//==================================================================
/// Options
//==================================================================
class Options
{
	const SymbolList	*mpGlobalSyms;

	class Display
	{
	public:
		u_int	mPixSamples[2];

		void Init()
		{
			mPixSamples[0] = 2;
			mPixSamples[1] = 2;
		}
	};

public:
	Options();
	void Init( const SymbolList *pGlobalSyms, RevisionTracker *pRevision );

	// avoid initialization of default values and just copy..
	//Options( const Options &options ) {	*this = options; }

	~Options();

	//Options &operator=(const Options &options);
public:
	// Camera
	int		mXRes, mYRes;
	float	mPixelAspectRatio;
	float	mFrameAspectRatio;
	float	mLeft, mRight, mBottom, mTop;
	float	mXMin, mXMax, mYMin, mYMax;
	
	const Symbol	*mpyProjection;
	ParamList		mProjectionParams;
	Matrix44		mMtxCamProj;
	
	float	mNearClip, mFarClip;
	float	mFStop, mFocalLength, mFocalDistance;
	float	mOpenShutter, mCloseShutter;
	
	RevisionTracker	*mpRevision;
	
	// Display
	Display	mDisp;

	enum
	{
		SEARCHPATH_SHADER,
		SEARCHPATH_TEXTURE,
		SEARCHPATH_N,
	};

	DVec<DStr>	mSearchPaths[SEARCHPATH_N];

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

	// Display
	void cmdPixelSamples( int samplesX, int samplesY );

	void Finalize();

};
	
//==================================================================
}

#endif