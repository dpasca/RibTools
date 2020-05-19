//==================================================================
/// RI_Hider_Filter.cpp
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
#define MAX_DATA_PER_SAMPLE	1024

//==================================================================
static void sortSampData(
				const HiderSampleData	**pSampDataListSort,
				const HiderSampleData	*pData,
				size_t					dataN )
{
	if ( dataN == 1 )
	{
		pSampDataListSort[0] = pData;
		return;
	}

	// (sad ?) insert sort
	{
		int doneDataN = 0;

		for (size_t i=0; i < dataN; ++i)
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
inline void filterPixelBox(
				Float4				&pixCol,
				const HiderPixel	&pixel,
				u_int				sampsPerPixel,
				float				ooSampsPerPixel )
{
	static Vec3<float>	one( 1 );

	for (u_int si=0; si < sampsPerPixel; ++si)
	{
		const DVec<HiderSampleData> &sampDataList = pixel.mpSampDataLists[si];

		size_t	dataN = sampDataList.size();
		if NOT( dataN )
			continue;

		DASSERT( dataN <= MAX_DATA_PER_SAMPLE );
		dataN = DMIN( dataN, MAX_DATA_PER_SAMPLE );

		const HiderSampleData	*pData = &sampDataList[0];

		const HiderSampleData *pSampDataListSort[ MAX_DATA_PER_SAMPLE ];

		sortSampData( pSampDataListSort, pData, dataN );

		// TODO: optimize by considering only until the first fully
		//       opaque sample from front

		Float3	accCol( 0.f );
		Float3	accOpa( 0.f );

		for (int i=(int)dataN-1; i >= 0; --i)
		{
			Vec3<float>	col = Vec3<float>( pSampDataListSort[i]->mCi );
			Vec3<float>	opa = Vec3<float>( pSampDataListSort[i]->mOi );

			accCol = accCol * (one - opa) + col;
			accOpa += opa;
		}

		float	accSingleOpa = (accOpa.x() + accOpa.y() + accOpa.z()) / 3;

		accSingleOpa = D::Clamp( accSingleOpa, 0.f, 1.f );

		pixCol += Float4( accCol.x(), accCol.y(), accCol.z(), accSingleOpa );
		//pixCol = pSampDataListSort[0]->mCi;
	}

	pixCol = pixCol * ooSampsPerPixel;
}

//==================================================================
void Hider::Hide(
				DVec<HiderPixel>	&pixels,
				HiderBucket			&buck )
{
	u_int	sampsPerPixel	;
	float	ooSampsPerPixel ;

	if ( mParams.mDbgRasterizeVerts )
	{
		sampsPerPixel	= 1;
		ooSampsPerPixel = 1;
	}
	else
	{
		sampsPerPixel	= buck.mpSampCoordsBuff->GetSampsPerPixel();
		ooSampsPerPixel = 1.0f / sampsPerPixel;
	}

	u_int	buckWd = buck.GetWd();
	u_int	buckHe = buck.GetHe();

	size_t	pixIdx = 0;
	for (u_int y=0; y < buckHe; ++y)
	{
		for (u_int x=0; x < buckWd; ++x, ++pixIdx)
		{
			Float4	pixCol( 0.f );

			filterPixelBox( pixCol, pixels[pixIdx], sampsPerPixel, ooSampsPerPixel );

			buck.mCBuff.SetSample( x, y, &pixCol.x() );
		}
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
for (size_t i=0; i < DMT_SIMD_FLEN; ++i)
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
andCode &= ccodes[ srcVertIdx+0 ];
andCode &= ccodes[ srcVertIdx+1 ];
andCode &= ccodes[ srcVertIdx+xDim ];
andCode &= ccodes[ srcVertIdx+xDim+1 ];
// all out ? Skip this
if ( andCode )
	continue;
*/

//==================================================================
}
