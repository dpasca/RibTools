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
	mpSampCoords(NULL),
	mpBaseSampCoords(NULL)
{
}

//==================================================================
HiderSampleCoordsBuffer::~HiderSampleCoordsBuffer()
{
	DSAFE_DELETE_ARRAY( mpSampCoords );
	DSAFE_DELETE_ARRAY( mpBaseSampCoords );
}

//==================================================================
void HiderSampleCoordsBuffer::Init( u_int wd, u_int he, u_int subPixelDimLog2 )
{
	mWd = wd;
	mHe = he;
	mSubPixelDimLog2 = subPixelDimLog2;

	u_int	sampsPerPixel = 1 << (subPixelDimLog2*2);

	size_t	sampArrSize = (size_t)mWd * (size_t)mHe * sampsPerPixel;

	mpSampCoords = DNEW HiderSampleCoords [ sampArrSize ];
	mpBaseSampCoords = DNEW HiderBaseSampleCoords [ sampArrSize ];

	DRandom	randGen( 0x11112222 );

	size_t sampsCnt = 0;
	for (u_int y=0; y < mHe; ++y)
	{
		for (u_int x=0; x < mWd; ++x, sampsCnt += sampsPerPixel)
		{
			initPixel(
				mpSampCoords + sampsCnt,
				mpBaseSampCoords + sampsCnt,
				subPixelDimLog2,
				randGen );
		}
	}
}

//==================================================================
void HiderSampleCoordsBuffer::Setup( float openTime, float closeTime )
{
	u_int	sampsPerPixel = 1 << (mSubPixelDimLog2*2);

	float dtime = closeTime - openTime;

	size_t sampsCnt = 0;
	for (u_int y=0; y < mHe; ++y)
	{
		for (u_int x=0; x < mWd; ++x, sampsCnt += sampsPerPixel)
		{
			setupPixel(
				mpSampCoords + sampsCnt,
				mpBaseSampCoords + sampsCnt,
				openTime,
				dtime );
		}
	}
}

//==================================================================
void HiderSampleCoordsBuffer::initPixel(
							HiderSampleCoords		*pSampCoods,
							HiderBaseSampleCoords	*pBaseSampleCoords,
							u_int					subPixelDimLog2,
							DRandom					&randGen )
{
	u_int subPixelDim = 1 << subPixelDimLog2;
	u_int randPosMask = (1 << subPixelDimLog2) - 1;

	// fill
	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			HiderSampleCoords *pCoords = &pSampCoods[ (y << subPixelDimLog2) + x ];
			pCoords->mX = (int)y;
			pCoords->mY = (int)x;
		}
	}

	// shuffle
	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			int	k = randGen.Next32Range( 0, subPixelDim-1-y );

			u_int idx = (y << subPixelDimLog2) + x;
			u_int idxK = (k << subPixelDimLog2) + x;

			float t = pSampCoods[ idx ].mY;
			pSampCoods[ idx ].mY = pSampCoods[ idxK ].mY;
			pSampCoods[ idxK ].mY = t;
		}
	}

	// time
	float timeRun = 0;
	float dtime = 1.0f / (subPixelDim * subPixelDim);
	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			u_int idx = (y << subPixelDimLog2) + x;

			float t = timeRun + randGen.NextF0_1() * dtime;
			
			pSampCoods[ idx ].mTime			= 0;
			pBaseSampleCoords[ idx ].mTime	= t;

			timeRun += dtime;
		}
	}

	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			u_int idx = (y << subPixelDimLog2) + x;

			pSampCoods[ idx ].mLensX	= 0;
			pSampCoods[ idx ].mLensY	= 0;
		}
	}
}

//==================================================================
void HiderSampleCoordsBuffer::setupPixel(
			HiderSampleCoords		*pSampCoods,
			HiderBaseSampleCoords	*pBaseSampleCoords,
			float					openTime,
			float					dtime )
{
	u_int subPixelDim = 1 << mSubPixelDimLog2;

	for (u_int y=0; y < subPixelDim; ++y)
	{
		for (u_int x=0; x < subPixelDim; ++x)
		{
			u_int idx = (y << mSubPixelDimLog2) + x;

			const HiderBaseSampleCoords	&baseCoords	= pBaseSampleCoords[ idx];
			HiderSampleCoords			&coords		= pSampCoods[ idx ];

			coords.mTime = openTime + dtime * baseCoords.mTime;
		}
	}
}

//==================================================================
}
