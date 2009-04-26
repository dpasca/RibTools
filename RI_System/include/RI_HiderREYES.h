//==================================================================
/// RI_HiderREYES.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_HIDERREYES_H
#define RI_HIDERREYES_H

#include "RI_Options.h"
#include "RI_HiderBase.h"
#include "RI_Buffer2D.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class Primitive;

//==================================================================
/// HiderREYES
//==================================================================
class HiderREYES : public HiderBase
{
	friend class FrameworkREYES;

	Options			mOptions;
	Buffer2D<3>		mDestBuff;
	Buffer2D<1>		mZBuff;
	DVec<Bucket *>	mpBuckets;

	struct Debug
	{
		int		mOnlyBucketAtX;
		int		mOnlyBucketAtY;
		bool	mShowBuckets;

		Debug() :
			mOnlyBucketAtX(-1),
			mOnlyBucketAtY(-1),
			mShowBuckets(false)
		{
		}
	} mDbg;

public:
	HiderREYES();
	~HiderREYES();

	void WorldBegin(
			const Options &opt,
			const Matrix44 &mtxWorldCamera );

	void Insert( Primitive *pPrim );

	void InsertSplitted(	
						Primitive	*pSplitPrim,
						Primitive	&srcPrim
						);

	void Remove( Primitive *pPrim );

	void WorldEnd();
	
	float RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const;

	void Hide( MicroPolygonGrid &g );
	
	u_int		GetOutputDataWd() const	{ return mDestBuff.GetWd();	}
	u_int		GetOutputDataHe() const	{ return mDestBuff.GetHe();	}
	const float *GetOutputData() const { return mDestBuff.GetData(); }

	void DbgDrawOnlyBucketAt( int x, int y )
	{
		mDbg.mOnlyBucketAtX = x;
		mDbg.mOnlyBucketAtY = y;
	}

	void DbgShowBuckets( bool onoff )
	{
		mDbg.mShowBuckets = onoff;
	}

private:
	bool	makeRasterBound(
						const Bound &b,
						const Matrix44 &mtxLocalWorld,
						float out_bound2d[4] ) const;

	//void pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n );
};

//==================================================================
}

#endif