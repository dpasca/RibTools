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

//==================================================================
namespace RI
{

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
	Matrix44 mtxLocalCamera =
				mpTransform->GetMatrix() *
					pHider->mMtxWorldCamera;
	
	Bound	bound;
	MakeBound( bound );
	float pixelArea = pHider->RasterEstimate( bound );
	
	if ( pixelArea > MicroPolygonGrid::MAX_SIZE )
	{
		out_uSplit = true;
		out_vSplit = true;
		return false;
	}
	else
	{
		float	dim = sqrtf( pixelArea );
		g.Setup(
				(int)dim,
				(int)dim,
				mtxLocalCamera
				);

		out_uSplit = false;
		out_vSplit = false;
		return true;
	}
}

//==================================================================
}