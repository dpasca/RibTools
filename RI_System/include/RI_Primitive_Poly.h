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

class HiderREYES;

//==================================================================
/// Polygon
//==================================================================
class Polygon : public ComplexPrimitiveBase
{
public:
	ParamList		mParams;

public:
	Polygon( ParamList &params, const SymbolList &staticSymbols );

		void Simplify( HiderREYES &hider );

private:
	void simplifyAddTriangle(
				HiderREYES &fwork,
				const Vec3f &v1,
				const Vec3f &v2,
				const Vec3f &v3
				);

};

//==================================================================
/// PointsGeneralPolygons
//==================================================================
class PointsGeneralPolygons : public ComplexPrimitiveBase
{
public:
	ParamList		mParams;

public:
	PointsGeneralPolygons( ParamList &params, const SymbolList &staticSymbols );

		void Simplify( HiderREYES &hider );

private:
	void simplifyAddTriangle(
				HiderREYES &hider,
				const Vec3f &v1,
				const Vec3f &v2,
				const Vec3f &v3
				);

};

//==================================================================
}

#endif