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
									SlVec2 locUV[],
									SlVec2 locDUDV[],
									float du,
									float dv,
									u_int xDim,
									u_int yDim ) const
{
	size_t	sampleIdx = 0;

	Vec2f	prevUV( 0, 0 );

	float	v = 0.0f;
	for (u_int i=0; i < yDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (u_int j=0; j < xDim; ++j, u += du, ++sampleIdx)
		{
			Vec2f	tmpUV = CalcLocalUV( Vec2f( u, v ) );

			size_t	blk = sampleIdx / RI_SIMD_BLK_LEN;
			size_t	sub = sampleIdx & (RI_SIMD_BLK_LEN-1);

			SlVec2	&blkLocUV = locUV[ blk ];
			SlVec2	&blkLocDUDV = locDUDV[ blk ];

			blkLocUV[0][ sub ] = tmpUV[0];
			blkLocUV[1][ sub ] = tmpUV[1];

			if ( j > 0 )
			{
				blkLocDUDV[0][ sub ] = tmpUV[0] - prevUV[0];
			}

			prevUV = tmpUV;
		}
	}

	u_int xBlkN = RI_GET_SIMD_BLOCKS( xDim );

	// -- calc du edge (replicates col 1 into col 0..)
	u_int	blk = 0;
	for (u_int i=0; i < yDim; ++i, blk += xBlkN)
	{
		SlVec2	&blkLocDUDV = locDUDV[ blk ];

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
	//SlVec3	*pPointsWS = g.mpPointsWS;

	SlVec3	 *pPointsWS = (SlVec3	*)g.mSymbolIs.FindSymbolIData( "P"	);
	SlScalar *pOODu		= (SlScalar *)g.mSymbolIs.FindSymbolIData( "_oodu" );
	SlScalar *pOODv		= (SlScalar *)g.mSymbolIs.FindSymbolIData( "_oodv" );
	SlVec3	 *pI		= (SlVec3	*)g.mSymbolIs.FindSymbolIData( "I"	);
	SlVec3	 *pN		= (SlVec3	*)g.mSymbolIs.FindSymbolIData( "N"	);
	SlVec3	 *pNg		= (SlVec3	*)g.mSymbolIs.FindSymbolIData( "Ng"	);
	SlColor	 *pOs		= (SlColor	*)g.mSymbolIs.FindSymbolIData( "Os"	);
	SlColor	 *pCs		= (SlColor	*)g.mSymbolIs.FindSymbolIData( "Cs"	);

	DASSERT( pPointsWS == g.mpPointsWS );

	float	du = 1.0f / (g.mXDim-1);
	float	dv = 1.0f / (g.mYDim-1);

	Matrix44 mtxLocalCameraNorm = g.mMtxLocalCamera.GetAs33();

	if ( mpAttribs->mOrientationFlipped )
		mtxLocalCameraNorm = mtxLocalCameraNorm * Matrix44::Scale( -1, -1, -1 );

	Vec3f	camPosCS = g.mMtxWorldCamera.GetTranslation();

	SlColor	useColor;
	SlColor	useOpa;

	if ( doColorCoded )
	{
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
		static int cnt;

		useColor = palette[ cnt++ & 7 ];
		useOpa	 = SlColor( 1, 1, 1 );
	}
	else
	{
		useColor = mpAttribs->mColor;
		useOpa	 = mpAttribs->mOpacity;
	}

	// build the UVs
	SlVec2	locUV[ MP_GRID_MAX_SIMD_BLKS ];
	SlVec2	locDUDV[ MP_GRID_MAX_SIMD_BLKS ];

	fillUVsArray( locUV, locDUDV, du, dv, g.mXDim, g.mYDim );

	//DASSERT( sampleIdx == g.mPointsN );

	size_t	blocksN = RI_GET_SIMD_BLOCKS( g.mPointsN );

	SlScalar	one( 1.0f );

	for (size_t blkIdx=0; blkIdx < blocksN; ++blkIdx)
	{
#if 1
		SlVec3	dPdu;
		SlVec3	dPdv;
		SlVec3	posLS;

		Eval_dPdu_dPdv( locUV[blkIdx], posLS, &dPdu, &dPdv );

		if ( posLS[0][0] == 0 && posLS[0][1] == 0 && posLS[0][2] == 0 )
			continue;

		SlVec3	norLS = dPdu.GetCross( dPdv ).GetNormalized();
		SlVec3	posWS = V3__V3W1_Mul_M44<SlScalar>( posLS, g.mMtxLocalWorld );
		SlVec3	posCS = V3__V3W1_Mul_M44<SlScalar>( posLS, g.mMtxLocalCamera );
		SlVec3	norCS = V3__V3W1_Mul_M44<SlScalar>( norLS, mtxLocalCameraNorm ).GetNormalized();

		pPointsWS[blkIdx]	= posWS;

		pI[blkIdx]		= (posCS - -camPosCS);//.GetNormalized();
		pOODu[blkIdx]	= one / locDUDV[blkIdx][0];
		pOODv[blkIdx]	= one / locDUDV[blkIdx][1];
		pN[blkIdx]		= norCS;
		pNg[blkIdx]		= norCS;
		pOs[blkIdx]		= useOpa;
		pCs[blkIdx]		= useColor;

#else
		SlVec2	&tmpvblk = locUV[ blkIdx ];

		for (size_t	itmIdx=0; itmIdx < RI_SIMD_BLK_LEN; ++itmIdx)
		{
			Vec3f	dPdu;
			Vec3f	dPdv;
			Vec3f	posLS;
			Eval_dPdu_dPdv(	locUV[ blkIdx ][0][ itmIdx ],
							locUV[ blkIdx ][1][ itmIdx ],
							posLS,
							&dPdu,
							&dPdv );

			Vec3f	norLS = dPdu.GetCross( dPdv ).GetNormalized();
			Vec3f	posWS = V3__V3W1_Mul_M44<float>( posLS, g.mMtxLocalWorld );
			Vec3f	posCS = V3__V3W1_Mul_M44<float>( posLS, g.mMtxLocalCamera );
			Vec3f	norCS = V3__V3W1_Mul_M44<float>( norLS, mtxLocalCameraNorm ).GetNormalized();

			// store in blocked SOA format

			pPointsWS[blkIdx][0][itmIdx] = posWS[0];
			pPointsWS[blkIdx][1][itmIdx] = posWS[1];
			pPointsWS[blkIdx][2][itmIdx] = posWS[2];

			Vec3f	tmpI = (posCS - -camPosCS).GetNormalized();
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
					DVec<Vec3f> &out_vectorP,
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
				out_vectorP[id++] = Vec3f( &fltVec[ iv ] );

			return true;
		}
	}
	
	return false;
}

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &globalSymbols,
					Vec3f	*pOut_vectorP,
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
				pOut_vectorP[ di ] = Vec3f( &fltVec[ si ] );
			
			return true;
		}
	}
	
	return false;
}

//==================================================================
SimplePrimitiveBase::DosRes
	SimplePrimitiveBase::SetupForDiceOrSplit(
							const Hider &hider,
							bool &out_uSplit,
							bool &out_vSplit )
{
	const Matrix44 &mtxLocalWorld = mpTransform->GetMatrix();

	DASSERT( mDiceGridWd == -1 && mDiceGridHe == -1 );
	
	Bound	bound;
	MakeBound( bound );

	float pixelArea = hider.RasterEstimate( bound, mtxLocalWorld );
	
	if ( pixelArea <= MP_GRID_MAX_SIZE )
	{
		if ( pixelArea < RI_EPSILON )
		{
			return DOSRES_CULL;
		}

		float	dim = DSqrt( pixelArea );

		mDiceGridWd = RI_GET_SIMD_PAD_SUBS( (int)ceilf( dim ) );
		mDiceGridHe = (int)ceilf( dim );

		out_uSplit = false;
		out_vSplit = false;

		return DOSRES_DICE;	// will dice
	}
	else
	{
		out_uSplit = true;
		out_vSplit = true;

		return DOSRES_SPLIT;	// will split
	}
}

/*
//==================================================================
bool SimplePrimitiveBase::IsDiceable(
						WorkGrid &g,
						Hider *pHider,
						bool &out_uSplit,
						bool &out_vSplit )
{
	const Matrix44 &mtxLocalWorld = mpTransform->GetMatrix();
	
	Bound	bound;
	if ( MakeBound( bound ) )
	{
		out_uSplit = false;
		out_vSplit = false;

		float pixelArea = pHider->RasterEstimate( bound, mtxLocalWorld );
		
		if ( pixelArea <= WorkGrid::MP_GRID_MAX_SIZE )
		{
			float	dim = sqrtf( pixelArea );
			if ( dim > 0 )
			{
				int	dimX = (int)ceilf( dim );
				int	dimY = (int)ceilf( dim );

				g.Setup(dimX,
						dimY,
						mURange,
						mVRange,
						mtxLocalWorld
						);

				return true;
			}
			else
				return false;
		}
	}

	out_uSplit = true;
	out_vSplit = true;
	return false;
}
*/

//==================================================================
}