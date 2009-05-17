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
static void MakeCube( const Bound &b, Vec3f out_box[8] )
{
	out_box[0] = Vec3f( b.mBox[0][0], b.mBox[0][1], b.mBox[0][2] );
	out_box[1] = Vec3f( b.mBox[1][0], b.mBox[0][1], b.mBox[0][2] );
	out_box[2] = Vec3f( b.mBox[0][0], b.mBox[1][1], b.mBox[0][2] );
	out_box[3] = Vec3f( b.mBox[1][0], b.mBox[1][1], b.mBox[0][2] );
	out_box[4] = Vec3f( b.mBox[0][0], b.mBox[0][1], b.mBox[1][2] );
	out_box[5] = Vec3f( b.mBox[1][0], b.mBox[0][1], b.mBox[1][2] );
	out_box[6] = Vec3f( b.mBox[0][0], b.mBox[1][1], b.mBox[1][2] );
	out_box[7] = Vec3f( b.mBox[1][0], b.mBox[1][1], b.mBox[1][2] );
}

//==================================================================
/// HiderREYES
//==================================================================
HiderREYES::HiderREYES( const Params &params ) :
	mParams(params),
	mpStatics(NULL)
{
}

//==================================================================
HiderREYES::~HiderREYES()
{
	for (size_t i=0; i < mpBuckets.size(); ++i)
		DDELETE( mpBuckets[i] );
}

//==================================================================
void HiderREYES::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxWorldProj	= mMtxWorldCamera * opt.mMtxCamProj;
	
	mFinalBuff.Setup( opt.mXRes, opt.mYRes );
	mFinalBuff.Clear();
	
#if 0
	mpBuckets.push_back(
			new Bucket( 0, 0, opt.mXRes, opt.mYRes ) );
#else
	for (int y=0; y < opt.mYRes; y += BUCKET_SIZE)
	{
		int	y2 = y + BUCKET_SIZE;
		y2 = DMIN( y2, opt.mYRes );

		for (int x=0; x < opt.mXRes; x += BUCKET_SIZE)
		{
			int	x2 = x + BUCKET_SIZE;
			x2 = DMIN( x2, opt.mXRes );

			if ( mParams.mDbgOnlyBucketAtX == -1 ||
				 mParams.mDbgOnlyBucketAtX >= x &&
				 mParams.mDbgOnlyBucketAtY >= y &&
				 mParams.mDbgOnlyBucketAtX < x2 &&
				 mParams.mDbgOnlyBucketAtY < y2 )
			{
				mpBuckets.push_back(
						DNEW Bucket( x, y, x2, y2 ) );
			}
		}
	}
#endif
}

//==================================================================
void HiderREYES::Insert( PrimitiveBase *pPrim )
{
	mpPrims.push_back( pPrim->Borrow() );
}

//==================================================================
void HiderREYES::InsertSimple(	
					SimplePrimitiveBase		*pSimplePrim,
					ComplexPrimitiveBase	&srcPrim
					)
{
	pSimplePrim->CopyStates( srcPrim );

	mpPrims.push_back( pSimplePrim->Borrow() );
}

//==================================================================
void HiderREYES::InsertSplitted(	
				SimplePrimitiveBase	*pDesPrim,
				SimplePrimitiveBase	&srcPrim
				)
{
	pDesPrim->CopyStates( srcPrim );

	pDesPrim->mSplitCnt += 1;

	mpPrims.push_back( pDesPrim->Borrow() );
}

//==================================================================
void HiderREYES::InsertForDicing( SimplePrimitiveBase *pPrim )
{
	Bound	bound;
	if NOT( pPrim->MakeBound( bound ) )
	{
		DASSERT( 0 );
		return;
	}

	const Matrix44 &mtxLocalWorld = pPrim->mpTransform->GetMatrix();

	float bound2df[4];
	if NOT( makeRasterBound( bound, mtxLocalWorld, bound2df ) )
	{
		return;
	}

	int minX = (int)floorf( bound2df[0] );
	int minY = (int)floorf( bound2df[1] );
	int maxX =  (int)ceilf( bound2df[2] );
	int maxY =  (int)ceilf( bound2df[3] );

	for (size_t i=0; i < mpBuckets.size(); ++i)
	{
		if ( mpBuckets[i]->Intersects( minX, minY, maxX, maxY ) )
		{
			mpBuckets[i]->mpPrims.push_back( (SimplePrimitiveBase *)pPrim->Borrow() );
		}
	}

}

//==================================================================
void HiderREYES::WorldEnd()
{
	if ( mParams.mDbgShowBuckets )
	{
		float borderCol[] = { 1, 0, 0 };
		for (size_t bi=0; bi < mpBuckets.size(); ++bi)
		{
			const Bucket	*pBucket = mpBuckets[bi];

			mFinalBuff.DrawHLine( pBucket->mX1, pBucket->mY1, pBucket->mX2, borderCol );
			mFinalBuff.DrawHLine( pBucket->mX1, pBucket->mY2, pBucket->mX2, borderCol );

			mFinalBuff.DrawVLine( pBucket->mX1, pBucket->mY1, pBucket->mY2, borderCol );
			mFinalBuff.DrawVLine( pBucket->mX2, pBucket->mY1, pBucket->mY2, borderCol );
		}
	}

	for (size_t i=0; i < mpBuckets.size(); ++i)
		DDELETE( mpBuckets[i] );

	mpBuckets.clear();
}

//==================================================================
bool HiderREYES::makeRasterBound(
						const Bound &b,
						const Matrix44 &mtxLocalWorld,
						float out_bound2d[4] ) const
{
	Vec3f	boxVerts[8];
	MakeCube( b, boxVerts );

	float destHalfWd	= (float)mFinalBuff.mWd * 0.5f;
	float destHalfHe	= (float)mFinalBuff.mHe * 0.5f;

	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	Matrix44	mtxLocalProj = mtxLocalWorld * mMtxWorldProj;

	for (size_t i=0; i < 8; ++i)
	{
		Vec4f	Pproj = V4__V3W1_Mul_M44<Vec4f,Vec3f,float>( boxVerts[i], mtxLocalProj );

		if ( Pproj.w() > 0 )
		{
			float	oow = 1.0f / Pproj.w();

			float	winX = destHalfWd + destHalfWd * Pproj.x() * oow;
			float	winY = destHalfHe - destHalfHe * Pproj.y() * oow;

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
		Vec4f	Pproj = MultiplyV3W1M( pSrc[i], mMtxCamProj );
		
		float	oow = 1.0f / Pproj.w;
		
		pDes[i].Set( 
				destHalfWd + destHalfWd * Pproj.x * oow,
				destHalfHe - destHalfHe * Pproj.y * oow
				);
	}
}
*/

//==================================================================
void HiderREYES::Hide(
				MicroPolygonGrid	&g,
				float				destOffX,
				float				destOffY,
				u_int				screenWd,
				u_int				screenHe,
				Buffer2D<3>			&destBuff,
				Buffer2D<1>			&zBuff ) const
{
	SlScalar screenCx  = SlScalar( screenWd * 0.5f + destOffX);
	SlScalar screenCy  = SlScalar( screenHe * 0.5f + destOffY);
	SlScalar screenHWd = SlScalar( screenWd * 0.5f			 );
	SlScalar screenHHe = SlScalar( screenHe * 0.5f			 );

	u_int	destWd = destBuff.mWd;
	u_int	destHe = destBuff.mHe;

	const SlVec3	*pPointsWS	= (const SlVec3 *)g.mpPointsWS;

	const SlColor	*pOi = (const SlColor *)g.mSymbols.LookupVariableData( "Oi", SlSymbol::COLOR, true );
	const SlColor	*pCi = (const SlColor *)g.mSymbols.LookupVariableData( "Ci", SlSymbol::COLOR, true );

	size_t	blocksN = RI_GET_SIMD_BLOCKS( g.mPointsN );

	size_t	sampleIdx = 0;

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		const SlColor	&blkCi = pCi[ blkIdx ];
		const SlColor	&blkOi = pOi[ blkIdx ];

		static const SlScalar	one( 1 );

		SlVec4		Pproj = V4__V3W1_Mul_M44<SlVec4,SlVec3,SlScalar>( pPointsWS[ blkIdx ], mMtxWorldProj );
		SlScalar	oow = one / Pproj.w();

		SlScalar	vWinX =  Pproj.x() * screenHWd * oow + screenCx;
		SlScalar	vWinY = -Pproj.y() * screenHHe * oow + screenCy;

		for (size_t	itmIdx=0; itmIdx < RI_SIMD_BLK_LEN; ++itmIdx)
		{
			int winX = (int)(vWinX[itmIdx]);
			int winY = (int)(vWinY[itmIdx]);

			if ( (u_int)winX < destWd && (u_int)winY < destHe )
			{
				Color	ci(	blkCi.v3[ 0 ][ itmIdx ],
							blkCi.v3[ 1 ][ itmIdx ],
							blkCi.v3[ 2 ][ itmIdx ] );

				Color	oi(	blkOi.v3[ 0 ][ itmIdx ],
							blkOi.v3[ 1 ][ itmIdx ],
							blkOi.v3[ 2 ][ itmIdx ] );

				float	*pZSample = zBuff.GetSamplePtr( winX, winY );
				float	posProjZ = Pproj.z()[itmIdx];

				if ( posProjZ < *pZSample )
				{
					*pZSample = posProjZ;
					//destBuff.SetSample( winX, winY, (const float *)&Color( 1, 0, 0 ) );
					destBuff.SetSample( winX, winY, &ci.x() );
				}
			}

			if ( ++sampleIdx >= g.mPointsN )
			{
				// avoid processing non-samples resulting from padding !
				DASSERT( blkIdx == blocksN-1 );
				break;	// assume next loop is at the last iteration as well..
			}
		}
	}
}

//==================================================================
}
