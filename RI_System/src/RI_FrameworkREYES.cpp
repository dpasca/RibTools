//==================================================================
/// RI_FrameworkREYES.cpp
///
/// Created by Davide Pasca - 2009/2/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Base.h"
#include "RI_State.h"
#include "RI_FrameworkREYES.h"
#include <omp.h>

//==================================================================
namespace RI
{

//==================================================================
/// FrameworkREYES
//==================================================================
FrameworkREYES::FrameworkREYES(
						RenderOutputBase *pRenderOutput,
						RenderBucketsBase *pRenderBuckets,
						const HiderREYES &hiderParams ) :
	mpRenderOutput(pRenderOutput),
	mpRenderBuckets(pRenderBuckets),
	mpStatics(NULL),
	mHider(hiderParams)
{
}

//==================================================================
void FrameworkREYES::WorldBegin(
						const Options &opt,
						const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;
	mHider.WorldBegin( opt, mtxWorldCamera );
}

//==================================================================
void FrameworkREYES::Insert(
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
void FrameworkREYES::RenderBucket_s( HiderREYES &hider, Bucket &bucket )
{
	DVec<SimplePrimitiveBase *>	&pPrimList = bucket.GetPrimList();

	bucket.BeginRender();

	MicroPolygonGrid	grid;

	for (size_t i=0; i < pPrimList.size(); ++i)
	{
		const SimplePrimitiveBase	*pPrim = (const SimplePrimitiveBase *)pPrimList[i];

		grid.Setup(
			pPrim->mDiceGridWd,
			pPrim->mDiceGridHe,
			pPrim->mURange,
			pPrim->mVRange,
			pPrim->mpTransform->GetMatrix() );

		pPrim->Dice( grid, hider.mMtxWorldCamera, hider.mParams.mDbgColorCodedGrids );

		// should check backface and trim
		grid.Displace( *pPrim->mpAttribs );

		grid.Shade( *pPrim->mpAttribs );

		hider.Hide(
				grid,
				(float)-bucket.mX1,
				(float)-bucket.mY1,
				hider.mFinalBuff.mWd,
				hider.mFinalBuff.mHe,
				bucket.mCBuff,
				bucket.mZBuff );

		// not thread safe to release here...
		// pPrim->Release();
		// pPrimList[i] = NULL;
	}

	bucket.EndRender( hider.mFinalBuff );
}

//==================================================================
void FrameworkREYES::worldEnd_simplify()
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
void FrameworkREYES::worldEnd_splitAndAddToBuckets()
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
		bool	willDice = pPrim->SetupForDiceOrSplit( mHider, uSplit, vSplit );

		if ( willDice )
		{
			mHider.InsertForDicing( (SimplePrimitiveBase *)pPrim->Borrow() );
		}
		else
		{
			pPrim->Split( mHider, uSplit, vSplit );
		}

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
	void Render( HiderREYES &hider )
	{
		DUT::QuickProf	prof( __FUNCTION__ );

		const DVec<Bucket *>	buckets = hider.GetBuckets();
	
		int	bucketsN = (int)buckets.size();

		// --- dice primitives accumulated in the buckets
		#pragma omp parallel for
		for (int bi=0; bi < bucketsN; ++bi)
		{
			FrameworkREYES::RenderBucket_s( hider, *buckets[ bi ] );
		}
	}
};

//==================================================================
void FrameworkREYES::WorldEnd()
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
