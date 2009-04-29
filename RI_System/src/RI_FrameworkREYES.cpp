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
		mpUniqueAttribs.push_back( new Attributes( attr ) );

	if ( mTransRev.Sync( *xform.mpRevision ) )
		mpUniqueTransform.push_back( new Transform( xform ) );

	pPrim->SetStates(
				mpUniqueAttribs.back(),
				mpUniqueTransform.back()
				);

	//printf( "Prim xform rev %i\n",
	//		mpUniqueTransform.back()->mpRevision->mRTrackRevisionCount );

	mHider.Insert( pPrim );
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

	// --- split primitives until necessary
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

	// --- dice primitives accumulated in the buckets
	for (size_t bi=0; bi < mHider.mpBuckets.size(); ++bi)
	{
		DVec<SimplePrimitiveBase *>	&pPrimList = mHider.mpBuckets[bi]->GetPrimList();

		for (size_t i=0; i < pPrimList.size(); ++i)
		{
			SimplePrimitiveBase	*pPrim = (SimplePrimitiveBase *)pPrimList[i];

			MicroPolygonGrid	grid;

			grid.Setup(
				pPrim->mDiceGridWd,
				pPrim->mDiceGridHe,
				pPrim->mURange,
				pPrim->mVRange,
				pPrim->mpTransform->GetMatrix() );

			pPrim->Dice( grid, mHider.mMtxWorldCamera, mHider.mParams.mDbgColorCodedGrids );

			// should check backface and trim
			// grid.displace();
			grid.Shade( *pPrim->mpAttribs );

			mHider.Hide( grid );

			pPrim->Release();
			pPrimList[i] = NULL;
		}
	}

	for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	delete mpUniqueAttribs[i];
	for (size_t i=0; i < mpUniqueTransform.size(); ++i)	delete mpUniqueTransform[i];
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
