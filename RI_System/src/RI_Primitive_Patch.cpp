/*
 *  RI_Primitive_Poly.cpp
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
#include "RI_Primitive_Patch.h"
#include "RI_FrameworkREYES.h"

//==================================================================
namespace RI
{

//==================================================================
PatchMesh::PatchMesh( RtToken type,
					  ParamList &params,
					  const SymbolList &staticSymbols ) :
	ComplexPrimitiveBase(PATCHMESH),
	mParams(params)
{
	mpyPatchType = staticSymbols.FindVoid( type );
}

//==================================================================
void PatchMesh::Simplify( HiderREYES &hider )
{
	// PatchMesh "bilinear" 2 "nonperiodic" 5 "nonperiodic" "P"  [ -0.995625 2 -0.495465 ...
	//               0      1       2       3       4        5     6

	int			nu		= mParams[1];
	CPSymVoid	pyUWrap = hider.mpStatics->FindVoid( mParams[2] );
	int			nv		= mParams[3];
	CPSymVoid	pyVWrap = hider.mpStatics->FindVoid( mParams[4] );

	bool	uPeriodic = pyUWrap->IsNameI( RI_PERIODIC );
	bool	vPeriodic = pyVWrap->IsNameI( RI_PERIODIC );
		
	int	PValuesParIdx = findParam( "P", Param::FLT_ARR, 5, mParams );
	if ( PValuesParIdx == -1 )
		return;
	
	int			meshHullSize = 3 * nu * nv;
	const float	*pMeshHull = mParams[PValuesParIdx].PFlt( meshHullSize );

	if ( mpyPatchType->IsNameI( RI_BILINEAR ) )
	{
		int	nUPatches = nu - 1 + uPeriodic ? 1 : 0;
		int	nVPatches = nv - 1 + vPeriodic ? 1 : 0;

		Vec3f	hullv3[4];

		for (int i=0; i < nUPatches; ++i)
		{
			int	ii = (i+1) % nu;

			for (int j=0; j < nVPatches; ++j)
			{
				int	jj = (j+1) % nv;

				hullv3[0] = Vec3f( &pMeshHull[(i +nu*j )*3] );
				hullv3[1] = Vec3f( &pMeshHull[(ii+nu*j )*3] );
				hullv3[2] = Vec3f( &pMeshHull[(i +nu*jj)*3] );
				hullv3[3] = Vec3f( &pMeshHull[(ii+nu*jj)*3] );

				hider.InsertSimple(
						new RI::PatchBilinear( mParams, hullv3 ),
						*this
						);
			}
		}
	}
	else
	if ( mpyPatchType->IsNameI( RI_BICUBIC ) )
	{
		const Attributes	&attr = *mpAttribs;
		
		int uSteps = attr.mUSteps;
		int vSteps = attr.mVSteps;

		DASSTHROW( uSteps >= 1 && uSteps <= 16, ("Invalid uSteps %i", uSteps) );
		DASSTHROW( vSteps >= 1 && vSteps <= 16, ("Invalid vSteps %i", vSteps) );
		
		int	nUPatches = uPeriodic ? nu / uSteps : (nu-4) / uSteps + 1;
		int	nVPatches = vPeriodic ? nv / vSteps : (nv-4) / vSteps + 1;

		Vec3f	hullv3[16];
		
		for (int j0=0; j0 < nVPatches; ++j0)
		{
			for (int i0=0; i0 < nUPatches; ++i0)
			{
				int	hidx = 0;

				for (int j=0; j < 4; ++j)
				{
					for (int i=0; i < 4; ++i)
					{
						int	srcIdx = ((i + i0 * uSteps) % nu) +
									 ((j + j0 * vSteps) % nv) * nu;

						DASSERT( srcIdx >= 0 && (srcIdx*3) < meshHullSize );

						hullv3[hidx++] = Vec3f( &pMeshHull[ srcIdx * 3 ]);
					}
				}

				hider.InsertSimple(
						new RI::PatchBicubic( mParams, hullv3, attr, *hider.mpStatics ),
						*this
						);
			}
		}
	}
	else
	{
		DASSTHROW( 0, ("Unrecognized Patch type %s", mpyPatchType->pName ) );
		//ErrHandler( E_BADARGUMENT );
	}	
}

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const SymbolList &staticSymbols ) :
	SimplePrimitiveBase(PATCHBILINEAR),
	mParams(params)
{
	bool	gotP = ParamsFindP( params, staticSymbols, mHullPos, 4 );
	
	DASSTHROW( gotP, ("Missing hull parameter") );
}

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const Vec3f hull[4] ) :
	SimplePrimitiveBase(PATCHBILINEAR),
	mParams(params)
{
	for (int i=0; i < 4; ++i)
		mHullPos[i] = hull[i];
}

//==================================================================
void PatchBilinear::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	Vec3f	left	= DMix( mHullPos[0], mHullPos[2], v );
	Vec3f	right	= DMix( mHullPos[1], mHullPos[3], v );
	out_pt			= DMix( left, right, u );

	if ( out_dPdu )
	{
		Vec3f	left	= mHullPos[2] - mHullPos[0];
		Vec3f	right	= mHullPos[3] - mHullPos[1];
		*out_dPdv		= DMix( left, right, u );

		Vec3f	bottom	= mHullPos[1] - mHullPos[0];
		Vec3f	top		= mHullPos[3] - mHullPos[2];
		*out_dPdu		= DMix( bottom, top, v );
	}
}

//==================================================================
bool PatchBilinear::MakeBound( Bound &out_bound ) const
{
	Point3	Po;
	out_bound.Reset();
	out_bound.Expand( EvalP( mURange[0], mVRange[0], Po ) );
	out_bound.Expand( EvalP( mURange[1], mVRange[0], Po ) );
	out_bound.Expand( EvalP( mURange[0], mVRange[1], Po ) );
	out_bound.Expand( EvalP( mURange[1], mVRange[1], Po ) );
	return true;
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols ) :
	SimplePrimitiveBase(PATCHBICUBIC),
	mParams(params)
{
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();

	bool	gotP = ParamsFindP( params, staticSymbols, mHullPos, 16 );
	
	DASSTHROW( gotP, ("Missing hull parameter") );

	setupEvalCalc();
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params,
							const Vec3f hull[16],
						    const Attributes &attr,
							const SymbolList &staticSymbols ) :
	SimplePrimitiveBase(PATCHBICUBIC),
	mParams(params)
{
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();

	for (int i=0; i < 16; ++i)
		mHullPos[i] = hull[i];

	setupEvalCalc();
}

//==================================================================
void PatchBicubic::setupEvalCalc()
{
	// only needed for diceing and for evals for bbox and other estimators
	mCalcU[0].Setup( *mpUBasis, mHullPos[ 0], mHullPos[ 1], mHullPos[ 2], mHullPos[ 3] );
	mCalcU[1].Setup( *mpUBasis, mHullPos[ 4], mHullPos[ 5], mHullPos[ 6], mHullPos[ 7] );
	mCalcU[2].Setup( *mpUBasis, mHullPos[ 8], mHullPos[ 9], mHullPos[10], mHullPos[11] );
	mCalcU[3].Setup( *mpUBasis, mHullPos[12], mHullPos[13], mHullPos[14], mHullPos[15] );

	mCalcV[0].Setup( *mpVBasis, mHullPos[ 0], mHullPos[ 4], mHullPos[ 8], mHullPos[12] );
	mCalcV[1].Setup( *mpVBasis, mHullPos[ 1], mHullPos[ 5], mHullPos[ 9], mHullPos[13] );
	mCalcV[2].Setup( *mpVBasis, mHullPos[ 2], mHullPos[ 6], mHullPos[10], mHullPos[14] );
	mCalcV[3].Setup( *mpVBasis, mHullPos[ 3], mHullPos[ 7], mHullPos[11], mHullPos[15] );
}

//==================================================================
static inline Vec3f spline( float t,
					const RtBasis &b,
					const Vec3f &p0,
					const Vec3f &p1,
					const Vec3f &p2,
					const Vec3f &p3 )
{
	Vec3f v0(b.u.m44[0][0]*p0 + b.u.m44[0][1]*p1 + b.u.m44[0][2]*p2 + b.u.m44[0][3]*p3);
	Vec3f v1(b.u.m44[1][0]*p0 + b.u.m44[1][1]*p1 + b.u.m44[1][2]*p2 + b.u.m44[1][3]*p3);
	Vec3f v2(b.u.m44[2][0]*p0 + b.u.m44[2][1]*p1 + b.u.m44[2][2]*p2 + b.u.m44[2][3]*p3);
	Vec3f v3(b.u.m44[3][0]*p0 + b.u.m44[3][1]*p1 + b.u.m44[3][2]*p2 + b.u.m44[3][3]*p3);

	return	v0 *t*t*t +
			v1 *t*t +
			v2 *t +
			v3 ;
}

//==================================================================
static inline Vec3f splineDeriv( float t,
					const RtBasis &b,
					const Vec3f &p0,
					const Vec3f &p1,
					const Vec3f &p2,
					const Vec3f &p3 )
{
	Vec3f v0(b.u.m44[0][0]*p0 + b.u.m44[0][1]*p1 + b.u.m44[0][2]*p2 + b.u.m44[0][3]*p3);
	Vec3f v1(b.u.m44[1][0]*p0 + b.u.m44[1][1]*p1 + b.u.m44[1][2]*p2 + b.u.m44[1][3]*p3);
	Vec3f v2(b.u.m44[2][0]*p0 + b.u.m44[2][1]*p1 + b.u.m44[2][2]*p2 + b.u.m44[2][3]*p3);

	return	v0 *3*t*t +
			v1 *2*t +
			v2 ;
}

//==================================================================
void PatchBicubic::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	const RtBasis	&uBasis = *mpUBasis;
	const RtBasis	&vBasis = *mpVBasis;

	Point3	uBottom	= mCalcU[0].Eval( u );
	Point3	uMid1	= mCalcU[1].Eval( u );
	Point3	uMid2	= mCalcU[2].Eval( u );
	Point3	uTop	= mCalcU[3].Eval( u );

	out_pt = spline( v, vBasis, uBottom, uMid1, uMid2, uTop );

	if ( out_dPdu )
	{
		*out_dPdv = splineDeriv( v, vBasis, uBottom, uMid1, uMid2, uTop );

		Point3	vBottom	= mCalcV[0].Eval( v );
		Point3	vMid1	= mCalcV[1].Eval( v );
		Point3	vMid2	= mCalcV[2].Eval( v );
		Point3	vTop	= mCalcV[3].Eval( v );

		*out_dPdu = splineDeriv( u, uBasis, vBottom, vMid1, vMid2, vTop );
	}
}

//==================================================================
bool PatchBicubic::MakeBound( Bound &out_bound ) const
{
	Point3	Po;
	out_bound.Reset();
	out_bound.Expand( EvalP( mURange[0], mVRange[0], Po ) );
	out_bound.Expand( EvalP( mURange[1], mVRange[0], Po ) );
	out_bound.Expand( EvalP( mURange[0], mVRange[1], Po ) );
	out_bound.Expand( EvalP( mURange[1], mVRange[1], Po ) );
	return true;
}

//==================================================================
}