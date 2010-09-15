//==================================================================
/// RI_Framework.cpp
///
/// Created by Davide Pasca - 2009/2/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Base.h"
#include "RI_State.h"
#include "RI_Framework.h"
#include <omp.h>

//==================================================================
namespace RI
{

//==================================================================
/// Framework
//==================================================================
Framework::Framework( const Params &params ) :
	mParams(params),
	mpGlobalSyms(NULL),
	mHider(*params.mpHiderParams)
{
}

//==================================================================
void Framework::WorldBegin(
						const Options &opt,
						const Matrix44 &mtxWorldCamera )
{
	mOptions = opt;
	mHider.WorldBegin( opt, mtxWorldCamera );
}

//==================================================================
void Framework::Insert(
						PrimitiveBase		*pPrim,
						const Attributes	&attr,
						const Transform		&xform )
{
	if ( mAttrsRev.Sync( *attr.mpRevision ) )
		mpUniqueAttribs.push_back( DNEW Attributes( attr ) );

	if ( mTransRev.Sync( *xform.mpRevision ) )
		mpUniqueTransform.push_back( DNEW Transform( xform ) );

	pPrim->SetStates(
				mpUniqueAttribs.back(),
				mpUniqueTransform.back()
				);

	//printf( "Prim xform rev %i\n",
	//		mpUniqueTransform.back()->mpRevision->mRTrackRevisionCount );

	mHider.Insert( pPrim );
}

//==================================================================
static void initAllocPixels(
				DVec<HiderPixel>			 	&pixels,
				DVec<DVec<HiderSampleData> >	&sampDataLists,
				HiderBucket						&bucket )
{
	size_t	buckPixelsN		= bucket.GetWd() * bucket.GetHe();

	pixels.resize( buckPixelsN );

	u_int	wd = bucket.GetWd();
	u_int	he = bucket.GetHe();

	HiderPixel			*pPixel = &pixels[0];
	HiderSampleCoords	*pSampCoords = bucket.mpSampCoordsBuff->mpSampCoords;
	u_int				sampsPerPix = bucket.mpSampCoordsBuff->GetSampsPerPixel();

	sampDataLists.resize( wd * he * sampsPerPix );

	size_t	sampIdx = 0;
	for (u_int y=0; y < he; ++y)
	{
		for (u_int x=0; x < wd; ++x, sampIdx += sampsPerPix, pPixel += 1)
		{
			pPixel->mX = 0;
			pPixel->mY = 0;
			pPixel->mpSampCoords = &pSampCoords[ sampIdx ];
			pPixel->mpSampDataLists	 = &sampDataLists[ sampIdx ];
		}
	}	
}

//==================================================================
void Framework::RenderBucket_s( Hider &hider, HiderBucket &bucket )
{
	DVec<SimplePrimitiveBase *>	&pPrimList = bucket.GetPrimList();

	bucket.BeginRender();

	WorkGrid	workGrid( *hider.mpGlobalSyms );

	DVec<ShadedGrid>	shadedGrids;

	DVec<HiderPixel>				pixels;
	DVec<DVec<HiderSampleData> >	sampDataLists;
	initAllocPixels( pixels, sampDataLists, bucket );

	size_t	primsN	= pPrimList.size();

	shadedGrids.resize( primsN );
	for (size_t i=0; i < primsN; ++i)
	{
		const SimplePrimitiveBase	*pPrim = (const SimplePrimitiveBase *)pPrimList[i];

		workGrid.Setup(
			pPrim->mDiceGridWd,
			pPrim->mDiceGridHe,
			pPrim->mURange,
			pPrim->mVRange,
			pPrim->mpTransform->GetMatrix(),
			hider.mMtxWorldCamera );

		pPrim->Dice( workGrid, hider.mParams.mDbgColorCodedGrids );

		// should check backface and trim
		workGrid.Displace( *pPrim->mpAttribs );

		workGrid.Shade( *pPrim->mpAttribs );

		shadedGrids[ i ].Init( workGrid.mPointsN );

		hider.Bust(
				bucket,
				shadedGrids[ i ],
				workGrid,
				pixels,
				hider.mFinalBuff.mWd,
				hider.mFinalBuff.mHe );
	}

	hider.Hide( pixels, bucket );

	bucket.EndRender( hider.mFinalBuff );
}

//==================================================================
void Framework::worldEnd_simplify()
{
	DUT::QuickProf	prof( __FUNCTION__ );

	// --- convert complex primitives into simple ones
	for (size_t i=0; i < mHider.mpPrims.size(); ++i)
	{
		PrimitiveBase	*pPrim = mHider.mpPrims[i];

		if ( pPrim->IsComplex() )
		{
			((ComplexPrimitiveBase *)pPrim)->Simplify( mHider );
			pPrim->Release();
			mHider.mpPrims[i] = NULL;
			// could compact mHider.mpPrims as it goes..
		}
	}
}

//==================================================================
void Framework::worldEnd_splitAndAddToBuckets()
{
	DUT::QuickProf	prof( __FUNCTION__ );

	// --- split primitives and assign to buckets for dicing
	for (size_t i=0; i < mHider.mpPrims.size(); ++i)
	{
		if NOT( mHider.mpPrims[i] )
			continue;

		SimplePrimitiveBase	*pPrim = (SimplePrimitiveBase *)mHider.mpPrims[i];

		int		bound2d[4];
		bool	uSplit;
		bool	vSplit;
		SimplePrimitiveBase::CheckSplitRes	dosRes =
								pPrim->CheckForSplit(
											mHider,
											bound2d,
											uSplit,
											vSplit );

		if ( dosRes == SimplePrimitiveBase::CHECKSPLITRES_DICE )
		{
			mHider.InsertForDicing( (SimplePrimitiveBase *)pPrim, bound2d );
		}
		else
		if ( dosRes == SimplePrimitiveBase::CHECKSPLITRES_SPLIT )
		{
			pPrim->Split( mHider, uSplit, vSplit );
		}
		// otherwise it's cull..

		pPrim->Release();
		mHider.mpPrims[i] = NULL;
	}
	mHider.mpPrims.clear();
}

//==================================================================
void Framework::worldEnd_setupDisplays()
{
	//--- setup the displays by assigning or creating display drivers
	for (size_t i=0; i < mOptions.mpDisplays.size(); ++i)
	{
		Options::Display	&disp = *mOptions.mpDisplays[i];

		const char *pDispMode = disp.mMode.c_str();

		U32			sampPerPix = (U32)strlen( pDispMode );

		DIMG::Image::SampType	sampType;

		if ( disp.mValMax == 0 )
		{	// float
			sampType = DIMG::Image::ST_F32;
		}
		else
		if ( disp.mValMax <= 255 )
		{	// good old 8-bit
			sampType = DIMG::Image::ST_U8;
		}
		else
		if ( disp.mValMax <= 65535 )
		{	// 16 bit
			sampType = DIMG::Image::ST_U16;
		}
		else
		{
			DASSTHROW( 0,
				("Invalid quantization value for the display '%s'", disp.mName.c_str()) );
		}

		disp.mImage.Init(	(U32)mOptions.mXRes,
							(U32)mOptions.mYRes,
							sampPerPix,
							sampType,
							-1,
							pDispMode );

	}
}

//==================================================================
/// RenderBucketsStd
//==================================================================
class RenderBucketsStd : public RenderBucketsBase
{
public:
	void Render( Hider &hider )
	{
		DUT::QuickProf	prof( __FUNCTION__ );

		const DVec<HiderBucket *>	buckets = hider.GetBuckets();
	
		int	bucketsN = (int)buckets.size();

		// --- dice primitives accumulated in the buckets
		#pragma omp parallel for
		for (int bi=0; bi < bucketsN; ++bi)
		{
			Framework::RenderBucket_s( hider, *buckets[ bi ] );
		}
	}
};

//==================================================================
void Framework::WorldEnd()
{
#if 0
	Point3	camPosWS =
		MultiplyV3M(
			-mHider.mMtxWorldCamera.GetTranslation(),
				mHider.mMtxWorldCamera.GetAs33().GetInverse()
		);
#else

	Point3	camPosWS = mHider.mMtxWorldCamera.GetTranslation();

#endif

	worldEnd_simplify();

	worldEnd_splitAndAddToBuckets();

	try {

		worldEnd_setupDisplays();

		// render the buckets..
		if ( mParams.mpRenderBuckets )
		{
			mParams.mpRenderBuckets->Render( mHider );
		}
		else
		{
			RenderBucketsStd	rendBuck;

			rendBuck.Render( mHider );
		}

		// --- release the primitives in all the buckets
		for (size_t bi=0; bi < mHider.mpBuckets.size(); ++bi)
		{
			DVec<SimplePrimitiveBase *>	&pPrimList = mHider.mpBuckets[ bi ]->GetPrimList();
			for (size_t i=0; i < pPrimList.size(); ++i)
			{
				if ( pPrimList[i] )
				{
					pPrimList[i]->Release();
					pPrimList[i] = NULL;
				}
			}
		}

		for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	DDELETE( mpUniqueAttribs[i] );
		for (size_t i=0; i < mpUniqueTransform.size(); ++i)	DDELETE( mpUniqueTransform[i] );
		mpUniqueAttribs.clear();
		mpUniqueTransform.clear();

		DIMG::Image	hiderImg(
					mHider.GetOutputDataWd(),
					mHider.GetOutputDataHe(),
					NOUTCOLS,
					DIMG::Image::ST_F32,
					mHider.GetOutputDataStride() * sizeof(float),
					"rgba",	// depends on NOUTCOLS too !
					(const U8 *)mHider.GetOutputData(0,0) );

		for (size_t i=0; i < mOptions.mpDisplays.size(); ++i)
		{
			Options::Display	&disp = *mOptions.mpDisplays[i];

			DIMG::ConvertImages( disp.mImage, hiderImg );
		}

		mHider.WorldEnd();
	}
	catch ( ... )
	{
		// free the displays anyway !
		mOptions.FreeDisplays();
		throw;
	}
}

//==================================================================
#if defined(DEBUG) || defined(_DEBUG)
void Framework::Dbg_MarkLastPrim( const char *pSrcFileName, int srcLine )
{
	mHider.mpPrims.back()->mDbg_SrcLine = srcLine;

	for (size_t i=0; i < mDbg_SrcFileNames.size(); ++i)
	{
		const char *pStr = mDbg_SrcFileNames[i].c_str();
		if ( 0 == strcasecmp( pStr, pSrcFileName ) )
		{
			mHider.mpPrims.back()->mpDbg_SrcArchive = pStr;
			return;
		}
	}

	mDbg_SrcFileNames.push_back( pSrcFileName );
	mHider.mpPrims.back()->mpDbg_SrcArchive = mDbg_SrcFileNames.back().c_str();
}
#endif



//==================================================================
}
