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
inline void updateMinMax( Float3 &minf, Float3 &maxf, const Float3 &val )
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
				const Float3			&minPos,
				const Float3			&maxPos,
				int					buckWd,
				int					buckHe,
				const Float3			microquad[4],
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
	Float3 d10 = microquad[1] - microquad[0];
	Float3 d31 = microquad[3] - microquad[1];
	Float3 d23 = microquad[2] - microquad[3];
	Float3 d02 = microquad[0] - microquad[2];

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
					HiderSampleData &sampData = Dgrow( pixel.mpSampDataLists[i] );

					sampData.mOi[0] = valOi[0];
					sampData.mOi[1] = valOi[1];
					sampData.mOi[2] = valOi[2];

					sampData.mCi[0] = valCi[0];
					sampData.mCi[1] = valCi[1];
					sampData.mCi[2] = valCi[2];

					sampData.mDepth = microquad[0][2];
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

	//const Float3_	 *pN = (const Float3_  *)workGrid.mSymbolIs.FindSymbolIData( "N"	);

	{
		static const Float_	one( 1 );

		Float_ screenCx  = Float_( screenWd * 0.5f );
		Float_ screenCy  = Float_( screenHe * 0.5f );
		Float_ screenHWd = Float_( screenWd * 0.5f );
		Float_ screenHHe = Float_( screenHe * 0.5f );

		const Float3_	*pPointsCS	= (const Float3_ *)workGrid.mpPointsCS;

		size_t	blocksN = DMT_SIMD_BLOCKS( workGrid.mPointsN );
		for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
		{
			Float4_		homoP = V4__V3W1_Mul_M44<Float_>( pPointsCS[ blkIdx ], mOptions.mMtxCamProj );

			Float4_		projP = homoP / homoP.w();

			shadGrid.mpPointsCS[ blkIdx ]			= pPointsCS[ blkIdx ];
			//shadGrid.mpPointsCloseCS[ blkIdx ]	= 0;

			shadGrid.mpPosWin[ blkIdx ][0] =  projP.x() * screenHWd + screenCx;
			shadGrid.mpPosWin[ blkIdx ][1] = -projP.y() * screenHHe + screenCy;

			shadGrid.mpCi[ blkIdx ] = pCi[ blkIdx ];
			shadGrid.mpOi[ blkIdx ] = pOi[ blkIdx ];
		}
	}

	DASSERT( workGrid.mXDim == DMT_SIMD_BLOCKS( workGrid.mXDim ) * DMT_SIMD_FLEN );

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
				u_int	blk = (u_int)srcVertIdx / DMT_SIMD_FLEN;
				u_int	sub = (u_int)srcVertIdx & (DMT_SIMD_FLEN-1);

				int pixX = (int)floor( shadGrid.mpPosWin[ blk ][0][ sub ] - (float)bucket.mX1 );
				int pixY = (int)floor( shadGrid.mpPosWin[ blk ][1][ sub ] - (float)bucket.mY1 );

				if ( pixX >= 0 && pixY >= 0 && pixX < (int)buckWd && pixY < (int)buckHe )
				{
					HiderPixel	&pixel = pixels[ pixY * buckWd + pixX ];

					HiderSampleData &sampData = Dgrow( pixel.mpSampDataLists[0] );

					sampData.mOi[0] = shadGrid.mpOi[ blk ][0][ sub ];
					sampData.mOi[1] = shadGrid.mpOi[ blk ][1][ sub ];
					sampData.mOi[2] = shadGrid.mpOi[ blk ][2][ sub ];

					sampData.mCi[0] = shadGrid.mpCi[ blk ][0][ sub ];
					sampData.mCi[1] = shadGrid.mpCi[ blk ][1][ sub ];
					sampData.mCi[2] = shadGrid.mpCi[ blk ][2][ sub ];

					sampData.mDepth = shadGrid.mpPointsCS[ blk ][2][ sub ];
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
							(u_int)srcVertIdx + 0,
							(u_int)srcVertIdx + 1,
							(u_int)srcVertIdx + xN+1,
							(u_int)srcVertIdx + xN+2 };

				Float3	minPos(  FLT_MAX,  FLT_MAX,  FLT_MAX );
				Float3	maxPos( -FLT_MAX, -FLT_MAX, -FLT_MAX );

				// calculate the bounds in window space and orientation
				//	--------
				//	| /\   |
				//	|/   \ |
				//	|\    /|
				//	|  \ / |
				//	--------
				size_t	blk[4];
				size_t	sub[4];
				Float3	buckPos[4];

				for (size_t k=0; k < 4; ++k)
				{
					blk[k] = vidx[k] / DMT_SIMD_FLEN;
					sub[k] = vidx[k] & (DMT_SIMD_FLEN-1);

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
