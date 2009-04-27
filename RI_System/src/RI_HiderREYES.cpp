/*
 *  RI_HiderREYES.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Primitive.h"
#include "RI_HiderREYES.h"
#include "RI_Transform.h"

static const u_int	BUCKET_SIZE = 128;

//==================================================================
namespace RI
{

//==================================================================
static void MakeCube( const Bound &b, Vector3 out_box[8] )
{
	out_box[0] = Vector3( b.mBox[0].x, b.mBox[0].y, b.mBox[0].z );
	out_box[1] = Vector3( b.mBox[1].x, b.mBox[0].y, b.mBox[0].z );
	out_box[2] = Vector3( b.mBox[0].x, b.mBox[1].y, b.mBox[0].z );
	out_box[3] = Vector3( b.mBox[1].x, b.mBox[1].y, b.mBox[0].z );
	out_box[4] = Vector3( b.mBox[0].x, b.mBox[0].y, b.mBox[1].z );
	out_box[5] = Vector3( b.mBox[1].x, b.mBox[0].y, b.mBox[1].z );
	out_box[6] = Vector3( b.mBox[0].x, b.mBox[1].y, b.mBox[1].z );
	out_box[7] = Vector3( b.mBox[1].x, b.mBox[1].y, b.mBox[1].z );
}

//==================================================================
/// HiderREYES
//==================================================================
HiderREYES::HiderREYES()
{
}

//==================================================================
HiderREYES::~HiderREYES()
{
	for (size_t i=0; i < mpBuckets.size(); ++i)
		delete mpBuckets[i];
}

//==================================================================
void HiderREYES::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxWorldProj	= mMtxWorldCamera * opt.mMtxCamProj;
	
	mDestBuff.Setup( opt.mXRes, opt.mYRes );
	mDestBuff.Clear();
	
	mZBuff.Setup( opt.mXRes, opt.mYRes );
	mZBuff.Fill( FLT_MAX );

	for (int y=0; y < opt.mYRes; y += BUCKET_SIZE)
	{
		int	y2 = y + BUCKET_SIZE;
		y2 = DMIN( y2, opt.mYRes );

		for (int x=0; x < opt.mXRes; x += BUCKET_SIZE)
		{
			int	x2 = x + BUCKET_SIZE;
			x2 = DMIN( x2, opt.mXRes );

			if ( mDbg.mOnlyBucketAtX == -1 ||
				 mDbg.mOnlyBucketAtX >= x &&
				 mDbg.mOnlyBucketAtY >= y &&
				 mDbg.mOnlyBucketAtX < x2 &&
				 mDbg.mOnlyBucketAtY < y2 )
			{
				mpBuckets.push_back(
						new Bucket( x, y, x2, y2 ) );
			}
		}
	}
}

//==================================================================
void HiderREYES::Insert( Primitive *pPrim )
{
	Bound	bound;
	if NOT( pPrim->MakeBound( bound ) )
	{
		// not boundable.. needs split !
		mpBuckets[0]->mpPrims.push_back( pPrim );
	}
	else
	{
		const Matrix44 &mtxLocalWorld = pPrim->mpTransform->GetMatrix();

		float bound2df[4];

		if NOT( makeRasterBound( bound, mtxLocalWorld, bound2df ) )
		{
			// delete what we can't understand 8)
			pPrim->MarkUnusable();
			//DSAFE_DELETE( pPrim );
			return;
		}

		int minX = (int)floorf( bound2df[0] );
		int minY = (int)floorf( bound2df[1] );
		int maxX =  (int)ceilf( bound2df[2] );
		int maxY =  (int)ceilf( bound2df[3] );

		if ( minX >= (int)mDestBuff.mWd ||
			 minY >= (int)mDestBuff.mHe ||
			 maxX <	0 ||
			 maxY <	0 )
		{
			// completely outside
			pPrim->MarkUnusable();
			//DSAFE_DELETE( pPrim );
			return;
		}

		//int	clampedMinX = D::Clamp( minX, 0, (int)mDestBuff.mWd-1 );
		//int	clampedMinY = D::Clamp( minY, 0, (int)mDestBuff.mHe-1 );

		for (size_t i=0; i < mpBuckets.size(); ++i)
		{
			//if ( mpBuckets[i]->Contains( clampedMinX, clampedMinY ) )
			if ( mpBuckets[i]->Intersects( minX, minY, maxX, maxY ) )
			{
				mpBuckets[i]->mpPrims.push_back( pPrim );
				break;
			}
		}
	}
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

	Insert( pSplitPrim );
}

//==================================================================
void HiderREYES::Remove( Primitive *pPrim )
{
}

//==================================================================
void HiderREYES::WorldEnd()
{
	if ( mDbg.mShowBuckets )
	{
		float borderCol[] = { 1, 0, 0 };
		for (size_t bi=0; bi < mpBuckets.size(); ++bi)
		{
			const Bucket	*pBucket = mpBuckets[bi];

			mDestBuff.DrawHLine( pBucket->mX1, pBucket->mY1, pBucket->mX2, borderCol );
			mDestBuff.DrawHLine( pBucket->mX1, pBucket->mY2, pBucket->mX2, borderCol );

			mDestBuff.DrawVLine( pBucket->mX1, pBucket->mY1, pBucket->mY2, borderCol );
			mDestBuff.DrawVLine( pBucket->mX2, pBucket->mY1, pBucket->mY2, borderCol );
		}
	}

	for (size_t i=0; i < mpBuckets.size(); ++i)
		delete mpBuckets[i];

	mpBuckets.clear();
}

//==================================================================
bool HiderREYES::makeRasterBound(
						const Bound &b,
						const Matrix44 &mtxLocalWorld,
						float out_bound2d[4] ) const
{
	Vector3	boxVerts[8];
	MakeCube( b, boxVerts );

	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	Matrix44	mtxLocalProj = mtxLocalWorld * mMtxWorldProj;

	for (size_t i=0; i < 8; ++i)
	{
		Vector4	Pproj = V4__V3W1_Mul_M44( boxVerts[i], mtxLocalProj );
		
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
			DASSERT( 0 );
		}
	}

	out_bound2d[0] = minX;
	out_bound2d[1] = minY;
	out_bound2d[2] = maxX;
	out_bound2d[3] = maxY;

	return minX < maxX && minY < maxY;
}

//==================================================================
float HiderREYES::RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const
{
	if NOT( b.IsValid() )
	{
		return MicroPolygonGrid::MAX_SIZE / 4;
	}

	float	bound2d[4];

	bool valid = makeRasterBound( b, mtxLocalWorld, bound2d );

	if ( valid )
	{
		float	squareArea = (bound2d[3] - bound2d[1]) * (bound2d[2] - bound2d[0]);
		return squareArea * 2;
	}
	else
		return 0.0f;	// invalid or zero area...
}

/*
//==================================================================
void HiderREYES::pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n )
{
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	for (size_t i=0; i < n; ++i)
	{
		Vector4	Pproj = MultiplyV3W1M( pSrc[i], mMtxCamProj );
		
		float	oow = 1.0f / Pproj.w;
		
		pDes[i].Set( 
				destHalfWd + destHalfWd * Pproj.x * oow,
				destHalfHe - destHalfHe * Pproj.y * oow
				);
	}
}
*/

//==================================================================
void HiderREYES::Hide( MicroPolygonGrid &g )
{
	float du = (g.mURange[1] - g.mURange[0]) / g.mXDim;
	float dv = (g.mVRange[1] - g.mVRange[0]) / g.mYDim;
	
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	const Point3	*pRunsWS	= g.mpPointsWS;

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
			
			Vector4	Pproj = V4__V3W1_Mul_M44( *pRunsWS++, mMtxWorldProj );
			
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
