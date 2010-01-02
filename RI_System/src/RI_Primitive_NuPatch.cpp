//==================================================================
/// RI_Primitive_NuPatch.cpp
///
/// Partly based on the "Graphics Gems IV" code by John Peterson
///
/// Created by Davide Pasca - 2009/12/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Primitive_NuPatch.h"

#define MAXORDER 20	    // Maximum order allowed

//==================================================================
namespace RI
{

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
	DASSTHROW( uorder <= MAXORDER && vorder <= MAXORDER, ("Patch order must be no higher than %i", MAXORDER) );
	DASSTHROW( umin < umax && vmin < vmax, ("'min' parameter must be less than 'max'") );

	fillKnot( moBaseDef->mUKnots, pUknot, nu, uorder );
	fillKnot( moBaseDef->mVKnots, pVknot, nv, vorder );

	// order-1 really ?
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
	size_t	nPoints		 = (size_t)(nu * nv);

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

	const float	*pSrcPos = params[PValuesParIdx].PFlt( nPoints * dimsN );

	if ( dimsN == 3 )
	{
		moBaseDef->mCtrlPws.resize( nPoints );
		for (size_t i=0; i < nPoints; ++i)
		{
			Vec4f	&pw = moBaseDef->mCtrlPws[i];
			pw.x() = pSrcPos[ i*3 + 0 ];
			pw.x() = pSrcPos[ i*3 + 1 ];
			pw.x() = pSrcPos[ i*3 + 2 ];
			pw.w() = 1.0f;
		}
	}
	else
	{
		moBaseDef->mCtrlPws.resize( nPoints );
		for (size_t i=0; i < nPoints; ++i)
			moBaseDef->mCtrlPws[i].Set( pSrcPos + i*4 );
	}
}

//==================================================================
/*
 * Return the current knot the parameter u is less than or equal to.
 * Find this "breakpoint" allows the evaluation routines to concentrate on
 * only those control points actually effecting the curve around u.]
 *
 *	m   is the number of points on the curve (or surface direction)
 *	k   is the order of the curve (or surface direction)
 *	kv  is the knot vector ([0..m+k-1]) to find the break point in.
 */
static SlScalari findBreakPoint(
					const SlScalar		&u,
					const DVec<float>	&kv,
					int					m,
					int					k )
{
	SlScalari	ret;

	for (size_t smdx_=0; smdx_ < DMT_SIMD_FLEN; ++smdx_)
	{
		// make sure that 'u' isn't larger than
		// the largest node value !!!
		// (should handle eventual error introduced in uvs
		// when splitting/dicing)
		float	useU = D::Min( u[smdx_], kv[m+k] );

		if ( useU == kv[m+1] )	// Special case for closed interval
		{
			ret[smdx_] = m;
		}
		else
		{
			int i = m + k;

			while ( useU < kv[i] && i > 0 )
				--i;

			ret[smdx_] = i;
		}
	}

	return ret;
}

//==================================================================
/*
 * Compute Bi,k(u), for i = 0..k.
 *  u		is the parameter of the spline to find the basis functions for
 *  brkPoint	is the start of the knot interval ("segment")
 *  kv		is the knot vector
 *  k		is the order of the curve
 *  bvals	is the array of returned basis values.
 *
 * (From Bartels, Beatty & Barsky, p.387)
 */
static void basisFunctions(
					const SlScalar		&u,
					const SlScalari		&brkPoint,
					const DVec<float>	&kv,
					int					k,
					SlScalar			bvals[] )
{
    bvals[0] = 1.0f;

    for (int r = 2; r <= k; r++)
    {
		bvals[r - 1] = 0.0f;

		for (size_t smdx_=0; smdx_ < DMT_SIMD_FLEN; ++smdx_)
		{
			int i = brkPoint[smdx_] - r + 1;
			
			for (int s = r-2; s >= 0; --s)
			{
				++i;

				float omega;

				if (i < 0)
					omega = 0;
				else
					omega = (u[smdx_] - kv[i]) / (kv[i + r - 1] - kv[i]);

				bvals[s + 1][smdx_] += (1 - omega) * bvals[s][smdx_];
				bvals[s][smdx_] = omega * bvals[s][smdx_];
			}
		}
    }
}

//==================================================================
// Compute derivatives of the basis functions Bi,k(u)'
static void basisDerivatives(
					const SlScalar		&u,
					const SlScalari		&brkPoint,
					const DVec<float>	&kv,
					int					k,
					SlScalar			dvals[] )
{
    basisFunctions( u, brkPoint, kv, k - 1, dvals );

    dvals[k-1] = 0.f;	    // basisFunctions misses this

	for (size_t smdx_=0; smdx_ < DMT_SIMD_FLEN; ++smdx_)
	{
		float knotScale = kv[brkPoint[smdx_] + 1] - kv[brkPoint[smdx_]];

		int i = brkPoint[smdx_] - k + 1;
		for (int s = k - 2; s >= 0; --s)
		{
			++i;

			float omega = knotScale * ((float)(k-1)) / (kv[i+k-1] - kv[i]);
			dvals[s + 1][smdx_] += -omega * dvals[s][smdx_];
			dvals[s    ][smdx_] *= omega;
		}
	}
}

//==================================================================
/*
 * Calculate a point p on NurbSurface n at a specific u, v using the tensor product.
 * If utan and vtan are not nil, compute the u and v tangents as well.
 *
 * Note the valid parameter range for u and v is
 * (kvU[orderU] <= u < kvU[numU), (kvV[orderV] <= v < kvV[numV])
 */
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

	SlScalar bu[MAXORDER], buprime[MAXORDER];
    SlScalar bv[MAXORDER], bvprime[MAXORDER];

	SlVec4 r( 0.f );
	SlVec4 rutan( 0.f );
	SlVec4 rvtan( 0.f );

    // Evaluate non-uniform basis functions (and derivatives)
	SlScalari ubrkPoint = findBreakPoint( uv[0], uknots, moBaseDef->mNu-1, uorder );
	SlScalari ufirst = ubrkPoint - uorder + 1;
	basisFunctions( uv[0], ubrkPoint, uknots, uorder, bu );

	if (out_dPdu)
		basisDerivatives( uv[0], ubrkPoint, uknots, uorder, buprime );

	SlScalari vbrkPoint = findBreakPoint( uv[1], vknots, moBaseDef->mNv-1, vorder );
	SlScalari vfirst = vbrkPoint - vorder + 1;
	basisFunctions( uv[1], vbrkPoint, vknots, vorder, bv );

	if (out_dPdv)
		basisDerivatives( uv[1], vbrkPoint, vknots, vorder, bvprime );

	// Weight control points against the basis functions

	int	ptsPerRow = moBaseDef->mNu;

	SlScalari	idx = vfirst * ptsPerRow + ufirst;

	const DVec<Vec4f>	&ctrlPws = moBaseDef->mCtrlPws;

    for (int i = 0; i < vorder; ++i)
	{
		int ri = vorder - 1 - i;

		for (int j = 0; j < uorder; ++j)
		{
			int rj = uorder - 1 - j;

		    SlVec4	srcPw;		
			for (size_t smdx_=0; smdx_ < DMT_SIMD_FLEN; ++smdx_)
			{
				Vec4f	tmp = ctrlPws[ idx[smdx_] + j ];
				srcPw.x()[smdx_] = tmp.x();
				srcPw.y()[smdx_] = tmp.y();
				srcPw.z()[smdx_] = tmp.z();
				srcPw.w()[smdx_] = tmp.w();
			}

			SlScalar tensor = bu[rj] * bv[ri];

			r += srcPw * tensor;

			if ( out_dPdu )
			{
				tensor = buprime[rj] * bv[ri];
				rutan += srcPw * tensor;

				tensor = bu[rj] * bvprime[ri];
				rvtan += srcPw * tensor;
			}
		}

		idx += ptsPerRow;
	}

	if ( out_dPdu )
	{
		// Project tangents, using the quotient rule for differentiation
		SlScalar wsqrdiv = SlScalar( 1.f ) / (r.w() * r.w());

		*out_dPdu = (r.w() * rutan.GetAsV3() - rutan.w() * r.GetAsV3()) * wsqrdiv;
		*out_dPdv = (r.w() * rvtan.GetAsV3() - rvtan.w() * r.GetAsV3()) * wsqrdiv;
	}

    out_pt = r.GetAsV3() / r.w();
}

//==================================================================
bool NuPatch::MakeBound( Bound &out_bound ) const
{
	return MakeBoundFromUVRange9( *this, out_bound );
}

//==================================================================
}