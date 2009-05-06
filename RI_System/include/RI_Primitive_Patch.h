/*
 *  RI_Primitive_Patch.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/18.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PRIMITIVE_PATCH_H
#define RI_PRIMITIVE_PATCH_H

#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

//==================================================================
/// PatchBilinear
//==================================================================
class PatchBilinear : public SimplePrimitiveBase
{
private:
	ParamList		mParams;
	Vec3f			mHullPos[4];

public:
	PatchBilinear( ParamList &params, const SymbolList &staticSymbols );
	PatchBilinear( ParamList &params, const Vec3f hull[4] );

		PatchBilinear *Clone() const {	return new PatchBilinear( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;
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
	SplinePatchCalc<Vec3f,float>	mCalcU_sca[4];
	SplinePatchCalc<Vec3f,float>	mCalcV_sca[4];

	SplinePatchCalc<SlVec3,SlScalar>	mCalcU[4];
	SplinePatchCalc<SlVec3,SlScalar>	mCalcV[4];

public:
	PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols );
	PatchBicubic( ParamList &params,
							const Vec3f hull[16],
						    const Attributes &attr,
							const SymbolList &staticSymbols );

		PatchBicubic *Clone() const {	return new PatchBicubic( *this ); }

		bool MakeBound( Bound &out_bound ) const;

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
}

#endif