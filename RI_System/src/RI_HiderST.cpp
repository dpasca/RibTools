//==================================================================
/// RI_HiderST.cpp
///
/// Created by Davide Pasca - 2009/02/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Primitive.h"
#include "RI_HiderST.h"
#include "RI_Transform.h"
#include "RI_MicroPolygon.h"

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
/// Hider
//==================================================================
Hider::Hider( const Params &params ) :
	mParams(params),
	mpGlobalSyms(NULL)
{
}

//==================================================================
Hider::~Hider()
{
	for (size_t i=0; i < mpBuckets.size(); ++i)
		DDELETE( mpBuckets[i] );
}

//==================================================================
HiderSampleCoordsBuffer *Hider::findOrAddSampCoordBuff( u_int wd, u_int he, u_int subPixelDimLog2 )
{
	// already exisitng ?
	for (size_t i=0; i < 4; ++i)
		if ( mSampCoordBuffs[i].IsInitialized() )
			if ( mSampCoordBuffs[i].GetWd() == wd && mSampCoordBuffs[i].GetHe() == he )
				return &mSampCoordBuffs[i];

	// create a new one
	for (size_t i=0; i < 4; ++i)
		if NOT( mSampCoordBuffs[i].IsInitialized() )
		{
			mSampCoordBuffs[i].Init( wd, he, subPixelDimLog2 );
			mSampCoordBuffs[i].Setup( 0, 0 );
			return &mSampCoordBuffs[i];
		}

	DASSERT( 0 );
	return NULL;
}

//==================================================================
static u_int findClosestSquareAreaLog2Dim( int dimX, int dimY )
{
	int	reqSubArea = dimX * dimY;

	u_int	subPixDimLog2;

	for (subPixDimLog2 = 0; subPixDimLog2 <= 8; ++subPixDimLog2)
	{
		int	dimSquareL = (1 << ((subPixDimLog2 + 0) << 1));
		int	dimSquareR = (1 << ((subPixDimLog2 + 1) << 1));

		// is the requested subpixel area less than the next log2 area
		// and greater or equal than the current log2 area ?
		if ( reqSubArea < dimSquareR && reqSubArea >= dimSquareL )
		{
			int	diffToL = DAbs( reqSubArea - dimSquareL );
			int	diffToR = DAbs( reqSubArea - dimSquareR );

			if ( diffToR < diffToL )
				++subPixDimLog2;

			break;
		}
	}

	return subPixDimLog2;
}

//==================================================================
void Hider::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxWorldProj	= mMtxWorldCamera * opt.mMtxCamProj;
	
	mFinalBuff.Setup( opt.mXRes, opt.mYRes );
	mFinalBuff.Clear();

	u_int subPixDimLog2 =
			findClosestSquareAreaLog2Dim(
					opt.mDisp.mPixSamples[0],
					opt.mDisp.mPixSamples[1] );
	
#if 0
	// DNEW instead ?
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
				HiderSampleCoordsBuffer	*pSampCoordsBuff =
						findOrAddSampCoordBuff( x2 - x, y2 - y, subPixDimLog2 );

				mpBuckets.push_back(
						DNEW HiderBucket( x, y, x2, y2, pSampCoordsBuff ) );
			}
		}
	}
#endif
}

//==================================================================
void Hider::Insert( PrimitiveBase *pPrim )
{
	mpPrims.push_back( pPrim->Borrow() );
}

//==================================================================
void Hider::InsertSimple(	
					SimplePrimitiveBase			*pSimplePrim,
					const ComplexPrimitiveBase	&srcPrim
					)
{
	pSimplePrim->CopyStates( srcPrim );

	mpPrims.push_back( pSimplePrim->Borrow() );
}

//==================================================================
void Hider::InsertSplitted(	
				SimplePrimitiveBase			*pDesPrim,
				const SimplePrimitiveBase	&srcPrim
				)
{
	pDesPrim->CopyStates( srcPrim );

	pDesPrim->mSplitCnt += 1;

	mpPrims.push_back( pDesPrim->Borrow() );
}

//==================================================================
void Hider::InsertForDicing( SimplePrimitiveBase *pPrim, const int bound2d[2] )
{
	for (size_t i=0; i < mpBuckets.size(); ++i)
	{
		if ( mpBuckets[i]->Intersects( bound2d[0], bound2d[1], bound2d[2], bound2d[3] ) )
		{
			mpBuckets[i]->mpPrims.push_back( (SimplePrimitiveBase *)pPrim->Borrow() );
		}
	}
}

//==================================================================
void Hider::WorldEnd()
{
	if ( mParams.mDbgShowBuckets )
	{
		float borderCol[] = { 1, 0, 0 };
		for (size_t bi=0; bi < mpBuckets.size(); ++bi)
		{
			const HiderBucket	*pBucket = mpBuckets[bi];

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
bool Hider::makeRasterBound(
						const Bound &b,
						const Matrix44 &mtxLocalWorld,
						float out_bound2d[4] ) const
{
	Vec3f	boxVerts[8];
	MakeCube( b, boxVerts );

	float destHalfWd	= (float)mOptions.mXRes * 0.5f;
	float destHalfHe	= (float)mOptions.mYRes * 0.5f;

	float minX =  FLT_MAX;
	float minY =  FLT_MAX;
	float maxX = -FLT_MAX;
	float maxY = -FLT_MAX;

	Matrix44	mtxLocalProj = mtxLocalWorld * mMtxWorldProj;

	static const U32	CCODE_X1 = 1;
	static const U32	CCODE_X2 = 2;
	static const U32	CCODE_Y1 = 4;
	static const U32	CCODE_Y2 = 8;
	static const U32	CCODE_Z1 = 16;
	static const U32	CCODE_Z2 = 32;

	U32 orCode = 0;
	U32 andCode = 0xff;

	for (size_t i=0; i < 8; ++i)
	{
		Vec4f	Pproj = V4__V3W1_Mul_M44<float>( boxVerts[i], mtxLocalProj );

		float	x = Pproj.x();
		float	y = Pproj.y();
		float	z = Pproj.z();
		float	w = Pproj.w();
		
		U32 code = 0;
		if ( x < -w )	code |= CCODE_X1;
		if ( x >  w )	code |= CCODE_X2;
		if ( y < -w )	code |= CCODE_Y1;
		if ( y >  w )	code |= CCODE_Y2;
		if ( z < -w )	code |= CCODE_Z1;
		if ( z >  w )	code |= CCODE_Z2;

		orCode |= code;
		andCode &= code;

		if ( w > 0 )
		{
			float	oow = 1.0f / w;

			float	winX = destHalfWd + destHalfWd * x * oow;
			float	winY = destHalfHe - destHalfHe * y * oow;

			minX = DMIN( minX, winX );
			minY = DMIN( minY, winY );
			maxX = DMAX( maxX, winX );
			maxY = DMAX( maxY, winY );
		}
		else
		{
			// $$$ this shouldn't happen ..once proper
			// front plane clipping is implemented 8)
			//DASSERT( 0 );
			return false;
		}
	}

	if ( andCode )
		return false;

	out_bound2d[0] = minX;
	out_bound2d[1] = minY;
	out_bound2d[2] = maxX;
	out_bound2d[3] = maxY;

	return minX < maxX && minY < maxY;
}

//==================================================================
float Hider::RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld, int out_box2D[4] ) const
{
	if NOT( b.IsValid() )
	{
		return MP_GRID_MAX_SIZE / 4;
	}

	float	bound2df[4];

	bool valid = makeRasterBound( b, mtxLocalWorld, bound2df );

	out_box2D[0] = (int)floorf( bound2df[0] );
	out_box2D[1] = (int)floorf( bound2df[1] );
	out_box2D[2] =  (int)ceilf( bound2df[2] );
	out_box2D[3] =  (int)ceilf( bound2df[3] );

	if ( valid )
	{
		float	squareArea = (bound2df[3] - bound2df[1]) * (bound2df[2] - bound2df[0]);
		return squareArea * 1;
	}
	else
		return 0.0f;	// invalid or zero area...
}

/*
//==================================================================
SlScalar Hider::RasterLengthSqr(
					const SlVec3 &ptA,
					const SlVec3 &ptB,
					const Matrix44 &mtxLocalWorld ) const
{
	Matrix44	mtxLocalProj = mtxLocalWorld * mMtxWorldProj;

	SlVec4	ptAProj = V4__V3W1_Mul_M44<float>( ptA, mtxLocalProj );
	SlVec4	ptBProj = V4__V3W1_Mul_M44<float>( ptB, mtxLocalProj );

	DASSERT( ptAProj.w() > SlScalar( 0.f ) || ptAProj.w() < SlScalar( 0.f ) );
	DASSERT( ptBProj.w() > SlScalar( 0.f ) || ptBProj.w() < SlScalar( 0.f ) );

	SlVec2	ptAWin = ptAProj.GetAsV2() / ptAProj.w();
	SlVec2	ptBWin = ptBProj.GetAsV2() / ptBProj.w();

	SlVec2	diff = ptBWin - ptAWin;

	SlScalar	xDiffWin = diff.x() * (float)mOptions.mXRes;
	SlScalar	yDiffWin = diff.y() * (float)mOptions.mYRes;

	return	xDiffWin * xDiffWin + yDiffWin * yDiffWin;
}
*/

/*
//==================================================================
void Hider::pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n )
{
	float destHalfWd	= (float)mDestBuff.mWd * 0.5f;
	float destHalfHe	= (float)mDestBuff.mHe * 0.5f;

	for (size_t i=0; i < n; ++i)
	{
		Vec4f	homoP = MultiplyV3W1M( pSrc[i], mMtxCamProj );
		
		float	oow = 1.0f / homoP.w;
		
		pDes[i].Set( 
				destHalfWd + destHalfWd * homoP.x * oow,
				destHalfHe - destHalfHe * homoP.y * oow
				);
	}
}
*/

//==================================================================
size_t Hider::GetOutputBucketMemSize( size_t buckIdx ) const
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd = (u_int)(buck.mX2 - buck.mX1);
	u_int	he = (u_int)(buck.mY2 - buck.mY1);

	return sizeof(float) * NCOLS * wd * he;
}

//==================================================================
void Hider::CopyOutputBucket( size_t buckIdx, float *pDest, size_t destMaxSize ) const
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd	= (u_int)(buck.mX2 - buck.mX1);
	u_int	he	= (u_int)(buck.mY2 - buck.mY1);

	const float *pSrc = GetOutputData( buck.mX1, buck.mY1 );

	size_t wdStride = wd * NCOLS;
	size_t srcStride = GetOutputDataStride();

	U8	*pDestEnd = (U8 *)pDest + destMaxSize;

	for (u_int y=0; y < he; ++y)
	{
		memcpy( pDest, pSrc, sizeof(float) * wdStride );
		pDest += wdStride;
		pSrc += srcStride;
	}

	DASSERT( (U8*)pDest == pDestEnd );
}

//==================================================================
void Hider::StoreOutputBucket( size_t buckIdx, const float *pSrc, size_t srcSize )
{
	const HiderBucket	&buck = *mpBuckets[ buckIdx ];

	u_int	wd	= (u_int)(buck.mX2 - buck.mX1);
	u_int	he	= (u_int)(buck.mY2 - buck.mY1);

	float *pDest = GetOutputDataRW( buck.mX1, buck.mY1 );
	size_t desStride = GetOutputDataStride();

	size_t wdStride = wd * NCOLS;

	U8	*pSrcEnd = (U8 *)pSrc + srcSize;

	for (u_int y=0; y < he; ++y)
	{
		memcpy( pDest, pSrc, sizeof(float) * wdStride );

		pSrc	+= wdStride;
		pDest	+= desStride;
	}

	DASSERT( (U8*)pSrc == pSrcEnd );
}

//==================================================================
}
