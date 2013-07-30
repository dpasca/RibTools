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
#include "RI_Primitive_Utils.h"

//==================================================================
namespace RI
{

//==================================================================
/// PatchBilinear
//==================================================================
class PatchBilinear : public SimplePrimitiveBase
{
private:
	//ParamList		mParams;
	Float3			mHullPos_sca[4];
	//Float3_		mHullPos[4];

public:
	PatchBilinear( ParamList &params, const SymbolList &globalSymbols );
	PatchBilinear( ParamList &params, const Float3 hull[4] );

	~PatchBilinear()
	{
	}

		PatchBilinear *Clone() const {	return DNEW PatchBilinear( *this ); }

		void MakeBound( Bound &out_bound, Float3_ *out_pPo ) const
		{
			MakeBoundFromUVRangeN<SimplePrimitiveBase,2>( *this, out_bound, out_pPo );
		}

		void Eval_dPdu_dPdv(
						const Float2_ &uv,
						Float3_ &out_pt,
						Float3_ *out_dPdu,
						Float3_ *out_dPdv ) const;
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
		v0 = p0*b.mij(0,0) + p1*b.mij(0,1) + p2*b.mij(0,2) + p3*b.mij(0,3);
		v1 = p0*b.mij(1,0) + p1*b.mij(1,1) + p2*b.mij(1,2) + p3*b.mij(1,3);
		v2 = p0*b.mij(2,0) + p1*b.mij(2,1) + p2*b.mij(2,2) + p3*b.mij(2,3);
		v3 = p0*b.mij(3,0) + p1*b.mij(3,1) + p2*b.mij(3,2) + p3*b.mij(3,3);
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
	Float3					mHullPos[16];
	//SplinePatchCalc<Float3,float>	mCalcU_sca[4];
	//SplinePatchCalc<Float3,float>	mCalcV_sca[4];

	SplinePatchCalc<Float3_,Float_>	mCalcU[4];
	SplinePatchCalc<Float3_,Float_>	mCalcV[4];

public:
	PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &globalSymbols );
	PatchBicubic( ParamList &params,
							const Float3 hull[16],
						    const Attributes &attr,
							const SymbolList &globalSymbols );

		PatchBicubic *Clone() const {	return DNEW PatchBicubic( *this ); }

		void MakeBound( Bound &out_bound, Float3_ *out_pPo ) const
		{
			MakeBoundFromUVRangeN<SimplePrimitiveBase,2>( *this, out_bound, out_pPo );
		}

		void Eval_dPdu_dPdv(
						const Float2_ &uv,
						Float3_ &out_pt,
						Float3_ *out_dPdu,
						Float3_ *out_dPdv ) const;

private:
	void setupEvalCalc();
};

//==================================================================
/// PatchMesh
//==================================================================
class PatchMesh : public ComplexPrimitiveBase
{
public:
	const Symbol*	mpPatchType;
	ParamList		mParams;

public:
	PatchMesh(RtToken type,
			  ParamList &params,
			  const SymbolList &globalSymbols );

		void Simplify( Hider &hider );
};

//==================================================================
}

#endif