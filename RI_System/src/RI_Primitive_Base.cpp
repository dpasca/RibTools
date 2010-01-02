//==================================================================
/// RI_Primitive_Base.cpp
///
/// Created by Davide Pasca - 2009/01/04
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive.h"
#include "RI_HiderST.h"

//==================================================================
static const u_int	TEST_DICE_LEN = 4;
static const u_int	TEST_DICE_SIMD_BLOCKS = DMT_SIMD_BLOCKS( TEST_DICE_LEN * TEST_DICE_LEN );

//==================================================================
namespace RI
{

//==================================================================
void SimplePrimitiveBase::Split( Hider &hider, bool uSplit, bool vSplit )
{
	DASSERT( IsUsed() );

	if ( mSplitCnt > 9 )
	{
		// $$$ too many splits !!!
		mSplitCnt = mSplitCnt;
		return;
	}

	if ( uSplit )
	{
		// U split
		SimplePrimitiveBase *pPrimsSU[2] =
		{
			Clone(),
			Clone()
		};
		float	uMid = (mURange[0] + mURange[1]) * 0.5f;
		pPrimsSU[0]->mURange[1] = uMid;
		pPrimsSU[1]->mURange[0] = uMid;
		hider.InsertSplitted( pPrimsSU[0], *this );
		hider.InsertSplitted( pPrimsSU[1], *this );

		if ( vSplit )
		{
			// optional V split
			float	vMid = (mVRange[0] + mVRange[1]) * 0.5f;

			// check.. because we can't be sure that the primitive didn't fail
			// at insertion time
			for (size_t i=0; i < 2; ++i)
			{
				if ( pPrimsSU[i]->IsUsed() )
				{
					SimplePrimitiveBase *pNewPrim = pPrimsSU[i]->Clone();
					pPrimsSU[i]->mVRange[1] = vMid;
					pNewPrim->mVRange[0] = vMid;
					hider.InsertSplitted( pNewPrim, *pPrimsSU[i] );
				}
			}
		}
	}
	else
	{
		// exclusive V split
		if ( vSplit )
		{
			SimplePrimitiveBase *pPrim1 = Clone();
			SimplePrimitiveBase *pPrim2 = Clone();
			
			float	vMid = (mVRange[0] + mVRange[1]) * 0.5f;
			pPrim1->mVRange[1] = vMid;
			pPrim2->mVRange[0] = vMid;

			hider.InsertSplitted( pPrim1, *this );
			hider.InsertSplitted( pPrim2, *this );
		}
	}
}

//==================================================================
void SimplePrimitiveBase::fillUVsArray(
									Float2_ out_locUV[],
									float du,
									float dv,
									u_int xDim,
									u_int yDim ) const
{
	size_t	sampleIdx = 0;

	float	v = 0.0f;
	for (u_int i=0; i < yDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (u_int j=0; j < xDim; ++j, u += du, ++sampleIdx)
		{
			Float2	tmpUV = CalcLocalUV( u, v );

			size_t	blk = sampleIdx / DMT_SIMD_FLEN;
			size_t	sub = sampleIdx & (DMT_SIMD_FLEN-1);

			Float2_	&blkLocUV = out_locUV[ blk ];

			blkLocUV[0][ sub ] = tmpUV[0];
			blkLocUV[1][ sub ] = tmpUV[1];
		}
	}
}

//==================================================================
void SimplePrimitiveBase::fillUVsArray(
									Float2_ locUV[],
									Float2_ locDUDV[],
									float du,
									float dv,
									u_int xDim,
									u_int yDim ) const
{
	size_t	sampleIdx = 0;

	Float2	prevUV( 0, 0 );

	float	v = 0.0f;
	for (u_int i=0; i < yDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (u_int j=0; j < xDim; ++j, u += du, ++sampleIdx)
		{
			Float2	tmpUV = CalcLocalUV( u, v );

			size_t	blk = sampleIdx / DMT_SIMD_FLEN;
			size_t	sub = sampleIdx & (DMT_SIMD_FLEN-1);

			Float2_	&blkLocUV = locUV[ blk ];
			Float2_	&blkLocDUDV = locDUDV[ blk ];

			blkLocUV[0][ sub ] = tmpUV[0];
			blkLocUV[1][ sub ] = tmpUV[1];

			if ( j > 0 )
			{
				blkLocDUDV[0][ sub ] = tmpUV[0] - prevUV[0];
			}

			prevUV = tmpUV;
		}
	}

	u_int xBlkN = DMT_SIMD_BLOCKS( xDim );

	// -- calc du edge (replicates col 1 into col 0..)
	u_int	blk = 0;
	for (u_int i=0; i < yDim; ++i, blk += xBlkN)
	{
		Float2_	&blkLocDUDV = locDUDV[ blk ];

		float	locDU = CalcLocalU( du ) - CalcLocalU( 0 );

		blkLocDUDV[0][0] = locDU;
	}

	// -- calc dv
	size_t	blkIdx = xBlkN;
	for (u_int i=1; i < yDim; ++i)
		for (u_int j=0; j < xBlkN; ++j, ++blkIdx)
			locDUDV[ blkIdx ][1] = locUV[blkIdx][1] - locUV[blkIdx - xBlkN][1];

	// row 0 dv = row 1 dv (coarse approximation !)
	for (u_int i=0; i < xBlkN; ++i)
		locDUDV[i][1] = locDUDV[i + xBlkN][1];
}

//==================================================================
void SimplePrimitiveBase::Dice(
					WorkGrid &g,
					bool doColorCoded ) const
{
	//Float3_	*pPointsWS = g.mpPointsWS;

	Float3_	 *pPointsWS = (Float3_	*)g.mSymbolIs.FindSymbolIData( "P"	);
	Float_ *pOODu		= (Float_ *)g.mSymbolIs.FindSymbolIData( "_oodu" );
	Float_ *pOODv		= (Float_ *)g.mSymbolIs.FindSymbolIData( "_oodv" );
	Float3_	 *pI		= (Float3_	*)g.mSymbolIs.FindSymbolIData( "I"	);
	Float3_	 *pN		= (Float3_	*)g.mSymbolIs.FindSymbolIData( "N"	);
	Float3_	 *pNg		= (Float3_	*)g.mSymbolIs.FindSymbolIData( "Ng"	);
	SlColor	 *pOs		= (SlColor	*)g.mSymbolIs.FindSymbolIData( "Os"	);
	SlColor	 *pCs		= (SlColor	*)g.mSymbolIs.FindSymbolIData( "Cs"	);

	DASSERT( pPointsWS == g.mpPointsWS );

	float	du = 1.0f / (g.mXDim-1);
	float	dv = 1.0f / (g.mYDim-1);

	Matrix44 mtxLocalCameraNorm = g.mMtxLocalCamera.GetAs33();

	if ( mpAttribs->mOrientationFlipped )
		mtxLocalCameraNorm = mtxLocalCameraNorm * Matrix44::Scale( -1, -1, -1 );

	Float3	camPosCS = g.mMtxWorldCamera.GetTranslation();

	SlColor	useColor;
	SlColor	useOpa;

	if ( doColorCoded )
	{
		const static float c0 = 0.0f;
		const static float c1 = 0.33f;
		const static float c2 = 0.66f;
		const static float c3 = 1.0f;
		static Color palette[16] =
		{
			Color( c0, c0, 1-c0 ),
			Color( c1, c0, 1-c1 ),
			Color( c2, c0, 1-c2 ),
			Color( c3, c0, 1-c3 ),
			Color( c0, c0, 1-c0 ),
			Color( c1, c0, 1-c1 ),
			Color( c2, c0, 1-c2 ),
			Color( c3, c0, 1-c3 ),

			Color( c0, c1, 1-c0 ),
			Color( c1, c1, 1-c1 ),
			Color( c2, c2, 1-c2 ),
			Color( c3, c2, 1-c3 ),
			Color( c0, c3, 1-c0 ),
			Color( c1, c3, 1-c1 ),
			Color( c2, c3, 1-c2 ),
			Color( c3, c3, 1-c3 ),
		};
/*
		const static float l = 0.3f;
		const static float m = 0.6f;
		const static float h = 1.0f;
		static Color palette[8] =
		{
			Color( l, l, l ),
			Color( l, l, h ),
			Color( l, h, l ),
			Color( l, h, h ),
			Color( h, l, l ),
			Color( h, l, h ),
			Color( h, h, l ),
			Color( h, h, h ),
		};
*/
		static int cnt;

		useColor = palette[ cnt++ & 15 ];
		useOpa	 = SlColor( 1, 1, 1 );
	}
	else
	{
		useColor = mpAttribs->mColor;
		useOpa	 = mpAttribs->mOpacity;
	}

	// build the UVs
	Float2_	locUV[ MP_GRID_MAX_SIMD_BLKS ];
	Float2_	locDUDV[ MP_GRID_MAX_SIMD_BLKS ];

	fillUVsArray( locUV, locDUDV, du, dv, g.mXDim, g.mYDim );

	//DASSERT( sampleIdx == g.mPointsN );

	size_t	blocksN = DMT_SIMD_BLOCKS( g.mPointsN );

	Float_	one( 1.0f );

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
#if 1
		Float3_	dPdu;
		Float3_	dPdv;
		Float3_	posLS;

		Eval_dPdu_dPdv( locUV[blkIdx], posLS, &dPdu, &dPdv );

		if ( posLS[0][0] == 0 && posLS[0][1] == 0 && posLS[0][2] == 0 )
			continue;

		Float3_	norLS = dPdu.GetCross( dPdv ).GetNormalized();
		Float3_	posWS = V3__V3W1_Mul_M44<Float_>( posLS, g.mMtxLocalWorld );
		Float3_	posCS = V3__V3W1_Mul_M44<Float_>( posLS, g.mMtxLocalCamera );
		Float3_	norCS = V3__V3W1_Mul_M44<Float_>( norLS, mtxLocalCameraNorm ).GetNormalized();

		pPointsWS[blkIdx]	= posWS;

		pI[blkIdx]		= (posCS - -camPosCS);//.GetNormalized();
		pOODu[blkIdx]	= one / locDUDV[blkIdx][0];
		pOODv[blkIdx]	= one / locDUDV[blkIdx][1];
		pN[blkIdx]		= norCS;
		pNg[blkIdx]		= norCS;
		pOs[blkIdx]		= useOpa;
		pCs[blkIdx]		= useColor;

#else
		Float2_	&tmpvblk = locUV[ blkIdx ];

		for (size_t	itmIdx=0; itmIdx < DMT_SIMD_FLEN; ++itmIdx)
		{
			Float3	dPdu;
			Float3	dPdv;
			Float3	posLS;
			Eval_dPdu_dPdv(	locUV[ blkIdx ][0][ itmIdx ],
							locUV[ blkIdx ][1][ itmIdx ],
							posLS,
							&dPdu,
							&dPdv );

			Float3	norLS = dPdu.GetCross( dPdv ).GetNormalized();
			Float3	posWS = V3__V3W1_Mul_M44<float>( posLS, g.mMtxLocalWorld );
			Float3	posCS = V3__V3W1_Mul_M44<float>( posLS, g.mMtxLocalCamera );
			Float3	norCS = V3__V3W1_Mul_M44<float>( norLS, mtxLocalCameraNorm ).GetNormalized();

			// store in blocked SOA format

			pPointsWS[blkIdx][0][itmIdx] = posWS[0];
			pPointsWS[blkIdx][1][itmIdx] = posWS[1];
			pPointsWS[blkIdx][2][itmIdx] = posWS[2];

			Float3	tmpI = (posCS - -camPosCS).GetNormalized();
			pI[blkIdx][0][itmIdx] = tmpI[0];
			pI[blkIdx][1][itmIdx] = tmpI[1];
			pI[blkIdx][2][itmIdx] = tmpI[2];

			pN[blkIdx][0][itmIdx] = norCS[0];
			pN[blkIdx][1][itmIdx] = norCS[1];
			pN[blkIdx][2][itmIdx] = norCS[2];

			pNg[blkIdx][0][itmIdx] = norCS[0];
			pNg[blkIdx][1][itmIdx] = norCS[1];
			pNg[blkIdx][2][itmIdx] = norCS[2];
		}

		pOs[blkIdx]	= useOpa;
		pCs[blkIdx]	= useColor;
#endif
	}
}

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &globalSymbols,
					DVec<Float3> &out_vectorP,
					int fromIdx )
{
	bool	gotP = false;

	for (size_t i=fromIdx; i < params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		const Symbol* pSymbol = globalSymbols.FindSymbol( params[i] );
		if ( pSymbol && pSymbol->IsName( "P" ) )
		{
			DASSTHROW( (i+1) < params.size(), ("Invalid number of arguments") );
			
			const FltVec	&fltVec = params[ i+1 ].NumVec();
			
			DASSTHROW( (fltVec.size() % 3) == 0, ("Invalid number of arguments") );
			
			out_vectorP.resize( fltVec.size() / 3 );

			for (size_t iv=0, id=0; iv < fltVec.size(); iv += 3)
				out_vectorP[id++] = Float3( &fltVec[ iv ] );

			return true;
		}
	}
	
	return false;
}

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &globalSymbols,
					Float3	*pOut_vectorP,
					int	expectedN,
					int fromIdx )
{
	bool	gotP = false;
	
	for (size_t i=fromIdx; i < params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );
		
		const Symbol* pSymbol = globalSymbols.FindSymbol( params[i] );
		if ( pSymbol && pSymbol->IsName( "P" ) )
		{
			DASSTHROW( (i+1) < params.size(), ("Invalid number of arguments") );
			
			const FltVec	&fltVec = params[ i+1 ].NumVec();

			DASSTHROW( (int)fltVec.size() == 3 * expectedN,
							("Invalid number of arguments."
							 " Expecting %i but it's %u", 3 * expectedN, fltVec.size() ) );

			DASSTHROW( (int)(fltVec.size() % 3) == 0,
							("Invalid number of arguments."
							 " Should be multiple of 3 but it's %u", fltVec.size()) );

			size_t	srcN = fltVec.size();

			for (size_t si=0, di=0; si < srcN; si += 3, di += 1)
				pOut_vectorP[ di ] = Float3( &fltVec[ si ] );
			
			return true;
		}
	}
	
	return false;
}

//==================================================================
SimplePrimitiveBase::CheckSplitRes
	SimplePrimitiveBase::CheckForSplit(
							const Hider &hider,
							int			out_bound2d[4],
							bool		&out_uSplit,
							bool		&out_vSplit )
{
	const Matrix44 &mtxLocalWorld = mpTransform->GetMatrix();

	DASSERT( mDiceGridWd == -1 && mDiceGridHe == -1 );

	Float3_	testDicePo[ MAX_MAKE_BOUND_OUT_SIZE ];

	Bound	bound;
	bound.Reset();

	MakeBound( bound, testDicePo );

	float pixelArea = hider.RasterEstimate( bound, mtxLocalWorld, out_bound2d );
	
	if ( pixelArea <= MP_GRID_MAX_SIZE )
	{
		if ( pixelArea < RI_EPSILON )
		{
			return CHECKSPLITRES_CULL;
		}

		float	dim = DSqrt( pixelArea );

		mDiceGridWd = DMT_SIMD_PADSIZE( (int)ceilf( dim ) );
		mDiceGridHe = (int)ceilf( dim );

		out_uSplit = false;
		out_vSplit = false;

		return CHECKSPLITRES_DICE;	// will dice
	}
	else
	{
#if 0
		SlVec2	locUV[ TEST_DICE_SIMD_BLOCKS ];
		// set last item to 0 to be safe when using the AddReduce() later on
		locUV[ TEST_DICE_SIMD_BLOCKS - 1 ] = SlVec2( 0.f, 0.f );

		fillUVsArray(
				locUV,
				1.0f / (TEST_DICE_LEN - 1),
				1.0f / (TEST_DICE_LEN - 1),
				TEST_DICE_LEN,
				TEST_DICE_LEN );

		//Float3_	avg_dPdu( 0.f, 0.f, 0.f );
		//Float3_	avg_dPdv( 0.f, 0.f, 0.f );
		SlScalar	avg_dPdu( 0.f );
		SlScalar	avg_dPdv( 0.f );
		for (u_int i=0; i < TEST_DICE_SIMD_BLOCKS; ++i)
		{
			SlVec3	posLS;
			SlVec3	dPdu;
			SlVec3	dPdv;

			Eval_dPdu_dPdv( locUV[ i ], posLS, &dPdu, &dPdv );

			avg_dPdu += dPdu.GetLengthSqr();
			avg_dPdv += dPdv.GetLengthSqr();
		}

		// divide by the total number of elements 
		//avg_dPdu /= (float)TEST_DICE_SIMD_BLOCKS * DMT_SIMD_FLEN;
		//avg_dPdv /= (float)TEST_DICE_SIMD_BLOCKS * DMT_SIMD_FLEN;
/*

		Float3 avg_dPdu_s(
				avg_dPdu.x().AddReduce(),	// avg_dPdu.x()[0] + avg_dPdu.x()[1] ...
				avg_dPdu.y().AddReduce(),
				avg_dPdu.z().AddReduce() );

		Float3 avg_dPdv_s(
				avg_dPdv.x().AddReduce(),	// avg_dPdv.x()[0] + avg_dPdv.x()[1] ...
				avg_dPdv.y().AddReduce(),
				avg_dPdv.z().AddReduce() );
*/

		float lenU = avg_dPdu.AddReduce();
		float lenV = avg_dPdv.AddReduce();

		if ( lenU < lenV )
		{
			out_uSplit = false;
			out_vSplit = true;
		}
		else
		{
			out_uSplit = true;
			out_vSplit = false;
		}

#else
		out_uSplit = true;
		out_vSplit = true;

#endif

		return CHECKSPLITRES_SPLIT;	// will split
	}
}

//==================================================================
}