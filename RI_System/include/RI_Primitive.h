/*
 *  RI_Primivie.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/04.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PRIMITIVE_H
#define RI_PRIMITIVE_H

#include "DMath.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;

//==================================================================
/// GState
//==================================================================
class GState
{
	Matrix44	mMtxLocalHomo;
	float		mHalfXRes;
	float		mHalfYRes;

public:
	//==================================================================
	GState( const Options		&opt,
			const Attributes	&attr,
			const Transform		&xform );

	inline void AddVertex( float x, float y, float z, float r, float g, float b );
};

//==================================================================
/// Primitive
//==================================================================
class Primitive
{
public:
	enum Type
	{
		CONE,
		CYLINDER,
		SPHERE
	};
	
	Type	mType;
	
public:
	Primitive( Type type ) :
		mType(type)
	{
	}
	
	virtual ~Primitive()
	{
	}
	
	virtual void Render( GState &gstate )
	{
	}
};

//==================================================================
/// Cylinder
//==================================================================
class Cylinder : public Primitive
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamax;

public:
	Cylinder( float radius, float zmin, float zmax, float thetamax ) :
		Primitive(CYLINDER),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamax(thetamax)
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
	float	mThetamax;

public:
	Cone( float height, float radius, float thetamax ) :
		Primitive(CONE),
		mHeight(height),
		mRadius(radius),
		mThetamax(thetamax)
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
	float	mThetamax;

public:
	Sphere( float radius, float zmin, float zmax, float thetamax ) :
		Primitive(SPHERE),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamax(thetamax)
	{
	}

	void Render( GState &gstate );
};

//==================================================================
}

#endif