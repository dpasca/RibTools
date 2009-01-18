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
class Cylinder : public Primitive
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Cylinder( float radius, float zmin, float zmax, float thetamax ) :
		Primitive(CYLINDER),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}
	
	void Render( GState &gstate );
};

//==================================================================
/// Cone
//==================================================================
class Cone : public Primitive
{
public:
	float	mHeight;
	float	mRadius;
	float	mThetamaxRad;

public:
	Cone( float height, float radius, float thetamax ) :
		Primitive(CONE),
		mHeight(height),
		mRadius(radius),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
/// Sphere
//==================================================================
class Sphere : public Primitive
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Sphere( float radius, float zmin, float zmax, float thetamax ) :
		Primitive(SPHERE),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
/// Hyperboloid
//==================================================================
class Hyperboloid : public Primitive
{
public:
	Vector3	mP1;
	Vector3	mP2;
	float	mThetamaxRad;

public:
	Hyperboloid( const Vector3 &p1, const Vector3 &p2, float thetamax ) :
		Primitive(HYPERBOLOID),
		mP1(p1),
		mP2(p2),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
/// Paraboloid
//==================================================================
class Paraboloid : public Primitive
{
public:
	float	mRmax;
	float	mZmin;
	float	mZmax;
	float	mThetamaxRad;

public:
	Paraboloid( float rmax, float zmin, float zmax, float thetamax ) :
		Primitive(PARABOLOID),
		mRmax(rmax),
		mZmin(zmin),
		mZmax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
/// Torus
//==================================================================
class Torus : public Primitive
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
		Primitive(TORUS),
		mMaxRadius(maxRadius),
		mMinRadius(minRadius),
		mPhiminRad(phimin*DEG2RAD),
		mPhimaxRad(phimax*DEG2RAD),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
}

#endif