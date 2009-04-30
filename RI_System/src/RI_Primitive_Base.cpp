/*
 *  RI_Primitive.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/04.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive.h"
#include "RI_HiderREYES.h"
#include "RI_FrameworkREYES.h"

//==================================================================
namespace RI
{

//==================================================================
void SimplePrimitiveBase::Split( HiderREYES &hider, bool uSplit, bool vSplit )
{
	if ( mSplitCnt > 10 )
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
void SimplePrimitiveBase::Dice(
					MicroPolygonGrid &g,
					const Matrix44 &mtxWorldCamera,
					bool doColorCoded ) const
{
	Point3	*pPointsWS = g.mpPointsWS;

	Vector3	*pI	 = (Vector3	*)g.mSymbols.LookupVariableData( "I", SlSymbol::VECTOR, true );
	Vector3	*pN  = (Vector3	*)g.mSymbols.LookupVariableData( "N", SlSymbol::NORMAL, true );
	Vector3	*pNg = (Vector3	*)g.mSymbols.LookupVariableData( "Ng", SlSymbol::NORMAL, true );
	Color	*pOs = (Color	*)g.mSymbols.LookupVariableData( "Os", SlSymbol::COLOR, true );
	Color	*pCs = (Color	*)g.mSymbols.LookupVariableData( "Cs", SlSymbol::COLOR, true );

	float	du = 1.0f / g.mXDim;
	float	dv = 1.0f / g.mYDim;

	Matrix44 mtxLocalCamera		= g.mMtxLocalWorld * mtxWorldCamera;

	Matrix44 mtxLocalCameraNorm = mtxLocalCamera.GetAs33();

	if ( mpAttribs->mOrientationFlipped )
		mtxLocalCameraNorm = mtxLocalCameraNorm * Matrix44::Scale( -1, -1, -1 );

	Vector3	camPosCS = mtxWorldCamera.GetTranslation();

	Color	useColor;

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
	}
	else
		useColor = mpAttribs->mColor;

	float	v = 0.0f;
	for (int i=0; i < (int)g.mYDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (int j=0; j < (int)g.mXDim; ++j, u += du)
		{
			Vector2	locUV = CalcLocalUV( Vector2( u, v ) );
			Vector3	dPdu;
			Vector3	dPdv;
			Vector3	posLS;
			Eval_dPdu_dPdv( locUV.x, locUV.y, posLS, &dPdu, &dPdv );

			Vector3 norLS = dPdu.GetCross( dPdv ).GetNormalized();

			//norLS = Vector3( 0, 0, 1 );

			Vector3	posWS = V3__V3W1_Mul_M44( posLS, g.mMtxLocalWorld );

			Vector3	posCS = V3__V3W1_Mul_M44( posLS, mtxLocalCamera );

			Vector3	norCS = V3__V3W1_Mul_M44( norLS, mtxLocalCameraNorm ).GetNormalized();
			//Vector3	norWS = V3__M44_Mul_V3W1( mtxLocalWorldNorm, norLS ).GetNormalized();

			*pPointsWS++	= posWS;
			//*pI++		= pos - camWorldPos;
			//*pI++		= (posCS - -camPosCS).GetNormalized();
			*pI++		= (posCS - -camPosCS).GetNormalized();
			*pN++		= norCS;
			*pNg++		= norCS;
			*pOs++		= mpAttribs->mOpacity;
			*pCs++		= useColor;

			//*pOs++ = Color( 1, 0, 0 );
			//*pCs++ = Color( 0, 1, 0 );
		}
	}
}

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					DVec<Vector3> &out_vectorP,
					int fromIdx )
{
	bool	gotP = false;

	for (size_t i=fromIdx; i < params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		CPSymVoid pyToken = staticSymbols.FindVoid( params[i] );
		if ( pyToken && pyToken->IsNameI( "P" ) )
		{
			DASSTHROW( (i+1) < params.size(), ("Invalid number of arguments") );
			
			const FltVec	&fltVec = params[ i+1 ].NumVec();
			
			DASSTHROW( (fltVec.size() % 3) == 0, ("Invalid number of arguments") );
			
			out_vectorP.resize( fltVec.size() / 3 );

			for (size_t iv=0, id=0; iv < fltVec.size(); iv += 3)
				out_vectorP[id++] = Vector3( &fltVec[ iv ] );

			return true;
		}
	}
	
	return false;
}

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					Vector3	*pOut_vectorP,
					int	expectedN,
					int fromIdx )
{
	bool	gotP = false;
	
	for (size_t i=fromIdx; i < params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );
		
		CPSymVoid pyToken = staticSymbols.FindVoid( params[i] );
		if ( pyToken && pyToken->IsNameI( "P" ) )
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
				pOut_vectorP[ di ] = Vector3( &fltVec[ si ] );
			
			return true;
		}
	}
	
	return false;
}

//==================================================================
bool SimplePrimitiveBase::SetupForDiceOrSplit(
							const HiderREYES &hider,
							bool &out_uSplit,
							bool &out_vSplit )
{
	const Matrix44 &mtxLocalWorld = mpTransform->GetMatrix();

	DASSERT( mDiceGridWd == -1 && mDiceGridHe == -1 );
	
	Bound	bound;
	MakeBound( bound );

	float pixelArea = hider.RasterEstimate( bound, mtxLocalWorld );
	
	if ( pixelArea <= MicroPolygonGrid::MAX_SIZE )
	{
		float	dim = sqrtf( pixelArea );

		mDiceGridWd = (int)ceilf( dim );
		mDiceGridHe = (int)ceilf( dim );

		out_uSplit = false;
		out_vSplit = false;

		return true;	// will dice
	}
	else
	{
		out_uSplit = true;
		out_vSplit = true;

		return false;	// will split
	}
}

/*
//==================================================================
bool SimplePrimitiveBase::IsDiceable(
						MicroPolygonGrid &g,
						HiderREYES *pHider,
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
		
		if ( pixelArea <= MicroPolygonGrid::MAX_SIZE )
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