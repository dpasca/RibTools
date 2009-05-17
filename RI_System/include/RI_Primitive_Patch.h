//==================================================================
/// RI_Primitive_Patch.h
///
/// Created by Davide Pasca - 2009/1/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_PATCH_H
#define RI_PRIMITIVE_PATCH_H

#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

//==================================================================
template <class _S>
bool MakeBoundFromUVRange( const _S &This, Bound &out_bound )
{
#if 1

	const float srcUVs[RI_GET_SIMD_PAD_SUBS(4)][2] = {
		This.mURange[0], This.mVRange[0],
		This.mURange[1], This.mVRange[0],
		This.mURange[0], This.mVRange[1],
		This.mURange[1], This.mVRange[1]
	};

	out_bound.Reset();

	for (size_t blkIdx=0; blkIdx < RI_GET_SIMD_BLOCKS( 4 ); ++blkIdx)
	{
		SlVec2	uvs;

		size_t	iStart	= (blkIdx + 0) * RI_SIMD_BLK_LEN + 0;
		size_t	iEnd	= (blkIdx + 1) * RI_SIMD_BLK_LEN + 0;
		iEnd = DMIN( 4, iEnd );
		size_t	n = iEnd - iStart;

		for (size_t subIdx=0; subIdx < n; ++subIdx)
		{
			uvs[0][subIdx] = srcUVs[(blkIdx + 0) * RI_SIMD_BLK_LEN + subIdx][0];
			uvs[1][subIdx] = srcUVs[(blkIdx + 0) * RI_SIMD_BLK_LEN + subIdx][1];
		}

		SlVec3	Po;
		This.EvalP( uvs, Po );


		for (size_t subIdx=0; subIdx < n; ++subIdx)
		{
			out_bound.Expand( Vec3f(
									Po[0][subIdx],
									Po[1][subIdx],
									Po[2][subIdx]
								) );
		}
	}

#else

	Point3	Po;
	out_bound.Reset();
	out_bound.Expand( This.EvalP( This.mURange[0], This.mVRange[0], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[1], This.mVRange[0], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[0], This.mVRange[1], Po ) );
	out_bound.Expand( This.EvalP( This.mURange[1], This.mVRange[1], Po ) );

#endif

	return true;
}

//==================================================================
/// PatchBilinear
//==================================================================
class PatchBilinear : public SimplePrimitiveBase
{
private:
	ParamList		mParams;
	Vec3f			mHullPos_sca[4];
	SlVec3			mHullPos[4];

public:
	PatchBilinear( ParamList &params, const SymbolList &staticSymbols );
	PatchBilinear( ParamList &params, const Vec3f hull[4] );

		PatchBilinear *Clone() const {	return DNEW PatchBilinear( *this ); }

		bool MakeBound( Bound &out_bound ) const
		{
			return MakeBoundFromUVRange( *this, out_bound );
		}

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// SplinePatchCalc
//==================================================================
template <class _S, class _T>
class SplinePatchCalc
{
	_S v0;
	_S v1;
	_S v2;
	_S v3;

public:
	void Setup( const RtBasis &b,
				const _S &p0,
				const _S &p1,
				const _S &p2,
				const _S &p3 )
	{
		v0 = p0*b.u.m44[0][0] + p1*b.u.m44[0][1] + p2*b.u.m44[0][2] + p3*b.u.m44[0][3];
		v1 = p0*b.u.m44[1][0] + p1*b.u.m44[1][1] + p2*b.u.m44[1][2] + p3*b.u.m44[1][3];
		v2 = p0*b.u.m44[2][0] + p1*b.u.m44[2][1] + p2*b.u.m44[2][2] + p3*b.u.m44[2][3];
		v3 = p0*b.u.m44[3][0] + p1*b.u.m44[3][1] + p2*b.u.m44[3][2] + p3*b.u.m44[3][3];
	}

	inline _S	Eval( const _T &t ) const
	{
		const _T	t2 = t * t;

		return	v0 * t2 * t +
				v1 * t2 +
				v2 * t +
				v3 ;
	}

	inline _S	EvalDeriv( const _T &t ) const
	{
		return	v0 *3*t*t +
				v1 *2*t +
				v2 ;
	}
};

//==================================================================
/// PatchBicubic
//==================================================================
class PatchBicubic : public SimplePrimitiveBase
{
private:
	ParamList				mParams;
	const RtBasis			*mpUBasis;
	const RtBasis			*mpVBasis;
	Vec3f					mHullPos[16];
	//SplinePatchCalc<Vec3f,float>	mCalcU_sca[4];
	//SplinePatchCalc<Vec3f,float>	mCalcV_sca[4];

	SplinePatchCalc<SlVec3,SlScalar>	mCalcU[4];
	SplinePatchCalc<SlVec3,SlScalar>	mCalcV[4];

public:
	PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols );
	PatchBicubic( ParamList &params,
							const Vec3f hull[16],
						    const Attributes &attr,
							const SymbolList &staticSymbols );

		PatchBicubic *Clone() const {	return DNEW PatchBicubic( *this ); }

		bool MakeBound( Bound &out_bound ) const
		{
			return MakeBoundFromUVRange( *this, out_bound );
		}

/*
		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;
*/

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;

private:
	void setupEvalCalc();
};

//==================================================================
/// PatchMesh
//==================================================================
class PatchMesh : public ComplexPrimitiveBase
{
public:
	CPSymVoid		mpyPatchType;
	ParamList		mParams;

public:
	PatchMesh(RtToken type,
			  ParamList &params,
			  const SymbolList &staticSymbols );

		void Simplify( HiderREYES &hider );
};

//==================================================================
/// NuPatch
//==================================================================
class NuPatch : public SimplePrimitiveBase
{
public:
	NuPatch(
			int		nu		,
			int		uorder	,
			const float	*pUknot	,
			float	umin	,
			float	umax	,
			int		nv		,
			int		vorder	,
			const float	*pVknot	,
			float	vmin	,
			float	vmax	,
			ParamList &params
			);

		NuPatch	*Clone() const {	return DNEW NuPatch( *this ); }

		bool MakeBound( Bound &out_bound ) const
		{
			return MakeBoundFromUVRange( *this, out_bound );
		}

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const
		{
		}
};
	
//==================================================================
}

#endif