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
	ParamList		mParams;

public:
	Polygon( ParamList &params, const SymbolList &staticSymbols );

		bool IsSplitable() const	{	return true; }
		void Split( Framework &fwork );

	//void Render( GState &gstate );

private:
	void splitAddTriangle(
				Framework &fwork,
				const Vector3 &v1,
				const Vector3 &v2,
				const Vector3 &v3
				);

};

//==================================================================
}

#endif