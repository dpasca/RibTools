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
	float			mCi[3];
	float			mOi[3];
};

//==================================================================
/// HiderSampleCoords
//==================================================================
class HiderSampleCoords
{
public:
	float		mX, mY;
	float		mTime;
	float		mLensX;
	float		mLensY;
};

//==================================================================
class HiderBaseSampleCoords
{
public:
	float			mTime;
};

//==================================================================
/// HiderPixel
//==================================================================
class HiderPixel
{
public:
	int						mX, mY;
	const HiderSampleCoords	*mpSampCoords;	// one per sub-sample
	DVec<HiderSampleData>	*mpSampDataLists;	// one per sub-sample
};

//==================================================================
/// HiderSampleCoordsBuffer
//==================================================================
class HiderSampleCoordsBuffer
{
public:
	u_int					mWd;
	u_int					mHe;
	u_int					mSubPixelDimLog2;
	HiderSampleCoords		*mpSampCoords;
	HiderBaseSampleCoords	*mpBaseSampCoords;

	HiderSampleCoordsBuffer();

	~HiderSampleCoordsBuffer();

	void Init( u_int wd, u_int he, u_int subPixelDimLog2 );

	void Setup( float openTime, float closeTime );

	bool IsInitialized() const		{ return mpSampCoords != NULL; }

	u_int GetWd() const				{ return mWd; }
	u_int GetHe() const				{ return mHe; }

	u_int GetSampsPerDim() const	{ return 1 << mSubPixelDimLog2;			}
	u_int GetSampsPerPixel() const	{ return 1 << (mSubPixelDimLog2 << 1);	}

private:
	void initPixel(
				HiderSampleCoords		*pSampCoods,
				HiderBaseSampleCoords	*pBaseSampleCoords,
				u_int					subPixelDimLog2,
				DRandom					&randGen );

	void setupPixel(
				HiderSampleCoords		*pSampCoods,
				HiderBaseSampleCoords	*pBaseSampleCoords,
				float					openTime,
				float					dtime );
};

//==================================================================
}

#endif
