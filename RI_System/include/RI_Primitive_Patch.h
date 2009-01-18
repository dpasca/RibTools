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
class PatchBilinear : public Primitive
{
public:
	ParamList		mParams;
	Vector3			mHullPos[4];

public:
	PatchBilinear( ParamList &params, const SymbolList &staticSymbols );
	PatchBilinear( ParamList &params, const Vector3 hull[4] );

	void Render( GState &gstate );
};

//==================================================================
/// PatchBicubic
//==================================================================
class PatchBicubic : public Primitive
{
public:
	ParamList		mParams;
	const RtBasis	*mpUBasis;
	const RtBasis	*mpVBasis;
	int				mUSteps;
	int				mVSteps;
	Vector3			mHullPos[16];

public:
	PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols );

	void Render( GState &gstate );
};

//==================================================================
}

#endif