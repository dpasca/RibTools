//==================================================================
/// RI_Primitive_Patch.cpp
///
/// Created by Davide Pasca - 2009/1/4
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive_Patch.h"
#include "RI_HiderST.h"

//==================================================================
namespace RI
{

//==================================================================
PatchMesh::PatchMesh( RtToken type,
					  ParamList &params,
					  const SymbolList &globalSymbols ) :
	ComplexPrimitiveBase(PATCHMESH),
	mParams(params)
{
	mpPatchType = globalSymbols.FindSymbol( type );
}

//==================================================================
void PatchMesh::Simplify( Hider &hider )
{
	// PatchMesh "bilinear" 2 "nonperiodic" 5 "nonperiodic" "P"  [ -0.995625 2 -0.495465 ...
	//               0      1       2       3       4        5     6

	int				nu		= mParams[1];
	const Symbol*	pyUWrap = hider.mpGlobalSyms->FindSymbol( mParams[2] );
	int				nv		= mParams[3];
	const Symbol*	pyVWrap = hider.mpGlobalSyms->FindSymbol( mParams[4] );

	bool	uPeriodic = pyUWrap->IsName( RI_PERIODIC );
	bool	vPeriodic = pyVWrap->IsName( RI_PERIODIC );
		
	int	PValuesParIdx = FindParam( "P", Param::FLT_ARR, 5, mParams );
	if ( PValuesParIdx == -1 )
		return;
	
	int			meshHullSize = 3 * nu * nv;
	const float	*pMeshHull = mParams[PValuesParIdx].PFlt( meshHullSize );

	if ( mpPatchType->IsName( RI_BILINEAR ) )
	{
		int	nUPatches = nu - 1 + uPeriodic ? 1 : 0;
		int	nVPatches = nv - 1 + vPeriodic ? 1 : 0;

		Float3	hullv3[4];

		for (int i=0; i < nUPatches; ++i)
		{
			int	ii = (i+1) % nu;

			for (int j=0; j < nVPatches; ++j)
			{
				int	jj = (j+1) % nv;

				hullv3[0] = Float3( &pMeshHull[(i +nu*j )*3] );
				hullv3[1] = Float3( &pMeshHull[(ii+nu*j )*3] );
				hullv3[2] = Float3( &pMeshHull[(i +nu*jj)*3] );
				hullv3[3] = Float3( &pMeshHull[(ii+nu*jj)*3] );

				hider.InsertSimple(
						DNEW RI::PatchBilinear( mParams, hullv3 ),
						*this
						);
			}
		}
	}
	else
	if ( mpPatchType->IsName( RI_BICUBIC ) )
	{
		const Attributes	&attr = *mpAttribs;
		
		int uSteps = attr.mUSteps;
		int vSteps = attr.mVSteps;

		DASSTHROW( uSteps >= 1 && uSteps <= 16, ("Invalid uSteps %i", uSteps) );
		DASSTHROW( vSteps >= 1 && vSteps <= 16, ("Invalid vSteps %i", vSteps) );
		
		int	nUPatches = uPeriodic ? nu / uSteps : (nu-4) / uSteps + 1;
		int	nVPatches = vPeriodic ? nv / vSteps : (nv-4) / vSteps + 1;

		Float3	hullv3[16];
		
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

						hullv3[hidx++] = Float3( &pMeshHull[ srcIdx * 3 ]);
					}
				}

				hider.InsertSimple(
						DNEW RI::PatchBicubic( mParams, hullv3, attr, *hider.mpGlobalSyms ),
						*this
						);
			}
		}
	}
	else
	{
		DASSTHROW( 0, ("Unrecognized Patch type %s", mpPatchType->mName ) );
		//ErrHandler( E_BADARGUMENT );
	}	
}

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const SymbolList &globalSymbols ) :
	SimplePrimitiveBase(PATCHBILINEAR)//,
	//mParams(params)
{
	Float3	pos[4];

	bool	gotP = ParamsFindP( params, globalSymbols, pos, 4 );

	for (int i=0; i < 4; ++i)
		mHullPos_sca[i] = pos[i];

	DASSTHROW( gotP, ("Missing hull parameter") );
}

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const Float3 hull[4] ) :
	SimplePrimitiveBase(PATCHBILINEAR)//,
	//mParams(params)
{
	for (int i=0; i < 4; ++i)
		mHullPos_sca[i] = hull[i];
}

/*
//==================================================================
void PatchBilinear::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Float3 *out_dPdu,
			Float3 *out_dPdv ) const
{
	Float3	left	= DMix( mHullPos_sca[0], mHullPos_sca[2], v );
	Float3	right	= DMix( mHullPos_sca[1], mHullPos_sca[3], v );
	out_pt			= DMix( left, right, u );

	if ( out_dPdu )
	{
		Float3	left	= mHullPos_sca[2] - mHullPos_sca[0];
		Float3	right	= mHullPos_sca[3] - mHullPos_sca[1];
		*out_dPdv		= DMix( left, right, u );

		Float3	bottom	= mHullPos_sca[1] - mHullPos_sca[0];
		Float3	top		= mHullPos_sca[3] - mHullPos_sca[2];
		*out_dPdu		= DMix( bottom, top, v );
	}
}
*/

//==================================================================
void PatchBilinear::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float3_	hullPos[4];

	for (int i=0; i < 4; ++i)
		hullPos[i] = mHullPos_sca[i];

	Float3_	left	= DMix( hullPos[0], hullPos[2], uv[1] );
	Float3_	right	= DMix( hullPos[1], hullPos[3], uv[1] );
	out_pt			= DMix( left, right, uv[0] );

	if ( out_dPdu )
	{
		Float3_	left	= hullPos[2] - hullPos[0];
		Float3_	right	= hullPos[3] - hullPos[1];
		*out_dPdv		= DMix( left, right, uv[0] );

		Float3_	bottom	= hullPos[1] - hullPos[0];
		Float3_	top		= hullPos[3] - hullPos[2];
		*out_dPdu		= DMix( bottom, top, uv[1] );
	}
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &globalSymbols ) :
	SimplePrimitiveBase(PATCHBICUBIC),
	mParams(params)
{
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();

	bool	gotP = ParamsFindP( params, globalSymbols, mHullPos, 16 );
	
	DASSTHROW( gotP, ("Missing hull parameter") );

	setupEvalCalc();
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params,
							const Float3 hull[16],
						    const Attributes &attr,
							const SymbolList &globalSymbols ) :
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
	// only needed for dicing and for evals for bbox and other estimators
/*
	mCalcU_sca[0].Setup( *mpUBasis, mHullPos[ 0], mHullPos[ 1], mHullPos[ 2], mHullPos[ 3] );
	mCalcU_sca[1].Setup( *mpUBasis, mHullPos[ 4], mHullPos[ 5], mHullPos[ 6], mHullPos[ 7] );
	mCalcU_sca[2].Setup( *mpUBasis, mHullPos[ 8], mHullPos[ 9], mHullPos[10], mHullPos[11] );
	mCalcU_sca[3].Setup( *mpUBasis, mHullPos[12], mHullPos[13], mHullPos[14], mHullPos[15] );

	mCalcV_sca[0].Setup( *mpVBasis, mHullPos[ 0], mHullPos[ 4], mHullPos[ 8], mHullPos[12] );
	mCalcV_sca[1].Setup( *mpVBasis, mHullPos[ 1], mHullPos[ 5], mHullPos[ 9], mHullPos[13] );
	mCalcV_sca[2].Setup( *mpVBasis, mHullPos[ 2], mHullPos[ 6], mHullPos[10], mHullPos[14] );
	mCalcV_sca[3].Setup( *mpVBasis, mHullPos[ 3], mHullPos[ 7], mHullPos[11], mHullPos[15] );
*/

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
template <class _S, class _T>
inline _S spline(
				const _T &t,
				const RtBasis &b,
				const _S &p0,
				const _S &p1,
				const _S &p2,
				const _S &p3 )
{
	_S v0(p0*b.u.m44[0][0] + p1*b.u.m44[0][1] + p2*b.u.m44[0][2] + p3*b.u.m44[0][3]);
	_S v1(p0*b.u.m44[1][0] + p1*b.u.m44[1][1] + p2*b.u.m44[1][2] + p3*b.u.m44[1][3]);
	_S v2(p0*b.u.m44[2][0] + p1*b.u.m44[2][1] + p2*b.u.m44[2][2] + p3*b.u.m44[2][3]);
	_S v3(p0*b.u.m44[3][0] + p1*b.u.m44[3][1] + p2*b.u.m44[3][2] + p3*b.u.m44[3][3]);

	return	v0 *t*t*t +
			v1 *t*t +
			v2 *t +
			v3 ;
}

//==================================================================
template <class _S, class _T>
inline _S splineDeriv(
					const _T &t,
					const RtBasis &b,
					const _S &p0,
					const _S &p1,
					const _S &p2,
					const _S &p3 )
{
	_S v0(p0*b.u.m44[0][0] + p1*b.u.m44[0][1] + p2*b.u.m44[0][2] + p3*b.u.m44[0][3]);
	_S v1(p0*b.u.m44[1][0] + p1*b.u.m44[1][1] + p2*b.u.m44[1][2] + p3*b.u.m44[1][3]);
	_S v2(p0*b.u.m44[2][0] + p1*b.u.m44[2][1] + p2*b.u.m44[2][2] + p3*b.u.m44[2][3]);

	return	v0 *3*t*t +
			v1 *2*t +
			v2 ;
}

//==================================================================
/*
void PatchBicubic::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Float3 *out_dPdu,
			Float3 *out_dPdv ) const
{
	const RtBasis	&uBasis = *mpUBasis;
	const RtBasis	&vBasis = *mpVBasis;

	Point3	uBottom	= mCalcU_sca[0].Eval( u );
	Point3	uMid1	= mCalcU_sca[1].Eval( u );
	Point3	uMid2	= mCalcU_sca[2].Eval( u );
	Point3	uTop	= mCalcU_sca[3].Eval( u );

	out_pt = spline( v, vBasis, uBottom, uMid1, uMid2, uTop );

	if ( out_dPdu )
	{
		*out_dPdv = splineDeriv( v, vBasis, uBottom, uMid1, uMid2, uTop );

		Point3	vBottom	= mCalcV_sca[0].Eval( v );
		Point3	vMid1	= mCalcV_sca[1].Eval( v );
		Point3	vMid2	= mCalcV_sca[2].Eval( v );
		Point3	vTop	= mCalcV_sca[3].Eval( v );

		*out_dPdu = splineDeriv( u, uBasis, vBottom, vMid1, vMid2, vTop );
	}
}
*/

//==================================================================
void PatchBicubic::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	const RtBasis	&uBasis = *mpUBasis;
	const RtBasis	&vBasis = *mpVBasis;

	Float3_ uBottom	= mCalcU[0].Eval( uv[0] );
	Float3_ uMid1	= mCalcU[1].Eval( uv[0] );
	Float3_ uMid2	= mCalcU[2].Eval( uv[0] );
	Float3_ uTop		= mCalcU[3].Eval( uv[0] );

	out_pt = spline( uv[1], vBasis, uBottom, uMid1, uMid2, uTop );

	if ( out_dPdu )
	{
		*out_dPdv = splineDeriv( uv[1], vBasis, uBottom, uMid1, uMid2, uTop );

		Float3_ vBottom	= mCalcV[0].Eval( uv[1] );
		Float3_ vMid1	= mCalcV[1].Eval( uv[1] );
		Float3_ vMid2	= mCalcV[2].Eval( uv[1] );
		Float3_ vTop		= mCalcV[3].Eval( uv[1] );

		*out_dPdu = splineDeriv( uv[0], uBasis, vBottom, vMid1, vMid2, vTop );
	}
}

//==================================================================
}