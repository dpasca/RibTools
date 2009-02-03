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
inline void State::insertPrimitive( Primitive *pPrim )
{
	mFramework.Insert( pPrim,
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Cylinder( float radius, float zmin, float zmax, float thetamax )
{
	insertPrimitive( new RI::Cylinder( radius, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Cone( float height, float radius, float thetamax )
{
	insertPrimitive( new RI::Cone( height, radius, thetamax ) );
}

//==================================================================
void State::Sphere( float radius, float zmin, float zmax, float thetamax )
{
	insertPrimitive( new RI::Sphere( radius, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Hyperboloid( const Vector3 &p1, const Vector3 &p2, float thetamax )
{
	insertPrimitive( new RI::Hyperboloid( p1, p2, thetamax ) );
}

//==================================================================
void State::Paraboloid( float rmax, float zmin, float zmax, float thetamax )
{
	insertPrimitive( new RI::Paraboloid( rmax, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Torus( float maxRadius, float minRadius,
				   float phimin, float phimax,
				   float thetamax )
{
	insertPrimitive( new RI::Torus( maxRadius, minRadius, phimin, phimax, thetamax ) );
}

//==================================================================
void State::Patch( RtToken type, ParamList &params )
{
	CPSymVoid	pyPatchType = mStatics.FindVoid( type );
	
	if ( pyPatchType->IsNameI( RI_BICUBIC ) )
		insertPrimitive( new RI::PatchBicubic( params, mAttributesStack.top(), mStatics ) );
	else
	if ( pyPatchType->IsNameI( RI_BILINEAR ) )
		insertPrimitive( new RI::PatchBilinear( params, mStatics ) );
}

//==================================================================
void State::PatchMesh( RtToken type, ParamList &params )
{
	insertPrimitive( new RI::PatchMesh( type, params, mStatics ) );
}

//==================================================================
void State::Polygon( ParamList &params )
{
	insertPrimitive( new RI::Polygon( params, mStatics ) );
}

	
//==================================================================
}
