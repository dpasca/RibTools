//==================================================================
/// RI_Primitive_Poly.h
///
/// Created by Davide Pasca - 2009/1/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_POLY_H
#define RI_PRIMITIVE_POLY_H

#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

class Hider;

//==================================================================
/// Polygon
//==================================================================
class Polygon : public ComplexPrimitiveBase
{
public:
	ParamList		mParams;

public:
	Polygon( ParamList &params, const SymbolList &globalSymbols );

		void Simplify( Hider &hider );
};

//==================================================================
/// PointsPolygons
//==================================================================
class PointsPolygons : public ComplexPrimitiveBase
{
public:
	ParamList		mParams;

public:
	PointsPolygons( ParamList &params, const SymbolList &globalSymbols );

		void Simplify( Hider &hider );
};

//==================================================================
/// PointsGeneralPolygons
//==================================================================
class PointsGeneralPolygons : public ComplexPrimitiveBase
{
public:
	ParamList		mParams;

public:
	PointsGeneralPolygons( ParamList &params, const SymbolList &globalSymbols );

		void Simplify( Hider &hider );
};

//==================================================================
}

#endif