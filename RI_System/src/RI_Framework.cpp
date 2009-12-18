//==================================================================
/// RI_Framework.cpp
///
/// Created by Davide Pasca - 2009/2/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Base.h"
#include "RI_State.h"
#include "RI_Framework.h"
#include <omp.h>

//==================================================================
namespace RI
{

//==================================================================
/// Framework
//==================================================================
Framework::Framework(
						RenderOutputBase *pRenderOutput,
						RenderBucketsBase *pRenderBuckets,
						const Hider &hiderParams ) :
	mpRenderOutput(pRenderOutput),
	mpRenderBuckets(pRenderBuckets),
	mpGlobalSyms(NULL),
	mHider(hiderParams)
{
}

//==================================================================
void Framework::WorldBegin(
						const Options &opt,
						const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;
	mHider.WorldBegin( opt, mtxWorldCamera );
}

//==================================================================
void Framework::Insert(
						PrimitiveBase		*pPrim,
						const Attributes	&attr,
						const Transform		&xform )
{
	if ( mAttrsRev.Sync( *attr.mpRevision ) )
		mpUniqueAttribs.push_back( DNEW Attributes( attr ) );

	if ( mTransRev.Sync( *xform.mpRevision ) )
		mpUniqueTransform.push_back( DNEW Transform( xform ) );

	pPrim->SetStates(
				mpUniqueAttribs.back(),
				mpUniqueTransform.back()
				);

	//printf( "Prim xform rev %i\n",
	//		mpUniqueTransform.back()->mpRevision->mRTrackRevisionCount );

	mHider.Insert( pPrim );
}

//==================================================================
void Framework::RenderBucket_s( Hider &hider, HiderBucket &bucket )
{
	DVec<SimplePrimitiveBase *>	&pPrimList = bucket.GetPrimList();

	bucket.BeginRender();

	WorkGrid	workGrid( *hider.mpGlobalSyms );

	DVec<ShadedGrid>	shadedGrids;

	size_t				primsN = pPrimList.size();

	DVec<MicroPolygon>	mpolys;

	size_t	apporxMPolysUsage = bucket.GetWd() * bucket.GetHe() * 8;

	mpolys.reserve( apporxMPolysUsage );

	DVec<u_int>	pixelsSamplesCount;
	hider.HideAllocSampsBegin( pixelsSamplesCount, bucket );

	shadedGrids.resize( primsN );
	for (size_t i=0; i < primsN; ++i)
	{
		const SimplePrimitiveBase	*pPrim = (const SimplePrimitiveBase *)pPrimList[i];

		workGrid.Setup(
			pPrim->mDiceGridWd,
			pPrim->mDiceGridHe,
			pPrim->mURange,
			pPrim->mVRange,
			pPrim->mpTransform->GetMatrix(),
			hider.mMtxWorldCamera );

		pPrim->Dice( workGrid, hider.mParams.mDbgColorCodedGrids );

		// should check backface and trim
		workGrid.Displace( *pPrim->mpAttribs );

		workGrid.Shade( *pPrim->mpAttribs );

		shadedGrids[ i ].Init( workGrid.mPointsN );

		hider.Bust(
				mpolys,
				bucket,
				shadedGrids[ i ],
				workGrid,
				pixelsSamplesCount,
				hider.mFinalBuff.mWd,
				hider.mFinalBuff.mHe );
	}

	DVec<HiderPixel>		pixels;
	DVec<HiderSampleData>	sampData;
	hider.HideAllocSampsEnd( pixels, sampData, pixelsSamplesCount );

	DVec<u_int>		pixelsSampsIdxs;
	hider.HideAddSamplesSetup( pixelsSampsIdxs, bucket );

	//for (size_t i=0; i < shadedGrids.size(); ++i)
	//	hider.HideAddSamples( pixels, pixelsSampsIdxs, bucket, shadedGrids[i] );

	hider.Hide( pixels, bucket );

	bucket.EndRender( hider.mFinalBuff );
}

//==================================================================
void Framework::worldEnd_simplify()
{
	DUT::QuickProf	prof( __FUNCTION__ );

	// --- convert complex primitives into simple ones
	for (size_t i=0; i < mHider.mpPrims.size(); ++i)
	{
		PrimitiveBase	*pPrim = mHider.mpPrims[i];

		if ( pPrim->IsComplex() )
		{
			((ComplexPrimitiveBase *)pPrim)->Simplify( mHider );
			pPrim->Release();
			mHider.mpPrims[i] = NULL;
			// could compact mHider.mpPrims as it goes..
		}
	}
}

//==================================================================
void Framework::worldEnd_splitAndAddToBuckets()
{
	DUT::QuickProf	prof( __FUNCTION__ );

	// --- split primitives and assign to buckets for dicing
	for (size_t i=0; i < mHider.mpPrims.size(); ++i)
	{
		if NOT( mHider.mpPrims[i] )
			continue;

		SimplePrimitiveBase	*pPrim = (SimplePrimitiveBase *)mHider.mpPrims[i];

		bool	uSplit;
		bool	vSplit;
		SimplePrimitiveBase::DosRes	dosRes =
				pPrim->SetupForDiceOrSplit( mHider, uSplit, vSplit );

		if ( dosRes == SimplePrimitiveBase::DOSRES_DICE )
		{
			mHider.InsertForDicing( (SimplePrimitiveBase *)pPrim );
		}
		else
		if ( dosRes == SimplePrimitiveBase::DOSRES_SPLIT )
		{
			pPrim->Split( mHider, uSplit, vSplit );
		}
		// otherwise it's cull..

		pPrim->Release();
		mHider.mpPrims[i] = NULL;
	}
	mHider.mpPrims.clear();
}

//==================================================================
/// RenderBucketsStd
//==================================================================
class RenderBucketsStd : public RenderBucketsBase
{
public:
	void Render( Hider &hider )
	{
		DUT::QuickProf	prof( __FUNCTION__ );

		const DVec<HiderBucket *>	buckets = hider.GetBuckets();
	
		int	bucketsN = (int)buckets.size();

		// --- dice primitives accumulated in the buckets
		#pragma omp parallel for
		for (int bi=0; bi < bucketsN; ++bi)
		{
			Framework::RenderBucket_s( hider, *buckets[ bi ] );
		}
	}
};

//==================================================================
void Framework::WorldEnd()
{
#if 0
	Point3	camPosWS =
		MultiplyV3M(
			-mHider.mMtxWorldCamera.GetTranslation(),
				mHider.mMtxWorldCamera.GetAs33().GetInverse()
		);
#else

	Point3	camPosWS = mHider.mMtxWorldCamera.GetTranslation();

#endif

	worldEnd_simplify();

	worldEnd_splitAndAddToBuckets();

	mpRenderOutput->SetSize( mOptions.mXRes, mOptions.mYRes );


	// render the buckets..
	if ( mpRenderBuckets )
	{
		mpRenderBuckets->Render( mHider );
	}
	else
	{
		RenderBucketsStd	rendBuck;

		rendBuck.Render( mHider );
	}

	// update the regions
	for (size_t bi=0; bi < mHider.mpBuckets.size(); ++bi)
	{
		u_int	x1 = mHider.mpBuckets[bi]->mX1;
		u_int	y1 = mHider.mpBuckets[bi]->mY1;

		mpRenderOutput->UpdateRegion(
					x1,
					y1,
					mHider.mpBuckets[bi]->mX2 - x1,
					mHider.mpBuckets[bi]->mY2 - y1,
					mHider.GetOutputData( x1, y1 ),
					mHider.GetOutputDataStride()
					);
	}

	// --- release the primitives in all the buckets
	for (size_t bi=0; bi < mHider.mpBuckets.size(); ++bi)
	{
		DVec<SimplePrimitiveBase *>	&pPrimList = mHider.mpBuckets[ bi ]->GetPrimList();
		for (size_t i=0; i < pPrimList.size(); ++i)
		{
			if ( pPrimList[i] )
			{
				pPrimList[i]->Release();
				pPrimList[i] = NULL;
			}
		}
	}

	for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	DDELETE( mpUniqueAttribs[i] );
	for (size_t i=0; i < mpUniqueTransform.size(); ++i)	DDELETE( mpUniqueTransform[i] );
	mpUniqueAttribs.clear();
	mpUniqueTransform.clear();

	mHider.WorldEnd();

	DASSERT( mHider.GetOutputDataWd() == (u_int)mOptions.mXRes );
	DASSERT( mHider.GetOutputDataHe() == (u_int)mOptions.mYRes );
}



//==================================================================
}
