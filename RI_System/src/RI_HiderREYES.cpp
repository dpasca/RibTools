/*
 *  RI_HiderREYES.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include <GLUT/GLUT.h>
#include "RI_Primitive.h"
#include "RI_HiderREYES.h"

//==================================================================
namespace RI
{

//==================================================================
/// HiderREYES
//==================================================================
void HiderREYES::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	for (size_t i=0; i < mpPrims.size(); ++i)
		delete mpPrims[i];
	mpPrims.clear();

	mOptions = opt;

	glutReshapeWindow( mOptions.mXRes, mOptions.mYRes );

	mMtxWorldCamera	= mtxWorldCamera;// * opt.mMtxViewHomo;
	//mHalfXRes		= opt.mXRes * 0.5f;
	//mHalfYRes		= opt.mYRes * 0.5f;
}

//==================================================================
void HiderREYES::Insert(
			Primitive			*pPrim,
			const Attributes	&attr,
			const Transform		&xform )
{
	mpPrims.push_back( pPrim );
}

//==================================================================
void HiderREYES::InsertSplitted(	
					Primitive	*pSplitPrim,
					Primitive	&srcPrim
					)
{
	pSplitPrim->CopyStates( srcPrim );

	mpPrims.push_back( pSplitPrim );
}

//==================================================================
void HiderREYES::Remove( Primitive *pPrim )
{
}

//==================================================================
void HiderREYES::WorldEnd()
{
	for (size_t i=0; i < mpPrims.size(); ++i)
		delete mpPrims[i];

	mpPrims.clear();
}

//==================================================================
void HiderREYES::Hide( MicroPolygonGrid &g )
{
}

//==================================================================
}
