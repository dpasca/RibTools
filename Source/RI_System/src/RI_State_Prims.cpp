//==================================================================
/// RI_State_Prims.cpp
///
/// Created by Davide Pasca - 2009/01/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_State.h"
#include "RI_Primitive_NuPatch.h"
#include "RI_Primitive_Patch.h"
#include "RI_Primitive_Poly.h"
#include "RI_Primitive_Quadric.h"
#include "RI_Primitive_Poly.h"

//==================================================================
namespace RI
{

//==================================================================
inline void State::insertPrimitive( PrimitiveBase *pPrim )
{
	mParams.mpFramework->Insert( pPrim,
					  mAttributesStack.top(),
					  mTransformOpenStack.top() );
}

//==================================================================
void State::Cylinder( float radius, float zmin, float zmax, float thetamax )
{
	insertPrimitive( DNEW RI::Cylinder( radius, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Cone( float height, float radius, float thetamax )
{
	insertPrimitive( DNEW RI::Cone( height, radius, thetamax ) );
}

//==================================================================
void State::Sphere( float radius, float zmin, float zmax, float thetamax )
{
	insertPrimitive( DNEW RI::Sphere( radius, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Hyperboloid( const Float3 &p1, const Float3 &p2, float thetamax )
{
	insertPrimitive( DNEW RI::Hyperboloid( p1, p2, thetamax ) );
}

//==================================================================
void State::Paraboloid( float rmax, float zmin, float zmax, float thetamax )
{
	insertPrimitive( DNEW RI::Paraboloid( rmax, zmin, zmax, thetamax ) );
}

//==================================================================
void State::Torus( float maxRadius, float minRadius,
				   float phimin, float phimax,
				   float thetamax )
{
	insertPrimitive( DNEW RI::Torus( maxRadius, minRadius, phimin, phimax, thetamax ) );
}

//==================================================================
void State::Patch( RtToken type, ParamList &params )
{
	const Symbol*	pPatchType = mGlobalSyms.FindSymbol( type );
	
	if ( pPatchType->IsName( RI_BICUBIC ) )
		insertPrimitive( DNEW RI::PatchBicubic( params, mAttributesStack.top(), mGlobalSyms ) );
	else
	if ( pPatchType->IsName( RI_BILINEAR ) )
		insertPrimitive( DNEW RI::PatchBilinear( params, mGlobalSyms ) );
}

//==================================================================
void State::PatchMesh( RtToken type, ParamList &params )
{
	insertPrimitive( DNEW RI::PatchMesh( type, params, mGlobalSyms ) );
}

//==================================================================
void State::NuPatch(
			int		nu		,
			int		uorder	,
			const float	*pUknot	,
			float	umin	,
			float	umax	,
			int		nv		,
			int		vorder	,
			const float	*pVknot	,
			float	vmin	,
			float	vmax	,
			ParamList &params )
{
	insertPrimitive( DNEW RI::NuPatch(
								nu		,
								uorder	,
								pUknot	,
								umin	,
								umax	,
								nv		,
								vorder	,
								pVknot	,
								vmin	,
								vmax	,
								params
							) );
}

//==================================================================
void State::Polygon( ParamList &params )
{
	insertPrimitive( DNEW RI::Polygon( params, mGlobalSyms ) );
}

//==================================================================
void State::PointsPolygons( ParamList &params )
{
	insertPrimitive( DNEW RI::PointsPolygons( params, mGlobalSyms ) );
}
	
//==================================================================
void State::PointsGeneralPolygons( ParamList &params )
{
	insertPrimitive( DNEW RI::PointsGeneralPolygons( params, mGlobalSyms ) );
}
	
//==================================================================
}
