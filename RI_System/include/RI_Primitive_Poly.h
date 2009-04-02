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
/// Polygon
//==================================================================
class Polygon : public Primitive
{
public:
	ParamList		mParams;

public:
	Polygon( ParamList &params, const SymbolList &staticSymbols );

		Polygon *Clone() const {	DASSERT( 0 ); return NULL; }

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }
		bool IsSplitable() const	{	return true; }
		void Split( FrameworkBase &fwork, bool uSplit, bool vSplit );

		void	Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const {}

private:
	void splitAddTriangle(
				FrameworkBase &fwork,
				const Vector3 &v1,
				const Vector3 &v2,
				const Vector3 &v3
				);

};

//==================================================================
/// PointsGeneralPolygons
//==================================================================
class PointsGeneralPolygons : public Primitive
{
public:
	ParamList		mParams;

public:
	PointsGeneralPolygons( ParamList &params, const SymbolList &staticSymbols );

		PointsGeneralPolygons *Clone() const {	DASSERT( 0 ); return NULL; }

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }
		bool IsSplitable() const	{	return true; }
		void Split( FrameworkBase &fwork, bool uSplit, bool vSplit );

		void	Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const {}

private:
	void splitAddTriangle(
				FrameworkBase &fwork,
				const Vector3 &v1,
				const Vector3 &v2,
				const Vector3 &v3
				);

};

//==================================================================
}

#endif