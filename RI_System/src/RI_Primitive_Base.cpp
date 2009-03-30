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
#include "RI_HiderBase.h"
#include "RI_FrameworkBase.h"

//==================================================================
namespace RI
{

//==================================================================
void Primitive::Split( FrameworkBase &fwork, bool uSplit, bool vSplit )
{
	if ( mSplitCnt > 8 )
	{
		// $$$ too many splits !!!
		mSplitCnt = mSplitCnt;
		return;
	}

	if ( uSplit )
	{
		// U split
		Primitive *pPrimsSU[2] =
		{
			Clone(),
			Clone()
		};
		float	uMid = (mURange[0] + mURange[1]) * 0.5f;
		pPrimsSU[0]->mURange[1] = uMid;
		pPrimsSU[1]->mURange[0] = uMid;
		fwork.InsertSplitted( pPrimsSU[0], *this );
		fwork.InsertSplitted( pPrimsSU[1], *this );
		this->MarkUnusable();

		if ( vSplit )
		{
			// optional V split
			float	vMid = (mVRange[0] + mVRange[1]) * 0.5f;

			Primitive *pPrimLB = pPrimsSU[0]->Clone();
			Primitive *pPrimRB = pPrimsSU[1]->Clone();
			fwork.InsertSplitted( pPrimLB, *pPrimsSU[0] );
			fwork.InsertSplitted( pPrimRB, *pPrimsSU[1] );

			pPrimsSU[0]->mVRange[1] = vMid;
			pPrimsSU[1]->mVRange[1] = vMid;
			pPrimLB->mVRange[0] = vMid;
			pPrimRB->mVRange[0] = vMid;
		}
	}
	else
	{
		// exclusive V split
		if ( vSplit )
		{
			Primitive *pPrim1 = Clone();
			Primitive *pPrim2 = Clone();
			
			float	vMid = (mVRange[0] + mVRange[1]) * 0.5f;
			pPrim1->mVRange[1] = vMid;
			pPrim2->mVRange[0] = vMid;

			fwork.InsertSplitted( pPrim1, *this );
			fwork.InsertSplitted( pPrim2, *this );
			this->MarkUnusable();
		}
	}
}

//==================================================================
void Primitive::Dice( MicroPolygonGrid &g, const Point3 &camWorldPos )
{
	Point3	*pPoints = g.mpPoints;

	Vector3	*pI	 = (Color *)g.mSymbols.LookupVariableData( "I", SlSymbol::VECTOR, true );
	Vector3	*pN  = (Color *)g.mSymbols.LookupVariableData( "N", SlSymbol::NORMAL, true );
	Vector3	*pNg = (Color *)g.mSymbols.LookupVariableData( "Ng", SlSymbol::NORMAL, true );
	Color	*pOs = (Color *)g.mSymbols.LookupVariableData( "Os", SlSymbol::COLOR, true );
	Color	*pCs = (Color *)g.mSymbols.LookupVariableData( "Cs", SlSymbol::COLOR, true );

	float	du = 1.0f / g.mXDim;
	float	dv = 1.0f / g.mYDim;
	
	Matrix44 mtxObjectCurrentNorm = g.mMtxObjectCurrent;
	mtxObjectCurrentNorm.u.m44[3][0] = 0;
	mtxObjectCurrentNorm.u.m44[3][1] = 0;
	mtxObjectCurrentNorm.u.m44[3][2] = 0;
	mtxObjectCurrentNorm.u.m44[3][3] = 1;

	float	v = 0.0f;
	for (int i=0; i < (int)g.mYDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (int j=0; j < (int)g.mXDim; ++j, u += du)
		{
			Vector3	pos;
			Vector3	dPdu;
			Vector3	dPdv;
			Vector2	locUV = CalcLocalUV( Vector2( u, v ) );
			Eval_dPdu_dPdv( locUV.x, locUV.y, pos, &dPdu, &dPdv );
			pos = MultiplyV3M( pos, g.mMtxObjectCurrent );
			
			Vector3 objNor = dPdu.GetCross( dPdv );
			Vector3	nor = MultiplyV3M( objNor, mtxObjectCurrentNorm ).GetNormalized();

			*pPoints++	= pos;
			//*pI++		= pos - camWorldPos;
			*pI++		= (pos - camWorldPos).GetNormalized();
			//*pN++		= Vector3(0,0,0) - (pos - camWorldPos).GetNormalized();
			*pN++		= nor;
			*pNg++		= nor;
			*pOs++		= mpAttribs->mOpacity;
			*pCs++		= mpAttribs->mColor;

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
bool DiceablePrim::IsDiceable(
						MicroPolygonGrid &g,
						HiderBase *pHider,
						bool &out_uSplit,
						bool &out_vSplit )
{
	out_uSplit = false;
	out_vSplit = false;
	
	Matrix44 mtxLocalCamera =
				mpTransform->GetMatrix() *
					pHider->mMtxWorldCamera;
	
	Bound	bound;
	MakeBound( bound );
	float pixelArea = pHider->RasterEstimate( bound, mtxLocalCamera );
	
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
					mtxLocalCamera
					);

			return true;
		}
		else
			return false;
	}
	else
	{
		out_uSplit = true;
		out_vSplit = true;
		return false;
	}
}

//==================================================================
}