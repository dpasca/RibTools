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
FrameworkREYES::FrameworkREYES( RenderOutputBase *pRenderOutput ) :
	FrameworkBase( pRenderOutput )
{
	mpHider = &mHiderREYES;
}

//==================================================================
void FrameworkREYES::WorldBegin(
						const Options &opt,
						const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;
	mpHider->WorldBegin( opt, mtxWorldCamera );
}

//==================================================================
void FrameworkREYES::Insert(
						Primitive			*pPrim,
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

	mpHider->Insert( pPrim, attr, xform );
}

//==================================================================
void FrameworkREYES::InsertSplitted(	
						Primitive			*pSplitPrim,
						Primitive			&srcPrim
						)
{
	mpHider->InsertSplitted( pSplitPrim, srcPrim );
}

//==================================================================
void FrameworkREYES::Remove( Primitive *pPrim )
{
}

//==================================================================
void FrameworkREYES::WorldEnd()
{
#if 0
	Point3	camPosWS =
		MultiplyV3M(
			-mpHider->mMtxWorldCamera.GetTranslation(),
				mpHider->mMtxWorldCamera.GetAs33().GetInverse()
		);
#else

	Point3	camPosWS = mpHider->mMtxWorldCamera.GetTranslation();

#endif

	for (size_t bi=0; bi < mHiderREYES.mpBuckets.size(); ++bi)
	{
		DVec<Primitive *>	&pPrimList = mHiderREYES.mpBuckets[bi]->GetPrimList();

		for (size_t i=0; i < pPrimList.size(); ++i)
		{
			Primitive	*pPrim = pPrimList[i];

			MicroPolygonGrid	grid;

			bool	uSplit = false;
			bool	vSplit = false;

			if ( pPrim->IsDiceable( grid, mpHider, uSplit, vSplit ) )
			{
				pPrim->Dice( grid, camPosWS, mpHider->mMtxWorldCamera );

				// should check backface and trim
				// grid.displace();
				grid.Shade( *pPrim->mpAttribs );

				mpHider->Hide( grid );
			}
			else
			if ( pPrim->IsSplitable() )
				pPrim->Split( *this, uSplit, vSplit );
		}
	}

	for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	delete mpUniqueAttribs[i];
	for (size_t i=0; i < mpUniqueTransform.size(); ++i)	delete mpUniqueTransform[i];
	mpUniqueAttribs.clear();
	mpUniqueTransform.clear();

	mpHider->WorldEnd();

	DASSERT( mpHider->GetOutputDataWd() == (u_int)mOptions.mXRes );
	DASSERT( mpHider->GetOutputDataHe() == (u_int)mOptions.mYRes );

	//glutReshapeWindow( mOptions.mXRes, mOptions.mYRes );
	mpRenderOutput->Update(
				mOptions.mXRes,
				mOptions.mYRes,
				mpHider->GetOutputData()
				);
}



//==================================================================
}
