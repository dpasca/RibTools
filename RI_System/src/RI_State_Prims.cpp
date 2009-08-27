//==================================================================
/// RI_State_Prims.cpp
///
/// Created by Davide Pasca - 2009/01/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_State.h"
#include "RI_Primitive.h"

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
void State::Hyperboloid( const Vec3f &p1, const Vec3f &p2, float thetamax )
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
	const SlSymbol*	pyPatchType = mStatics.LookupVoid( type );
	
	if ( pyPatchType->IsNameI( RI_BICUBIC ) )
		insertPrimitive( DNEW RI::PatchBicubic( params, mAttributesStack.top(), mStatics ) );
	else
	if ( pyPatchType->IsNameI( RI_BILINEAR ) )
		insertPrimitive( DNEW RI::PatchBilinear( params, mStatics ) );
}

//==================================================================
void State::PatchMesh( RtToken type, ParamList &params )
{
	insertPrimitive( DNEW RI::PatchMesh( type, params, mStatics ) );
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
	insertPrimitive( DNEW RI::Polygon( params, mStatics ) );
}

//==================================================================
void State::PointsGeneralPolygons( ParamList &params )
{
	insertPrimitive( DNEW RI::PointsGeneralPolygons( params, mStatics ) );
}
	
//==================================================================
}
