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
public:
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
/// PatchBicubic
//==================================================================
class PatchBicubic : public DiceablePrim
{
public:
	ParamList		mParams;
	const RtBasis	*mpUBasis;
	const RtBasis	*mpVBasis;
	Vector3			mHullPos[16];

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