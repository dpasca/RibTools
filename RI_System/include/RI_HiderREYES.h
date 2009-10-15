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
	int							mX1;
	int							mY1;
	int							mX2;
	int							mY2;
	Buffer2D<NCOLS>				mCBuff;
	Buffer2D<1>					mZBuff;
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

	void BeginRender()
	{
		u_int wd = (u_int)(mX2 - mX1);
		u_int he = (u_int)(mY2 - mY1);

		mCBuff.Setup( wd, he );
		mCBuff.Clear();

		mZBuff.Setup( wd, he );
		mZBuff.Fill( FLT_MAX );
	}

	void EndRender( Buffer2D<NCOLS> &outCBuff )
	{
		mZBuff.Free();

		outCBuff.CopyRect( mX1, mY1, mCBuff );
		mCBuff.Free();
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

	Options					mOptions;
	Buffer2D<NCOLS>			mFinalBuff;
	DVec<Bucket *>			mpBuckets;
	Params					mParams;
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

	void Hide(
			MicroPolygonGrid	&g,
			float				destOffX,
			float				destOffY,
			u_int				destWd,
			u_int				destHe,
			Buffer2D<NCOLS>		&destBuff,
			Buffer2D<1>			&zBuff ) const;

	u_int		GetOutputDataStride() const	{ return mFinalBuff.GetWd() * NCOLS;	}
	u_int		GetOutputDataWd() const		{ return mFinalBuff.GetWd();			}
	u_int		GetOutputDataHe() const		{ return mFinalBuff.GetHe();			}

	const float *GetOutputData( u_int x1, u_int y1 ) const
	{
		return mFinalBuff.GetSamplePtrR( (int)x1, (int)y1 );
	}

	float *GetOutputDataRW( u_int x1, u_int y1 )
	{
		return mFinalBuff.GetSamplePtrRW( (int)x1, (int)y1 );
	}

	const DVec<Bucket *>	&GetBuckets() {	return mpBuckets; }

	size_t	GetOutputBucketMemSize( size_t buckIdx ) const;
	void	CopyOutputBucket( size_t buckIdx, float *pDest, size_t destMaxSize ) const;

	void	StoreOutputBucket( size_t buckIdx, const float *pSrc, size_t srcSize );

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