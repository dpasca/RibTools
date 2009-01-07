/*
 *  RI_State_Prims.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/08.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_State.h"
#include "RI_Primitive.h"

//==================================================================
namespace RI
{

//==================================================================
void State::Cylinder( float radius, float zmin, float zmax, float thetamax )
{
	mFramework.Insert( new RI::Cylinder( radius, zmin, zmax, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Cone( float height, float radius, float thetamax )
{
	mFramework.Insert( new RI::Cone( height, radius, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Sphere( float radius, float zmin, float zmax, float thetamax )
{
	mFramework.Insert( new RI::Sphere( radius, zmin, zmax, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Hyperboloid( const Vector3 &p1, const Vector3 &p2, float thetamax )
{
	mFramework.Insert( new RI::Hyperboloid( p1, p2, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Torus( float maxRadius, float minRadius,
				   float phimin, float phimax,
				   float thetamax )
{
	mFramework.Insert(
		new RI::Torus( maxRadius, minRadius, phimin, phimax, thetamax ),
					  mOptionsStack.top(),
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
}
