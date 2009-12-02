//==================================================================
/// RI_HiderSampleCoordsBuffer.h
///
/// Created by Davide Pasca - 2009/12/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_HIDERSAMPLECOORDSBUFFER_H
#define RI_HIDERSAMPLECOORDSBUFFER_H

//==================================================================
namespace RI
{

//==================================================================
/// HiderSampleData
//==================================================================
class HiderSampleData
{
public:
	float			mDepth;
	float			*mpData;
	u_int			mDataSize;

	HiderSampleData	*mpNext;
};

//==================================================================
/// HiderSampleCoords
//==================================================================
class HiderSampleCoords
{
public:
	float			mX, mY;
	float			mTime;
	float			mLensX;
	float			mLensY;
};

//==================================================================
/// HiderPixel
//==================================================================
class HiderPixel
{
public:
	int						mX, mY;
	const HiderSampleCoords	*mpSampCoords;
	HiderSampleData			*mpSampData;
};

//==================================================================
/// HiderSampleCoordsBuffer
//==================================================================
class HiderSampleCoordsBuffer
{
public:
	u_int				mWd;
	u_int				mHe;
	HiderSampleCoords	*mpSampCoords;

	HiderSampleCoordsBuffer();

	~HiderSampleCoordsBuffer();

	void Setup( u_int wd, u_int he, u_int subPixelDimLog2 );

private:
	void setupPixel( HiderSampleCoords *pSampCoods, u_int subPixelDimLog2 );
};

//==================================================================
}

#endif
