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
static int findParam( const char *pFindName, u_int expectedType, int fromIdx, ParamList &params )
{
	DASSERT( ((params.size()-fromIdx) & 1) == 0 );

	if NOT( ((params.size()-fromIdx) & 1) == 0 )
		return -1;

	for (int i=fromIdx; i < (int)params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		if ( 0 == strcasecmp( pFindName, params[i] ) )
		{
			if ( params[i+1].type == expectedType )
				return i+1;
			else
				return -1;
		}
	}

	return -1;
}

//==================================================================
void State::PatchMesh( RtToken type, ParamList &params )
{
	CPSymVoid	pyPatchType = mStatics.FindVoid( type );

	// PatchMesh "bilinear" 2 "nonperiodic" 5 "nonperiodic" "P"  [ -0.995625 2 -0.495465 ...
	//               0      1       2       3       4        5     6

	int			nu		= params[1];
	CPSymVoid	pyUWrap = mStatics.FindVoid( params[2] );
	int			nv		= params[3];
	CPSymVoid	pyVWrap = mStatics.FindVoid( params[4] );

	bool	uPeriodic = pyUWrap->IsNameI( RI_PERIODIC );
	bool	vPeriodic = pyVWrap->IsNameI( RI_PERIODIC );
		
	int	PValuesParIdx = findParam( "P", Param::FLT_ARR, 5, params );
	if ( PValuesParIdx == -1 )
		return;
	
	int			meshHullSize = 3 * nu * nv;
	const float	*pMeshHull = params[PValuesParIdx].PFlt( meshHullSize );

	if ( pyPatchType->IsNameI( RI_BILINEAR ) )
	{
		int	nUPatches = nu - 1 + uPeriodic ? 1 : 0;
		int	nVPatches = nv - 1 + vPeriodic ? 1 : 0;

		Vector3	hullv3[4];

		for (int i=0; i < nUPatches; ++i)
		{
			int	ii = (i+1) % nu;

			for (int j=0; j < nVPatches; ++j)
			{
				int	jj = (j+1) % nv;

				hullv3[0] = Vector3( &pMeshHull[(i +nu*j )*3] );
				hullv3[1] = Vector3( &pMeshHull[(ii+nu*j )*3] );
				hullv3[2] = Vector3( &pMeshHull[(i +nu*jj)*3] );
				hullv3[3] = Vector3( &pMeshHull[(ii+nu*jj)*3] );

				insertPrimitive(
						new RI::PatchBilinear( params, hullv3 ) );
			}
		}
	}
	else
	if ( pyPatchType->IsNameI( RI_BICUBIC ) )
	{
		const Attributes	&attr = mAttributesStack.top();
		
		int uSteps = attr.mUSteps;
		int vSteps = attr.mVSteps;

		DASSTHROW( uSteps >= 1 && uSteps <= 16, ("Invalid uSteps %i", uSteps) );
		DASSTHROW( vSteps >= 1 && vSteps <= 16, ("Invalid vSteps %i", vSteps) );
		
		int	nUPatches = uPeriodic ? nu / uSteps : (nu-4) / uSteps + 1;
		int	nVPatches = vPeriodic ? nv / vSteps : (nv-4) / vSteps + 1;

		Vector3	hullv3[16];
		
		for (int j0=0; j0 < nVPatches; ++j0)
		{
			for (int i0=0; i0 < nUPatches; ++i0)
			{
				int	hidx = 0;

				for (int j=0; j < 4; ++j)
				{
					for (int i=0; i < 4; ++i)
					{
						int	srcIdx = ((i + i0 * uSteps) % nu) +
									 ((j + j0 * vSteps) % nv) * nu;

						DASSERT( srcIdx >= 0 && (srcIdx*3) < meshHullSize );

						hullv3[hidx++] = Vector3( &pMeshHull[ srcIdx * 3 ]);
					}
				}

				insertPrimitive( new RI::PatchBicubic( params, hullv3, attr, mStatics ) );
			}
		}
	}
	else
	{
		DASSTHROW( 0, ("Unrecognized Patch type %s", pyPatchType->pName ) );
		ErrHandler( E_BADARGUMENT );
	}
}

//==================================================================
void State::Polygon( ParamList &params )
{
	insertPrimitive( new RI::Polygon( params, mStatics ) );
}

	
//==================================================================
}
