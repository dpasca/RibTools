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
class PatchBilinear : public DiceablePrim
{
private:
	ParamList		mParams;
	Vector3			mHullPos[4];

public:
	PatchBilinear( ParamList &params, const SymbolList &staticSymbols );
	PatchBilinear( ParamList &params, const Vector3 hull[4] );

		PatchBilinear *Clone() const {	return new PatchBilinear( *this ); }

		void MakeBound( Bound &out_bound );

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vector3 *out_dPdu,
					Vector3 *out_dPdv ) const;
};

//==================================================================
/// SplinePatchCalc
//==================================================================
class SplinePatchCalc
{
	Vector3 v0;
	Vector3 v1;
	Vector3 v2;
	Vector3 v3;

public:
	void Setup( const RtBasis &b,
				const Vector3 &p0,
				const Vector3 &p1,
				const Vector3 &p2,
				const Vector3 &p3 )
	{
		v0 = b.u.m44[0][0]*p0 + b.u.m44[0][1]*p1 + b.u.m44[0][2]*p2 + b.u.m44[0][3]*p3;
		v1 = b.u.m44[1][0]*p0 + b.u.m44[1][1]*p1 + b.u.m44[1][2]*p2 + b.u.m44[1][3]*p3;
		v2 = b.u.m44[2][0]*p0 + b.u.m44[2][1]*p1 + b.u.m44[2][2]*p2 + b.u.m44[2][3]*p3;
		v3 = b.u.m44[3][0]*p0 + b.u.m44[3][1]*p1 + b.u.m44[3][2]*p2 + b.u.m44[3][3]*p3;
	}

	inline Vector3	Eval( float t ) const
	{
		return	v0 *t*t*t +
				v1 *t*t +
				v2 *t +
				v3 ;
	}

	inline Vector3	EvalDeriv( float t ) const
	{
		return	v0 *3*t*t +
				v1 *2*t +
				v2 ;
	}
};

//==================================================================
/// PatchBicubic
//==================================================================
class PatchBicubic : public DiceablePrim
{
private:
	ParamList		mParams;
	const RtBasis	*mpUBasis;
	const RtBasis	*mpVBasis;
	Vector3			mHullPos[16];
	SplinePatchCalc	mCalcU[4];
	SplinePatchCalc	mCalcV[4];

public:
	PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols );
	PatchBicubic( ParamList &params,
							const Vector3 hull[16],
						    const Attributes &attr,
							const SymbolList &staticSymbols );

		PatchBicubic *Clone() const {	return new PatchBicubic( *this ); }

		void MakeBound( Bound &out_bound );

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vector3 *out_dPdu,
					Vector3 *out_dPdv ) const;

private:
	void setupEvalCalc();
};

//==================================================================
/// PatchMesh
//==================================================================
class PatchMesh : public Primitive
{
public:
	CPSymVoid		mpyPatchType;
	ParamList		mParams;

public:
	PatchMesh(RtToken type,
			  ParamList &params,
			  const SymbolList &staticSymbols );

		PatchMesh *Clone() const {	DASSERT( 0 ); return NULL; }

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }
		bool IsSplitable() const	{	return true; }
		void Split( FrameworkBase &fwork, bool uSplit, bool vSplit );

		void	Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const {}
};

	
//==================================================================
}

#endif