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
#include "RI_Buffer2D.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class PrimitiveBase;

//==================================================================
/// Bucket
//==================================================================
class Bucket
{
public:
	int	mX1;
	int	mY1;
	int	mX2;
	int	mY2;

	DVec<SimplePrimitiveBase *>	mpPrims;

public:
	Bucket( int x1, int y1, int x2, int y2 ) :
		mX1(x1),
		mY1(y1),
		mX2(x2),
		mY2(y2)
	{
	}

	~Bucket()
	{
		for (size_t i=0; i < mpPrims.size(); ++i)
			if ( mpPrims[i] )
				mpPrims[i]->Release();
	}

	bool Contains( int x, int y ) const
	{
		return x >= mX1 && y >= mY1 && x < mX2 && y < mY2;
	}

	bool Intersects( int minX, int minY, int maxX, int maxY ) const
	{
		return
			minX < mX2 && maxX >= mX1 &&	// >= just to be sure.. ummm
			minY < mY2 && maxY >= mY1;
	}

	DVec<SimplePrimitiveBase *>	&GetPrimList()	{ return mpPrims;	}
};

//==================================================================
/// HiderREYES
//==================================================================
class HiderREYES
{
public:
	SymbolList		*mpStatics;
private:
	Matrix44		mMtxWorldCamera;
	Matrix44		mMtxWorldProj;
	float			mHalfXRes;
	float			mHalfYRes;

public:
	//==================================================================
	struct Params
	{
		int		mDbgOnlyBucketAtX;
		int		mDbgOnlyBucketAtY;
		bool	mDbgShowBuckets;
		bool	mDbgColorCodedGrids;

		Params() :
			mDbgOnlyBucketAtX(-1),
			mDbgOnlyBucketAtY(-1),
			mDbgShowBuckets(false),
			mDbgColorCodedGrids(false)
		{
		}
	};

private:
	friend class FrameworkREYES;

	Options				mOptions;
	Buffer2D<3>			mDestBuff;
	Buffer2D<1>			mZBuff;
	DVec<Bucket *>		mpBuckets;
	Params				mParams;
	DVec<PrimitiveBase *>	mpPrims;

public:
	HiderREYES( const Params &params );
	~HiderREYES();

	void WorldBegin(
			const Options &opt,
			const Matrix44 &mtxWorldCamera );

	void Insert( PrimitiveBase *pPrim );

	void InsertSimple(	
					SimplePrimitiveBase		*pSimplePrim,
					ComplexPrimitiveBase	&srcPrim
					);

	void InsertSplitted(	
					SimplePrimitiveBase		*pDesPrim,
					SimplePrimitiveBase		&srcPrim
					);

	void InsertForDicing( SimplePrimitiveBase *pPrim );

	void WorldEnd();
	
	float RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const;

	void Hide( MicroPolygonGrid &g );
	
	u_int		GetOutputDataWd() const	{ return mDestBuff.GetWd();	}
	u_int		GetOutputDataHe() const	{ return mDestBuff.GetHe();	}
	const float *GetOutputData() const { return mDestBuff.GetData(); }

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