//==================================================================
/// RI_HiderSampleCoordsBuffer.cpp
///
/// Created by Davide Pasca - 2009/12/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_HiderSampleCoordsBuffer.h"

//==================================================================
namespace RI
{

//==================================================================
HiderSampleCoordsBuffer::HiderSampleCoordsBuffer() :
	mWd(0),
	mHe(0),
	mpSampCoords(NULL)
{
}

//==================================================================
HiderSampleCoordsBuffer::~HiderSampleCoordsBuffer()
{
	DSAFE_DELETE_ARRAY( mpSampCoords );
}

//==================================================================
void HiderSampleCoordsBuffer::Setup( u_int wd, u_int he, u_int subPixelDimLog2 )
{
	mWd = wd;
	mHe = he;

	u_int	sampsPerPixel = 1 << (subPixelDimLog2*2);

	mpSampCoords = DNEW HiderSampleCoords [ (size_t)mWd * (size_t)mHe * sampsPerPixel ];

	size_t sampsCnt = 0;
	for (u_int y=0; y < mHe; ++y)
	{
		for (u_int x=0; x < mWd; ++x, sampsCnt += sampsPerPixel)
		{
			setupPixel( mpSampCoords + sampsCnt, subPixelDimLog2 );
		}
	}
}

//==================================================================
static int randomN( int n )
{
	return rand() % n;
}

//==================================================================
void HiderSampleCoordsBuffer::setupPixel( HiderSampleCoords *pSampCoods, u_int subPixelDimLog2 )
{
	u_int subPixelDim = 1 << subPixelDimLog2;
	u_int randPosMask = (1 << subPixelDimLog2) - 1;

	// fill
	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			HiderSampleCoords *pCoords = &pSampCoods[ (y << subPixelDimLog2) + x ];
			pCoords->mX = (float)y;
			pCoords->mY = (float)x;
		}
	}

	// shuffle
	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			int	k = randomN( subPixelDim-1-y );

			float t = pSampCoods[ (y << subPixelDimLog2) + x ].mY;
			pSampCoods[ (y << subPixelDimLog2) + x ].mY = pSampCoods[ (k << subPixelDimLog2) + x ].mY;
			pSampCoods[ (k << subPixelDimLog2) + x ].mY = t;
		}
	}
}

//==================================================================
}
