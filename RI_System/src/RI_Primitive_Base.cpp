/*
 *  RI_Primitive.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/04.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLUT/glut.h>
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
void Primitive::Dice( MicroPolygonGrid &g )
{
	Point3	*pPoints = g.mpPoints;
	
	float	du = 1.0f / g.mYDim;
	float	dv = 1.0f / g.mXDim;

	float	v = 0.0f;
	for (int i=0; i < (int)g.mYDim; ++i, v += dv)
	{
		float	u = 0.0f;
		for (int j=0; j < (int)g.mXDim; ++j, u += du)
		{
			EvalP( u, v, *pPoints, g.mMtxObjectCurrent );
			++pPoints;
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
	
	if ( /*mSplitCnt >= 1 ||*/ (pixelArea > 1 && pixelArea <= MicroPolygonGrid::MAX_SIZE) )
	{
		float	dim = sqrtf( pixelArea );
		g.Setup(
				(int)dim,
				(int)dim,
				mURange,
				mVRange,
				mtxLocalCamera
				);

		return true;
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