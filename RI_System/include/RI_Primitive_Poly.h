/*
 *  RI_Primitive_Poly.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/27.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PRIMITIVE_POLY_H
#define RI_PRIMITIVE_POLY_H

#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

//==================================================================
/// PatchBilinear
//==================================================================
class Polygon : public Primitive
{
public:
	//ParamList		mParams;
	DVec<Vector3>	mVertsP;

public:
	Polygon( ParamList &params, const SymbolList &staticSymbols );
	Polygon( ParamList &params, const Vector3 *pVertsP, int vertsN );

	void Render( GState &gstate );
};

//==================================================================
}

#endif