//==================================================================
/// RI_Hider_Micropolys.cpp
///
/// Created by Davide Pasca - 2010/1/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_HiderST.h"

//==================================================================
namespace RI
{

//==================================================================
inline void updateMinMax( Vec3f &minf, Vec3f &maxf, const Vec3f &val )
{
	if ( val[0] < minf[0] ) minf[0] = val[0];
	if ( val[1] < minf[1] ) minf[1] = val[1];
	if ( val[2] < minf[2] ) minf[2] = val[2];

	if ( val[0] > maxf[0] ) maxf[0] = val[0];
	if ( val[1] > maxf[1] ) maxf[1] = val[1];
	if ( val[2] > maxf[2] ) maxf[2] = val[2];
}

//==================================================================
// NOTE: all coords here are in bucket space
inline void addMPSamples(
				const HiderSampleCoordsBuffer	&sampCoordsBuff,
				HiderPixel			*pPixels,
				const Vec3f			&minPos,
				const Vec3f			&maxPos,
				int					buckWd,
				int					buckHe,
				const Vec3f			microquad[4],
				const float			*valOi,
				const float			*valCi
			)
{
	int	minX = (int)floor( minPos[0] );
	int	maxX = (int) ceil( maxPos[0] );

	int	minY = (int)floor( minPos[1] );
	int	maxY = (int) ceil( maxPos[1] );

	// completely out ?
	if ( maxX < 0 || maxY < 0 || minX >= buckWd || minY >= buckHe )
		return;

	// clip
	if ( minX < 0 )	minX = 0;
	if ( minY < 0 )	minY = 0;
	if ( maxX >= buckWd )	maxX = buckWd-1;
	if ( maxY >= buckHe )	maxY = buckHe-1;

/*
   0__
   /  --
  /     -- 1
 /__      /
2   --   /
      --/
	    3
*/
	Vec3f d10 = microquad[1] - microquad[0];
	Vec3f d31 = microquad[3] - microquad[1];
	Vec3f d23 = microquad[2] - microquad[3];
	Vec3f d02 = microquad[0] - microquad[2];

	u_int sampsPerPixel = sampCoordsBuff.GetSampsPerPixel();

	HiderPixel	*pPixelsRow = pPixels + minY * buckWd;

	for (int y=minY; y <= maxY; ++y)
	{
		for (int x=minX; x <= maxX; ++x)
		{
			HiderPixel	&pixel = pPixelsRow[ x ];

			for (u_int i=0; i < sampsPerPixel; ++i)
			{
				const HiderSampleCoords &sampCoords = pixel.mpSampCoords[i];

				float sampX = (float)x + sampCoords.mX;
				float sampY = (float)y + sampCoords.mY;

				float crs0 = ((sampY-microquad[0][1]) * d10[0] - (sampX-microquad[0][0]) * d10[1]);
				float crs1 = ((sampY-microquad[1][1]) * d31[0] - (sampX-microquad[1][0]) * d31[1]);
				float crs2 = ((sampY-microquad[3][1]) * d23[0] - (sampX-microquad[3][0]) * d23[1]);
				float crs3 = ((sampY-microquad[2][1]) * d02[0] - (sampX-microquad[2][0]) * d02[1]);

				if (
					(crs0 <= 0 && crs1 <= 0 && crs2 <= 0 && crs3 <= 0) ||
					(crs0 >= 0 && crs1 >= 0 && crs2 >= 0 && crs3 >= 0)
					)
				{
					HiderSampleData *pSampData = pixel.mpSampDataLists[i].grow();

					pSampData->mOi[0] = valOi[0];
					pSampData->mOi[1] = valOi[1];
					pSampData->mOi[2] = valOi[2];

					pSampData->mCi[0] = valCi[0];
					pSampData->mCi[1] = valCi[1];
					pSampData->mCi[2] = valCi[2];

					pSampData->mDepth = microquad[0][2];
				}
			}
		}

		pPixelsRow += buckWd;
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
	const SlColor	*pOi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Oi" );
	const SlColor	*pCi = (const SlColor *)workGrid.mSymbolIs.FindSymbolIData( "Ci" );

	//const SlVec3	 *pN = (const SlVec3  *)workGrid.mSymbolIs.FindSymbolIData( "N"	);

	{
		static const SlScalar	one( 1 );

		SlScalar screenCx  = SlScalar( screenWd * 0.5f );
		SlScalar screenCy  = SlScalar( screenHe * 0.5f );
		SlScalar screenHWd = SlScalar( screenWd * 0.5f );
		SlScalar screenHHe = SlScalar( screenHe * 0.5f );

		const SlVec3	*pPointsWS	= (const SlVec3 *)workGrid.mpPointsWS;

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
	}

	DASSERT( workGrid.mXDim == RI_GET_SIMD_BLOCKS( workGrid.mXDim ) * RI_SIMD_BLK_LEN );

	u_int	xN		= workGrid.mXDim - 1;
	u_int	yN		= workGrid.mYDim - 1;

	u_int	buckWd	= bucket.GetWd();
	u_int	buckHe	= bucket.GetHe();

	if ( mParams.mDbgRasterizeVerts )
	{
		// scan the grid.. for every vertex
		size_t	srcVertIdx = 0;
		for (u_int i=0; i < yN; ++i)
		{
			for (u_int j=0; j < xN; ++j, ++srcVertIdx)
			{
				u_int	blk = (u_int)srcVertIdx / RI_SIMD_BLK_LEN;
				u_int	sub = (u_int)srcVertIdx & (RI_SIMD_BLK_LEN-1);

				int pixX = (int)floor( shadGrid.mpPosWin[ blk ][0][ sub ] - (float)bucket.mX1 );
				int pixY = (int)floor( shadGrid.mpPosWin[ blk ][1][ sub ] - (float)bucket.mY1 );

				if ( pixX >= 0 && pixY >= 0 && pixX < (int)buckWd && pixY < (int)buckHe )
				{
					HiderPixel	&pixel = pixels[ pixY * buckWd + pixX ];

					HiderSampleData *pSampData = pixel.mpSampDataLists[0].grow();

					pSampData->mOi[0] = shadGrid.mpOi[ blk ][0][ sub ];
					pSampData->mOi[1] = shadGrid.mpOi[ blk ][1][ sub ];
					pSampData->mOi[2] = shadGrid.mpOi[ blk ][2][ sub ];

					pSampData->mCi[0] = shadGrid.mpCi[ blk ][0][ sub ];
					pSampData->mCi[1] = shadGrid.mpCi[ blk ][1][ sub ];
					pSampData->mCi[2] = shadGrid.mpCi[ blk ][2][ sub ];

					pSampData->mDepth = shadGrid.mpPointsCS[ blk ][2][ sub ];
				}
			}

			srcVertIdx += 1;
		}
	}
	else
	{
		// scan the grid.. for every potential micro-polygon
		size_t	srcVertIdx = 0;
		for (u_int i=0; i < yN; ++i)
		{
			for (u_int j=0; j < xN; ++j, ++srcVertIdx)
			{
				// vector coords of the micro-poly
				u_int	vidx[4] = {
							srcVertIdx + 0,
							srcVertIdx + 1,
							srcVertIdx + xN+1,
							srcVertIdx + xN+2 };

				Vec3f	minPos(  FLT_MAX,  FLT_MAX,  FLT_MAX );
				Vec3f	maxPos( -FLT_MAX, -FLT_MAX, -FLT_MAX );

				// calculate the bounds in window space and orientation
				//	--------
				//	| /\   |
				//	|/   \ |
				//	|\    /|
				//	|  \ / |
				//	--------
				size_t	blk[4];
				size_t	sub[4];
				Vec3f	buckPos[4];

				for (size_t k=0; k < 4; ++k)
				{
					blk[k] = vidx[k] / RI_SIMD_BLK_LEN;
					sub[k] = vidx[k] & (RI_SIMD_BLK_LEN-1);

					buckPos[k][0] = shadGrid.mpPosWin[ blk[k] ][0][ sub[k] ] - (float)bucket.mX1;
					buckPos[k][1] = shadGrid.mpPosWin[ blk[k] ][1][ sub[k] ] - (float)bucket.mY1;
					buckPos[k][2] = shadGrid.mpPointsCS[ blk[k] ][2][ sub[k] ];

					updateMinMax( minPos, maxPos, buckPos[k] );
				}

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
						*bucket.mpSampCoordsBuff,
						&pixels[0],
						minPos,
						maxPos,
						(int)buckWd,
						(int)buckHe,
						buckPos,
						valOi,
						valCi );
			}

			srcVertIdx += 1;
		}
	}
}

//==================================================================
}
