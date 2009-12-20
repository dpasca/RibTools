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
#define MAX_SAMP_DATA_PER_PIXEL	1024

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
void Hider::WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;

	mMtxWorldCamera	= mtxWorldCamera;
	mMtxWorldProj	= mMtxWorldCamera * opt.mMtxCamProj;
	
	mFinalBuff.Setup( opt.mXRes, opt.mYRes );
	mFinalBuff.Clear();
	
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
						findOrAddSampCoordBuff( x2 - x, y2 - y, 1 );

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
					SimplePrimitiveBase		*pSimplePrim,
					ComplexPrimitiveBase	&srcPrim
					)
{
	pSimplePrim->CopyStates( srcPrim );

	mpPrims.push_back( pSimplePrim->Borrow() );
}

//==================================================================
void Hider::InsertSplitted(	
				SimplePrimitiveBase	*pDesPrim,
				SimplePrimitiveBase	&srcPrim
				)
{
	pDesPrim->CopyStates( srcPrim );

	pDesPrim->mSplitCnt += 1;

	mpPrims.push_back( pDesPrim->Borrow() );
}

//==================================================================
void Hider::InsertForDicing( SimplePrimitiveBase *pPrim )
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

	float destHalfWd	= (float)mFinalBuff.mWd * 0.5f;
	float destHalfHe	= (float)mFinalBuff.mHe * 0.5f;

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
float Hider::RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const
{
	if NOT( b.IsValid() )
	{
		return MP_GRID_MAX_SIZE / 4;
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
inline void updateMinMax( float minf[3], float maxf[3], const float val[3] )
{
	if ( val[0] < minf[0] ) minf[0] = val[0];
	if ( val[1] < minf[1] ) minf[1] = val[1];
	if ( val[2] < minf[2] ) minf[2] = val[2];

	if ( val[0] > maxf[0] ) maxf[0] = val[0];
	if ( val[1] > maxf[1] ) maxf[1] = val[1];
	if ( val[2] > maxf[2] ) maxf[2] = val[2];
}

//==================================================================
inline float getFractional( float a )
{
	return a - floor( a );
}

//==================================================================
inline void addMPSamples(
				HiderPixel	*pPixels,
				u_int		pixPerRow,
				const float *minGlob,
				const float *maxGlob,
				int sampsPerDim,
				int subPixDimLog2,
				int minX,	// micro-poly bounds in bucket space
				int minY,
				int maxX,
				int maxY,
				const float winPos[4][3],
				float buckX1,
				float buckY1,
				const float *valOi,
				const float *valCi
				)
{
	float sampsPerDimF = (float)sampsPerDim;

	// calculate the sub-samples touched by the micro-poly
	int minSubX = (int)floor( getFractional( minGlob[0] ) * sampsPerDimF );
	int minSubY = (int)floor( getFractional( minGlob[1] ) * sampsPerDimF );
	int maxSubX =  (int)ceil( getFractional( maxGlob[0] ) * sampsPerDimF );
	int maxSubY =  (int)ceil( getFractional( maxGlob[1] ) * sampsPerDimF );

	float dxa = winPos[1][0] - winPos[0][0];
	float dxb = winPos[3][0] - winPos[1][0];
	float dxc = winPos[2][0] - winPos[3][0];
	float dxd = winPos[0][0] - winPos[2][0];

	float dya = winPos[1][1] - winPos[0][1];
	float dyb = winPos[3][1] - winPos[1][1];
	float dyc = winPos[2][1] - winPos[3][1];
	float dyd = winPos[0][1] - winPos[2][1];

	float subSampToPixFrac = 1.0f / (1 << subPixDimLog2);

	// for every pixel touched by the micro-polygon
	int	sy		= minSubY;
	int	syMax	= maxSubY - 1;
	for (int y=minY; y <= maxY; ++y)
	{
		if ( y == maxY )	syMax = maxSubY;

		HiderPixel	*pPixels2 = pPixels;

		int	sx		= minSubX;
		int	sxMax	= maxSubX - 1;
		for (int x=minX; x <= maxX; ++x)
		{
			if ( x == maxX )	sxMax = maxSubX;

			for (int ssy=sy; ssy <= syMax; ++ssy)
			{
				for (int ssx=sx; ssx <= sxMax; ++ssx)
				{
					const HiderSampleCoords *pSampCoords =
							&pPixels2->mpSampCoords[ (ssy << subPixDimLog2) + ssx ];

					float sampX = x + pSampCoords->mX * subSampToPixFrac + buckX1;
					float sampY = y + pSampCoords->mY * subSampToPixFrac + buckY1;

					if ( ((sampY-winPos[0][1]) * dxa - (sampX-winPos[0][0]) * dya) <= 0 ) continue;
					if ( ((sampY-winPos[1][1]) * dxb - (sampX-winPos[1][0]) * dyb) <= 0 ) continue;
					if ( ((sampY-winPos[3][1]) * dxc - (sampX-winPos[3][0]) * dyc) <= 0 ) continue;
					if ( ((sampY-winPos[2][1]) * dxd - (sampX-winPos[2][0]) * dyd) <= 0 ) continue;

/*
					if (sampX < minX ||
						sampX > maxX ||
						sampY < minY ||
						sampY > maxY )
					{
						continue;
					}
*/

					HiderSampleData *pSampData = pPixels2->mSampData.grow();

					pSampData->mOi[0] = valOi[0];
					pSampData->mOi[1] = valOi[1];
					pSampData->mOi[2] = valOi[2];

					pSampData->mCi[0] = valCi[0];
					pSampData->mCi[1] = valCi[1];
					pSampData->mCi[2] = valCi[2];

					pSampData->mDepth = minGlob[2];
				}
			}

			pPixels2 += 1;

			sx = 0;
		}

		pPixels += pixPerRow;

		sy = 0;
	}
}

//==================================================================
void Hider::Bust(
				const HiderBucket	&bucket,
				ShadedGrid			&shadGrid,
				const WorkGrid		&workGrid,
				DVec<HiderPixel>	&pixels,
				u_int				screenWd,
				u_int				screenHe ) const
{
	SlScalar screenCx  = SlScalar( screenWd * 0.5f );
	SlScalar screenCy  = SlScalar( screenHe * 0.5f );
	SlScalar screenHWd = SlScalar( screenWd * 0.5f );
	SlScalar screenHHe = SlScalar( screenHe * 0.5f );

	const SlVec3	*pPointsWS	= (const SlVec3 *)workGrid.mpPointsWS;

	const SlColor	*pOi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Oi" );
	const SlColor	*pCi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Ci" );

	const SlVec3	 *pN = (const SlVec3  *)workGrid.mSymbolIs.FindSymbolIData( "N"	);

	static const SlScalar	one( 1 );

	size_t	blocksN = RI_GET_SIMD_BLOCKS( workGrid.mPointsN );
	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		SlVec4		homoP = V4__V3W1_Mul_M44<SlScalar>( pPointsWS[ blkIdx ], mMtxWorldProj );

		SlVec4		projP = homoP / homoP.w();

		// TODO: should replace pPointsWS with pPointsCS... ..to avoid this and also because
		// perhaps P should indeed be in "current" space (camera)
		SlVec3		PtCS = V3__V3W1_Mul_M44<SlScalar>( pPointsWS[ blkIdx ], mMtxWorldCamera );

		shadGrid.mpPointsCS[ blkIdx ]			= PtCS;
		//shadGrid.mpPointsCloseCS[ blkIdx ]	= 0;

		shadGrid.mpPosWin[ blkIdx ][0] =  projP.x() * screenHWd + screenCx;
		shadGrid.mpPosWin[ blkIdx ][1] = -projP.y() * screenHHe + screenCy;

		shadGrid.mpCi[ blkIdx ] = pCi[ blkIdx ];
		shadGrid.mpOi[ blkIdx ] = pOi[ blkIdx ];
	}

	DASSERT( workGrid.mXDim == RI_GET_SIMD_BLOCKS( workGrid.mXDim ) * RI_SIMD_BLK_LEN );

	size_t	sampIdx = 0;
	u_int	xDim	= workGrid.mXDim;
	u_int	yDim	= workGrid.mYDim;
	u_int	xN		= workGrid.mXDim - 1;
	u_int	yN		= workGrid.mYDim - 1;

	u_int	buckWd	= bucket.GetWd();

	int	sampsPerDim = (int)bucket.mpSampCoordsBuff->GetSampsPerDim();
	int subPixDimLog2 = (int)bucket.mpSampCoordsBuff->mSubPixelDimLog2;

	u_int	pixPerRow = bucket.GetWd();

	// scan the grid.. for every potential micro-polygon
	for (u_int i=0; i < yN; ++i)
	{
		for (u_int j=0; j < xN; ++j, ++sampIdx)
		{
			// vector coords of the micro-poly
			u_int	vidx[4] = {
						sampIdx+0,
						sampIdx+1,
						sampIdx+xDim,
						sampIdx+xDim+1 };

			float	minGlob[3] = {  FLT_MAX,  FLT_MAX,  FLT_MAX };
			float	maxGlob[3] = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			// calculate the bounds in window space and orientation
			//	--------
			//	| /\   |
			//	|/   \ |
			//	|\    /|
			//	|  \ / |
			//	--------
			size_t	blk[4];
			size_t	sub[4];
			float winPos[4][3];
			for (size_t k=0; k < 4; ++k)
			{
				blk[k] = vidx[k] / RI_SIMD_BLK_LEN;
				sub[k] = vidx[k] & (RI_SIMD_BLK_LEN-1);

				winPos[k][0] = shadGrid.mpPosWin[ blk[k] ][0][ sub[k] ];
				winPos[k][1] = shadGrid.mpPosWin[ blk[k] ][1][ sub[k] ];
				winPos[k][2] = shadGrid.mpPosWin[ blk[k] ][2][ sub[k] ];

				updateMinMax( minGlob, maxGlob, winPos[k] );
			}

			// integer bounding box of the micro-poly
			int	minX = (int)floor( minGlob[0] );
			int	maxX = (int)ceil(  maxGlob[0] );
			int	minY = (int)floor( minGlob[1] );
			int	maxY = (int)ceil(  maxGlob[1] );

			// is the micro-poly bound outside the bucket ?
			if (minX <  bucket.mX1 ||
				minY <  bucket.mY1 || 
				maxX >= bucket.mX2 ||
				maxY >= bucket.mY2 )
			{
				// if not, then simply skip it
				continue;
			}

			// move the micro-poly bounding box in bucket coordinates space
			minX -= bucket.mX1;
			maxX -= bucket.mX1;
			minY -= bucket.mY1;
			maxY -= bucket.mY1;

			DASSERT( minX >= 0 && minY >= 0 && maxX < bucket.mX2 && maxY < bucket.mY2 );

			HiderPixel	*pFirstPixel = &pixels[ minY * pixPerRow + minX ];

			// sample only from the first vertex.. no bilinear
			// interpolation in the micro-poly !
			float valOi[3] =
				{
					shadGrid.mpOi[ blk[0] ][0][ sub[0] ],
					shadGrid.mpOi[ blk[0] ][1][ sub[0] ],
					shadGrid.mpOi[ blk[0] ][2][ sub[0] ]
				};
			float valCi[3] =
				{
					shadGrid.mpCi[ blk[0] ][0][ sub[0] ],
					shadGrid.mpCi[ blk[0] ][1][ sub[0] ],
					shadGrid.mpCi[ blk[0] ][2][ sub[0] ]
				};

			addMPSamples(
					pFirstPixel,
					pixPerRow,
					minGlob,
					maxGlob,
					sampsPerDim,
					subPixDimLog2,
					minX,
					minY,
					maxX,
					maxY,
					winPos,
					(float)bucket.mX1,
					(float)bucket.mY1,
					valOi,
					valCi );
		}

		sampIdx += 1;
	}
}

/*
//==================================================================
#define CCODE_X1	1
#define CCODE_X2	2
#define CCODE_Y1	4
#define CCODE_Y2	8
#define CCODE_Z1	16
#define CCODE_Z2	32

U8	ccodes[ MP_GRID_MAX_SIZE ];
U8	gridORCCodes = 0;
// would be nice to simdfy this one too
for (size_t i=0; i < RI_SIMD_BLK_LEN; ++i)
{
	float	x = homoP.x()[i];
	float	y = homoP.y()[i];
	float	z = homoP.z()[i];
	float	w = homoP.w()[i];

	U8	ccode = 0;
	if ( x < -w )	ccode |= CCODE_X1;
	if ( x >  w )	ccode |= CCODE_X2;
	if ( y < -w )	ccode |= CCODE_Y1;
	if ( y >  w )	ccode |= CCODE_Y2;
	if ( z < -w )	ccode |= CCODE_Z1;
	if ( z >  w )	ccode |= CCODE_Z2;
	ccodes[i] = ccode;
	gridORCCodes |= ccode;
}

U8	andCode = 0xff;
andCode &= ccodes[ sampIdx+0 ];
andCode &= ccodes[ sampIdx+1 ];
andCode &= ccodes[ sampIdx+xDim ];
andCode &= ccodes[ sampIdx+xDim+1 ];
// all out ? Skip this
if ( andCode )
	continue;
*/

/*
//==================================================================
void Hider::HideAddSamples(
					DVec<HiderPixel>	&pixels,
					DVec<u_int>			&pixelsSampsIdxs,
					HiderBucket			&buck,
					const ShadedGrid	&shadGrid )
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( shadGrid.mPointsN );

	int	buckWd = buck.GetWd();
	int	buckHe = buck.GetHe();

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
		for (size_t	itmIdx=0; itmIdx < RI_SIMD_BLK_LEN; ++itmIdx)
		{
			int	x = (int)(shadGrid.mpPosWin[ blkIdx ][0][ itmIdx ]) - buck.mX1;
			int	y = (int)(shadGrid.mpPosWin[ blkIdx ][1][ itmIdx ]) - buck.mY1;

			if ( x >= 0 && y >= 0 && x < buckWd && y < buckHe )
			{
				float	rgb[3];

				rgb[0] = shadGrid.mpCi[blkIdx][0][itmIdx];
				rgb[1] = shadGrid.mpCi[blkIdx][1][itmIdx];
				rgb[2] = shadGrid.mpCi[blkIdx][2][itmIdx];

				size_t		pixIdx = buckWd * y + x;
				size_t		sampIdx = pixelsSampsIdxs[ pixIdx ]++;

				HiderSampleData	*pSampData = &pixels[ pixIdx ].mSampData[ sampIdx ];

				pSampData->mDepth = shadGrid.mpPointsCS[blkIdx][2][itmIdx];

				pSampData->mCi[0] = shadGrid.mpCi[blkIdx][0][itmIdx];
				pSampData->mCi[1] = shadGrid.mpCi[blkIdx][1][itmIdx];
				pSampData->mCi[2] = shadGrid.mpCi[blkIdx][2][itmIdx];

				pSampData->mOi[0] = shadGrid.mpOi[blkIdx][0][itmIdx];
				pSampData->mOi[1] = shadGrid.mpOi[blkIdx][1][itmIdx];
				pSampData->mOi[2] = shadGrid.mpOi[blkIdx][2][itmIdx];
			}
		}
	}
}
*/

//==================================================================
static void sortSampData( HiderSampleData *pSampDataListSort[], HiderSampleData *pData, u_int dataN )
{
	if ( dataN == 1 )
	{
		pSampDataListSort[0] = pData;
		return;
	}

	// (sad ?) insert sort
	{
		int doneDataN = 0;

		for (u_int i=0; i < dataN; ++i)
		{
			float	depthI = pData[i].mDepth;

			int j=0;
			for (; j < doneDataN; ++j)
			{
				if ( depthI < pSampDataListSort[j]->mDepth )
				{
					for (int k=doneDataN-1; k >= j; --k)
						pSampDataListSort[k+1] = pSampDataListSort[k];

					break;
				}
			}

			pSampDataListSort[j] = &pData[i];
			doneDataN += 1;
		}
	}
}

//==================================================================
void Hider::Hide(
				DVec<HiderPixel>	&pixels,
				HiderBucket			&buck )
{
	u_int	buckWd = buck.GetWd();
	u_int	buckHe = buck.GetHe();

	size_t	pixIdx = 0;
	for (u_int y=0; y < buckHe; ++y)
	{
		for (u_int x=0; x < buckWd; ++x, ++pixIdx)
		{
			u_int			dataN = pixels[pixIdx].mSampData.size();

			if NOT( dataN )
				continue;

			DASSERT( dataN <= MAX_SAMP_DATA_PER_PIXEL );
			dataN = DMIN( dataN, MAX_SAMP_DATA_PER_PIXEL );

			HiderSampleData	*pData = &pixels[pixIdx].mSampData[0];

			HiderSampleData *pSampDataListSort[ MAX_SAMP_DATA_PER_PIXEL ];

			sortSampData( pSampDataListSort, pData, dataN );

			static Vec3<float>	one( 1 );

			if ( dataN > 0 )
			{
				Vec3<float>	accCol( 0.f );

				for (int i=(int)dataN-1; i >= 0; --i)
				{
					Vec3<float>	col = Vec3<float>( pSampDataListSort[i]->mCi );
					Vec3<float>	opa = Vec3<float>( pSampDataListSort[i]->mOi );

					accCol = accCol * (one - opa) + col;
				}

				buck.mCBuff.SetSample( x, y, &accCol.x() );
			}
		}
	}
}

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
