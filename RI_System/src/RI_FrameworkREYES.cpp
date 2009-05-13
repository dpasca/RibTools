/*
 *  RI_FrameworkREYES.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

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
						const HiderREYES &hiderParams ) :
	mpRenderOutput(pRenderOutput),
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
void FrameworkREYES::renderBucket_s( HiderREYES &hider, Bucket &bucket )
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
		// grid.displace();
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
/// QuickProf
//==================================================================
class QuickProf
{
	const char *mpMsg;
	I64			mStart;

public:
	QuickProf( const char *pMsg ) :
		mpMsg(pMsg)
	{
		mStart = DUT::GetTimeTicks();
	}

	~QuickProf()
	{
		I64	elapsed = DUT::GetTimeTicks() - mStart;
		printf( "%s: %4.2lf ms\n", mpMsg, DUT::TimeTicksToMS( elapsed ) );
	}
};

//==================================================================
void FrameworkREYES::worldEnd_simplify()
{
	QuickProf	prof( __FUNCTION__ );

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
	QuickProf	prof( __FUNCTION__ );

	// --- split primitives and assing to buckets for dicing
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
void FrameworkREYES::worldEnd_renderBuckets()
{
	QuickProf	prof( __FUNCTION__ );

	int	bucketsN = (int)mHider.mpBuckets.size();

	// --- dice primitives accumulated in the buckets
	#pragma omp parallel for
	for (int bi=0; bi < bucketsN; ++bi)
	{
		renderBucket_s( mHider, *mHider.mpBuckets[ bi ] );
	}
}

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

	worldEnd_renderBuckets();

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

	//glutReshapeWindow( mOptions.mXRes, mOptions.mYRes );
	mpRenderOutput->Update(
				mOptions.mXRes,
				mOptions.mYRes,
				mHider.GetOutputData()
				);
}



//==================================================================
}
