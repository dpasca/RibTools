//==================================================================
/// RI_Primitive_NuPatch.cpp
///
/// Partly based on the "Graphics Gems" code by John Peterson
///
/// Created by Davide Pasca - 2009/12/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Primitive_NuPatch.h"

//==================================================================
namespace RI
{

#define MAXORDER 20	    // Maximum order allowed (for local array sizes)

//==================================================================
static void fillKnot( DVec<float> &out_knots, const float *pSrcKnots, int n, int order )
{
	out_knots.resize( n + order );
	for (size_t i=0; i < out_knots.size(); ++i)
	{
		if ( i > 0 )
			DASSTHROW( pSrcKnots[i-1] <= pSrcKnots[i], ("Invalid knot value") );

		out_knots[i] = pSrcKnots[i];
	}
}

//==================================================================
template<class _S, class _T>
inline _S DSelectInRageIE( const _S &x, const _T &leftInclusive, const _T &rightExclusive  )
{
	_S	ret( (_T)0 );

	for (size_t i=0; i < DMT_SIMD_FLEN; ++i)
	{
		if ( x[i] >= leftInclusive && x[i] < rightExclusive )
			ret[i] = (_T)1;
	}

	return ret;
}

//==================================================================
// See http://www.gamasutra.com/features/19991117/eq_1.gif
static SlScalar eval_Base(
					int					i,
					int					p,
					const SlScalar		&u,
					const DVec<float>	&knots )
{
	if ( p == 0 )
	{
		float	ui	= knots[ i + 0 ];
		float	ui1 = knots[ i + 1 ];

/*		if ( u >= ui && u < ui1 )
			return 1;
		else
			return 0;
*/
		return DSelectInRageIE<SlScalar,float>( u, ui, ui1 );
	}

	float	u_i00	= knots[ i + 0 + 0 ];
	float	u_i10	= knots[ i + 1 + 0 ];
	float	u_i0p	= knots[ i + 0 + p ];
	float	u_i1p	= knots[ i + 1 + p ];

	float	div1 = u_i0p - u_i00;
	float	div2 = u_i1p - u_i10;

	SlScalar	tmp1( 0.0f );
	SlScalar	tmp2( 0.0f );

	if ( div1 )		tmp1 = (u - u_i00) / div1;
	if ( div2 )		tmp2 = (u_i1p - u) / div2;

	SlScalar	B__i0_pn1 = eval_Base( i + 0, p - 1, u, knots );	
	SlScalar	B__i1_pn1 = eval_Base( i + 1, p - 1, u, knots );

	return
		tmp1 * B__i0_pn1 +
		tmp2 * B__i1_pn1;
}

//==================================================================
static void calcDenumArrayForDerivative( DVec<float> &out_denum, int segmentsN, const DVec<float> &knots, int degree )
{
	out_denum.resize( segmentsN + 1 + 1 );

	int	knotsMaxIdx = (int)knots.size() - 1;

	for (int i=0; i <= (segmentsN+1); ++i)
	{
		int	idx1 = i + degree;
		int	idx2 = i;

		idx1 = D::Min( idx1, knotsMaxIdx );
		idx2 = D::Min( idx2, knotsMaxIdx );

		float	denum = knots[ idx1 ] - knots[ idx2 ];

		if ( denum > 0 )
			out_denum[ i ] = degree / denum;
		else
			out_denum[ i ] = 0;
	}
}

//==================================================================
/// NuPatch
//==================================================================
NuPatch::NuPatch(
			int			nu		,
			int			uorder	,
			const float	*pUknot	,
			float		umin	,
			float		umax	,
			int			nv		,
			int			vorder	,
			const float	*pVknot	,
			float		vmin	,
			float		vmax	,
			ParamList	&params
			) :
		SimplePrimitiveBase(PrimitiveBase::NUPATCH, umin, umax, vmin, vmax)
{
	moBaseDef = new BaseDef( uorder, vorder, nu, nv );

	DASSTHROW( uorder > 1 && vorder > 1, ("Patch order must be 2 or larger") );
	DASSTHROW( umin < umax && vmin < vmax, ("'min' parameter must be less than 'max'") );

	fillKnot( moBaseDef->mUKnots, pUknot, nu, uorder );
	fillKnot( moBaseDef->mVKnots, pVknot, nv, vorder );

	// order-1th really ?
	DASSTHROW(
		umin >= moBaseDef->mUKnots[ uorder-2 ] &&
		vmin >= moBaseDef->mVKnots[ vorder-2 ],
		("'min' parameter must be larger or equal of knot[ order-2 ]") );

	DASSTHROW(
		umax <= moBaseDef->mUKnots[ nu ] &&
		vmax <= moBaseDef->mVKnots[ nv ],
		("'max' less than knot[ n-1 ]") );

	int	udegree = uorder - 1;
	int	vdegree = vorder - 1;

	int	uSegmentsN = nu - udegree;
	int	vSegmentsN = nv - udegree;

	DASSTHROW( uSegmentsN <= 0 || vSegmentsN, ("The patch generates no segments !") );

	size_t	nVarsUniform = (size_t)(uSegmentsN * vSegmentsN);
	size_t	nVarsVarying = (size_t)((uSegmentsN + 1) * (vSegmentsN + 1));

	size_t	curParamIdx = 10;

	int		PValuesParIdx;
	size_t	dimsN = 3;

	// do we have 'P' ?
	PValuesParIdx = FindParam( "P", Param::FLT_ARR, curParamIdx, params );
	if ( PValuesParIdx == -1 )
	{
		// must have 'Pw' then !
		PValuesParIdx = FindParam( "Pw", Param::FLT_ARR, curParamIdx, params );
		if ( PValuesParIdx == -1 )
		{
			DASSTHROW( 0, ("Missing P or Pw parameter") );
			return;
		}

		dimsN = 4;
	}

	curParamIdx += 1;

	const float	*pSrcPos = params[PValuesParIdx].PFlt( nVarsVarying * dimsN );

	if ( dimsN == 3 )
	{
		moBaseDef->mCtrlPs.resize( nVarsVarying );
		for (size_t i=0; i < nVarsVarying; ++i)
			moBaseDef->mCtrlPs[i].Set( pSrcPos + i*3 );
	}
	else
	{
		moBaseDef->mCtrlPws.resize( nVarsVarying );
		for (size_t i=0; i < nVarsVarying; ++i)
			moBaseDef->mCtrlPws[i].Set( pSrcPos + i*4 );
	}

	calcDenumArrayForDerivative(
					moBaseDef->mCalc_UDerivCoe,
					uSegmentsN,
					moBaseDef->mUKnots,
					udegree );

	calcDenumArrayForDerivative(
					moBaseDef->mCalc_VDerivCoe,
					vSegmentsN,
					moBaseDef->mVKnots,
					vdegree );
}

//==================================================================
// For derivatives see http://heim.ifi.uio.no/michaelf/papers/biri_paper.pdf
//
void NuPatch::Eval_dPdu_dPdv(
				const SlVec2 &uv,
				SlVec3 &out_pt,
				SlVec3 *out_dPdu,
				SlVec3 *out_dPdv ) const
{
	int	uorder = moBaseDef->mUOrder;
	int	vorder = moBaseDef->mVOrder;

	int	udegree = uorder - 1;
	int	vdegree = vorder - 1;

	int	uSegmentsN = moBaseDef->mNu - udegree;
	int	vSegmentsN = moBaseDef->mNv - vdegree;

	const DVec<float> &uknots =	moBaseDef->mUKnots;
	const DVec<float> &vknots =	moBaseDef->mVKnots;

	size_t	stride = uSegmentsN + 1;

	out_pt.SetZero();

	if ( out_dPdu )
	{
		out_dPdu->SetZero();
		out_dPdv->SetZero();
	}

	size_t	ptsIdx = 0;
	// 4D
	if ( moBaseDef->mCtrlPws.size() )
	{
		SlVec4	accPw( 0.0f );
		SlVec4	dp_du( 0.0f );
		SlVec4	dp_dv( 0.0f );

		//SlScalar	div( 0.0f );
	
		SlScalar deriv_vnum1 = eval_Base( 0, vdegree-1, uv[1], vknots );
		SlScalar deriv_vnum2;
		float	 deriv_vcoe1 = moBaseDef->mCalc_VDerivCoe[ 0 ];
		float	 deriv_vcoe2;

		for (int j=0; j <= vSegmentsN; ++j)
		{
			size_t	 ptsIdxRow = ptsIdx;
			SlScalar B_j_q = eval_Base( j, vdegree, uv[1], vknots );

			deriv_vnum2 = eval_Base( j + 1, vdegree-1, uv[1], vknots );
			deriv_vcoe2 = moBaseDef->mCalc_VDerivCoe[ j + 1 ];

			SlScalar deriv_unum1 = eval_Base( 0, udegree-1, uv[0], uknots );
			SlScalar deriv_unum2;
			float	 deriv_ucoe1 = moBaseDef->mCalc_UDerivCoe[ 0 ];
			float	 deriv_ucoe2;

			for (int i=0; i <= uSegmentsN; ++i)
			{
				SlScalar B_i_p = eval_Base( i, udegree, uv[0], uknots );

				deriv_unum2 = eval_Base( i + 1, udegree-1, uv[0], uknots );
				deriv_ucoe2 = moBaseDef->mCalc_UDerivCoe[ i + 1 ];

				const Vec4f	&srcPw = moBaseDef->mCtrlPws[ ptsIdxRow + i ];

				SlVec4	slPw = SlVec4( srcPw.x(), srcPw.y(), srcPw.z(), srcPw.w() );

				SlScalar	tmp = B_i_p * B_j_q;

				accPw += slPw * tmp;
				//div += tmp;

				if ( out_dPdu )
				{
					dp_du += slPw * (deriv_unum2 * deriv_ucoe2 - deriv_unum1 * deriv_ucoe1);
					dp_dv += slPw * (deriv_vnum2 * deriv_vcoe2 - deriv_vnum1 * deriv_vcoe1);
				}

				deriv_unum1 = deriv_unum2;
				deriv_ucoe1 = deriv_ucoe2;
			}

			ptsIdx += stride;

			deriv_vnum1 = deriv_vnum2;
			deriv_vcoe1 = deriv_vcoe2;
		}

		out_pt = accPw.GetAsV3() / accPw.w();

		if ( out_dPdu )
		{
			*out_dPdu = dp_du.GetAsV3() / dp_du.w();
			*out_dPdv = dp_dv.GetAsV3() / dp_dv.w();
		}
	}
	else
	{
		// 3D

		DASSERT( 0 );

/*
		for (int j=0; j < vorder; ++j)
		{
			size_t	 ptsIdxRow = ptsIdx;
			SlScalar B_j_q = eval_Base( j, vorder-1, uv[1], moBaseDef->mVKnots );

			for (int i=0; i < uorder; ++i)
			{
				SlScalar B_i_p = eval_Base( j, uorder-1, uv[0], moBaseDef->mUKnots );

				const Vec3f	&srcP = moBaseDef->mCtrlPs[ ptsIdxRow + i ];

				out_pt += SlVec3( srcP.x(), srcP.y(), srcP.z() ) * B_i_p * B_j_q;
			}

			ptsIdx += stride;
		}
*/
	}


/*
	const RtBasis	&uBasis = *mpUBasis;
	const RtBasis	&vBasis = *mpVBasis;

	SlVec3 uBottom	= mCalcU[0].Eval( uv[0] );
	SlVec3 uMid1	= mCalcU[1].Eval( uv[0] );
	SlVec3 uMid2	= mCalcU[2].Eval( uv[0] );
	SlVec3 uTop		= mCalcU[3].Eval( uv[0] );

	out_pt = spline( uv[1], vBasis, uBottom, uMid1, uMid2, uTop );

	if ( out_dPdu )
	{
		*out_dPdv = splineDeriv( uv[1], vBasis, uBottom, uMid1, uMid2, uTop );

		SlVec3 vBottom	= mCalcV[0].Eval( uv[1] );
		SlVec3 vMid1	= mCalcV[1].Eval( uv[1] );
		SlVec3 vMid2	= mCalcV[2].Eval( uv[1] );
		SlVec3 vTop		= mCalcV[3].Eval( uv[1] );

		*out_dPdu = splineDeriv( uv[0], uBasis, vBottom, vMid1, vMid2, vTop );
	}
*/
}

//==================================================================
}