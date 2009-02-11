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
HiderREYES::HiderREYES()
{
}

//==================================================================
HiderREYES::~HiderREYES()
{
}

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

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxCameraProj	= opt.mMtxViewHomo;
	
	mDestBuff.Setup( opt.mXRes, opt.mYRes );
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
	float du = 1.0f / g.mXDim;
	float dv = 1.0f / g.mYDim;
	
	float destHalfWd	= (float)mDestBuff.mWd / 2.0f;
	float destHalfHe	= (float)mDestBuff.mHe / 2.0f;
	float destWd		= (float)mDestBuff.mWd;
	float destHe		= (float)mDestBuff.mHe;

	const Point3	*pRuns = g.mpPoints;

	for (u_int iv=0; iv < g.mYDim; ++iv)
	{
		float	v = iv * dv;
		
		for (u_int iu=0; iu < g.mXDim; ++iu)
		{
			float	u = iu * du;
			
			Vector4	Pproj = *pRuns++ * mMtxCameraProj;
			
			float	oow = 1.0f / Pproj.w;
			
			int	winX = (int)(destHalfWd + destWd * Pproj.x * oow);
			int	winY = (int)(destHalfHe - destHe * Pproj.y * oow);

			float	destCol[3] =
			{
				u,
				v,
				0
			};

			mDestBuff.SetSample( winX, winY, destCol );
		}
	}

}

//==================================================================
}
