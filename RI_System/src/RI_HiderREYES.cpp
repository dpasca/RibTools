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

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxCameraProj	= opt.mMtxViewHomo;
	
	mDestBuff.Setup( opt.mXRes, opt.mYRes );
	mDestBuff.Clear();
	
	mZBuff.Setup( opt.mXRes, opt.mYRes );
	mZBuff.Fill( FLT_MAX );
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
	// $$$ mark splitted stuff to never be used again !
	DASSERT( srcPrim.IsUsable() );
	
	pSplitPrim->CopyStates( srcPrim );
	
	pSplitPrim->mSplitCnt += 1;

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
float HiderREYES::RasterEstimate( const Bound &b, Matrix44 &mtxLocalCamera ) const
{
	if NOT( b.IsValid() )
	{
		return MicroPolygonGrid::MAX_SIZE / 4;
	}
	
	Vector3	boxVerts[8] =
	{
		Vector3( b.mBox[0].x, b.mBox[0].y, b.mBox[0].z ),
		Vector3( b.mBox[1].x, b.mBox[0].y, b.mBox[0].z ),
		Vector3( b.mBox[0].x, b.mBox[1].y, b.mBox[0].z ),
		Vector3( b.mBox[1].x, b.mBox[1].y, b.mBox[0].z ),
		Vector3( b.mBox[0].x, b.mBox[0].y, b.mBox[1].z ),
		Vector3( b.mBox[1].x, b.mBox[0].y, b.mBox[1].z ),
		Vector3( b.mBox[0].x, b.mBox[1].y, b.mBox[1].z ),
		Vector3( b.mBox[1].x, b.mBox[1].y, b.mBox[1].z ),
	};

	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;
	
	Matrix44	mtxLocalProj = mtxLocalCamera * mMtxCameraProj;

	for (size_t i=0; i < 8; ++i)
	{
		Vector4	Pproj = MultiplyV3W1M( boxVerts[i], mtxLocalProj );
		
		if ( Pproj.w > 0 )
		{
			float	oow = 1.0f / Pproj.w;

			float	winX = destHalfWd + destHalfWd * Pproj.x * oow;
			float	winY = destHalfHe - destHalfHe * Pproj.y * oow;
			
			minX = DMIN( minX, winX );
			minY = DMIN( minY, winY );
			maxX = DMAX( maxX, winX );
			maxY = DMAX( maxY, winY );
		}
		else
		{
			// $$$ this shouldn't happen ..once proper
			// front plane clipping is implemented 8)
			return 0.0f;
		}
	}
	
	if ( maxX > minX && maxY > minY )
	{
		float	squareArea = (maxY - minY) * (maxX - minX);
		return squareArea;
	}
	else
		return 0.0f;	// invalid or zero area...
}

//==================================================================
void HiderREYES::pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n )
{
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	for (size_t i=0; i < n; ++i)
	{
		Vector4	Pproj = MultiplyV3W1M( pSrc[i], mMtxCameraProj );
		
		float	oow = 1.0f / Pproj.w;
		
		pDes[i].Set( 
				destHalfWd + destHalfWd * Pproj.x * oow,
				destHalfHe - destHalfHe * Pproj.y * oow
				);
	}
}

//==================================================================
void HiderREYES::Hide( MicroPolygonGrid &g )
{
	float du = (g.mURange[1] - g.mURange[0]) / g.mXDim;
	float dv = (g.mVRange[1] - g.mVRange[0]) / g.mYDim;
	
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	const Point3	*pRuns	= g.mpPoints;

	const Color	*pOi = (const Color *)g.mSymbols.LookupVariableData( "Oi", SlSymbol::COLOR, true );
	const Color	*pCi = (const Color *)g.mSymbols.LookupVariableData( "Ci", SlSymbol::COLOR, true );

	u_int destWd	= mDestBuff.mWd;
	u_int destHe	= mDestBuff.mHe;


	for (u_int iv=0; iv < g.mYDim; ++iv)
	{
		float	v = g.mVRange[0] + iv * dv;
		
		for (u_int iu=0; iu < g.mXDim; ++iu)
		{
			float	u = g.mURange[0] + iu * du;
			
			Vector4	Pproj = MultiplyV3W1M( *pRuns++, mMtxCameraProj );
			
			float	oow = 1.0f / Pproj.w;

			int	winX = (int)(destHalfWd + destHalfWd * Pproj.x * oow);
			int	winY = (int)(destHalfHe - destHalfHe * Pproj.y * oow);
			
			if ( (u_int)winX >= destWd || (u_int)winY >= destHe )
				continue;

#if 0
			float	destCol[3] =
			{
				u,
				v,
				0
			};
#else
			float	destCol[3] =
			{
				pCi->x,
				pCi->y,
				pCi->z
			};
#endif
			++pCi;
			++pOi;

			float	*pZSample = mZBuff.GetSamplePtr( winX, winY );
			if ( Pproj.z < *pZSample )
			{
				*pZSample = Pproj.z;
				mDestBuff.SetSample( winX, winY, destCol );
			}
		}
	}

}

//==================================================================
}
