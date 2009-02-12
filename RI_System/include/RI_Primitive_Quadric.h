/*
 *  RI_Primitive_Quadric.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/18.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PRIMITIVE_QUADRIC_H
#define RI_PRIMITIVE_QUADRIC_H

#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

//==================================================================
/// Cylinder
//==================================================================
class Cylinder : public DiceablePrim
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Cylinder( float radius, float zmin, float zmax, float thetamax ) :
		DiceablePrim(CYLINDER),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

		void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
/// Cone
//==================================================================
class Cone : public DiceablePrim
{
public:
	float	mHeight;
	float	mRadius;
	float	mThetamaxRad;

public:
	Cone( float height, float radius, float thetamax ) :
		DiceablePrim(CONE),
		mHeight(height),
		mRadius(radius),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

		void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
/// Sphere
//==================================================================
class Sphere : public DiceablePrim
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Sphere( float radius, float zmin, float zmax, float thetamax ) :
		DiceablePrim(SPHERE),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

		void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
/// Hyperboloid
//==================================================================
class Hyperboloid : public DiceablePrim
{
public:
	Vector3	mP1;
	Vector3	mP2;
	float	mThetamaxRad;

public:
	Hyperboloid( const Vector3 &p1, const Vector3 &p2, float thetamax ) :
		DiceablePrim(HYPERBOLOID),
		mP1(p1),
		mP2(p2),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

	void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
/// Paraboloid
//==================================================================
class Paraboloid : public DiceablePrim
{
public:
	float	mRmax;
	float	mZmin;
	float	mZmax;
	float	mThetamaxRad;

public:
	Paraboloid( float rmax, float zmin, float zmax, float thetamax ) :
		DiceablePrim(PARABOLOID),
		mRmax(rmax),
		mZmin(zmin),
		mZmax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

		void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
/// Torus
//==================================================================
class Torus : public DiceablePrim
{
public:
	float	mMinRadius;
	float	mMaxRadius;
	float	mPhiminRad;
	float	mPhimaxRad;
	float	mThetamaxRad;

public:
	Torus( float maxRadius, float minRadius,
		   float phimin, float phimax,
		   float thetamax ) :
		DiceablePrim(TORUS),
		mMaxRadius(maxRadius),
		mMinRadius(minRadius),
		mPhiminRad(phimin*DEG2RAD),
		mPhimaxRad(phimax*DEG2RAD),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}
	
		void MakeBound( Bound &out_bound ) { out_bound.Reset(); }

		void EvalP( float uGrid,
					float vGrid,
					Point3 &out_pt,
					const Matrix44 &mtxObjectCurrent ) const;

	void Render( GState &gstate );
};

//==================================================================
}

#endif