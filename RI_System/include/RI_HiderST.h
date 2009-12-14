//==================================================================
/// RI_HiderST.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_HIDERST_H
#define RI_HIDERST_H

#include "RI_Options.h"
#include "RI_Buffer2D.h"
#include "RI_HiderSTBucket.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class PrimitiveBase;

//==================================================================
/// Hider
//==================================================================
class Hider
{
public:
	const SymbolList	*mpGlobalSyms;

private:
	Matrix44			mMtxWorldCamera;
	Matrix44			mMtxWorldProj;
	float				mHalfXRes;
	float				mHalfYRes;

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
	friend class Framework;

	Options					mOptions;
	Buffer2D<NCOLS>			mFinalBuff;
	DVec<HiderBucket *>		mpBuckets;
	Params					mParams;
	DVec<PrimitiveBase *>	mpPrims;

public:
	Hider( const Params &params );
	~Hider();

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

	void Bust(	ShadedGrid		&shadGrid,
				const WorkGrid	&workGrid,
				float			destOffX,
				float			destOffY,
				u_int			screenWd,
				u_int			screenHe ) const;

	void HideCountBegin(
					DVec<u_int> &out_pixelsSamplesCount,
					HiderBucket &buck );

	void HideCountGrid(
					DVec<u_int>			&inout_pixelsSamplesCount,
					HiderBucket			&buck,
					const ShadedGrid	&shadGrid );

	void HideCountEnd(
					DVec<HiderPixel>		&out_pixels,
					DVec<HiderSampleData>	&out_sampData,
					HiderBucket				&buck,
					const DVec<u_int>		&pixelsSamplesCount );

	void HideAddSamplesSetup(
					DVec<u_int>			&out_pixelsSampsIdxs,
					HiderBucket			&buck );

	void HideAddSamples(
					DVec<HiderPixel>	&pixels,
					DVec<u_int>			&pixelsSampsIdxs,
					HiderBucket			&buck,
					const ShadedGrid	&shadGrid );

	void Hide(
					DVec<HiderPixel>	&pixels,
					HiderBucket			&buck );

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

	const DVec<HiderBucket *>	&GetBuckets() {	return mpBuckets; }

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