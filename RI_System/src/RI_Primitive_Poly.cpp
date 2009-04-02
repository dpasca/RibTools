/*
 *  RI_Primitive_Poly.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/27.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive_Poly.h"
#include "RI_Primitive_Patch.h"
#include "RI_FrameworkBase.h"

//==================================================================
namespace RI
{

//==================================================================
/// Polygon
//==================================================================
Polygon::Polygon( ParamList &params, const SymbolList &staticSymbols ) :
	Primitive(POLYGON)
	, mParams(params)
{
}

//==================================================================
void Polygon::splitAddTriangle(
				FrameworkBase &fwork,
				const Vector3 &v1,
				const Vector3 &v2,
				const Vector3 &v3
				)
{
	Vector3	mid = (v1 + v2 + v3) * (1.0f/3);
	Vector3	a	= (v1 + v2) * 0.5f;
	Vector3	b	= (v2 + v3) * 0.5f;
	Vector3	c	= (v1 + v3) * 0.5f;

	Vector3	patchVerts[4];
	patchVerts[0] = v1;
	patchVerts[1] = a;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
	patchVerts[0] = v2;
	patchVerts[1] = a;
	patchVerts[2] = b;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
	patchVerts[0] = v3;
	patchVerts[1] = b;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
}

//==================================================================
void Polygon::Split( FrameworkBase &fwork, bool uSplit, bool vSplit )
{
	int	PValuesParIdx = findParam( "P", Param::FLT_ARR, 0, mParams );
	if ( PValuesParIdx == -1 )
	{
		DASSTHROW( 0, ("Missing 'P'") );
		return;
	}

	const FltVec	&paramP = mParams[PValuesParIdx].NumVec();
	
	int	last	= (int)paramP.size()/3 - 1;
	int	start	= 1;
	int	end		= DMIN( (int)3, last );
	
	Vector3	patchVerts[4];
	
	while ( (end - start) == 2 )
	{
		patchVerts[0].Set( &paramP[3 * 0] );
		patchVerts[1].Set( &paramP[3 * start] );
		patchVerts[2].Set( &paramP[3 * end] );
		patchVerts[3].Set( &paramP[3 * (start+1)] );

		fwork.InsertSplitted(
				new PatchBilinear( mParams, patchVerts ),
				*this );
		
		start	= end;
		end		= DMIN( start+2, last );
	}

	if ( (end - start) == 1 )
	{	
		splitAddTriangle(
						fwork,
						Vector3( &paramP[3 * 0] ),
						Vector3( &paramP[3 * start] ),
						Vector3( &paramP[3 * end] )
					);
	}
}

//==================================================================
/// 
//==================================================================
PointsGeneralPolygons::PointsGeneralPolygons( ParamList &params, const SymbolList &staticSymbols ) :
	Primitive(POINTSGENERALPOLYGONS)
	, mParams(params)
{
}

//==================================================================
void PointsGeneralPolygons::splitAddTriangle(
				FrameworkBase &fwork,
				const Vector3 &v1,
				const Vector3 &v2,
				const Vector3 &v3
				)
{
	Vector3	mid = (v1 + v2 + v3) * (1.0f/3);
	Vector3	a	= (v1 + v2) * 0.5f;
	Vector3	b	= (v2 + v3) * 0.5f;
	Vector3	c	= (v1 + v3) * 0.5f;

	Vector3	patchVerts[4];
	patchVerts[0] = v1;
	patchVerts[1] = a;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
	patchVerts[0] = v2;
	patchVerts[1] = a;
	patchVerts[2] = b;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
	patchVerts[0] = v3;
	patchVerts[1] = b;
	patchVerts[2] = c;
	patchVerts[3] = mid;
	fwork.InsertSplitted( new PatchBilinear( mParams, patchVerts ), *this );
}

//==================================================================
static const size_t MAX_VERT_INFO = 32;
struct VertInfo
{
	const char	*pName;
	int			parIdx;
};

//==================================================================
void PointsGeneralPolygons::Split( FrameworkBase &fwork, bool uSplit, bool vSplit )
{
	size_t		nloopsN = mParams[0].IntArrSize();
	const int	*pNLoops = mParams[0].PInt();

	size_t		nvertsN = mParams[1].IntArrSize();
	const int	*pNVerts = mParams[1].PInt();

	size_t		vertsN = mParams[2].IntArrSize();
	const int	*pVerts = mParams[2].PInt();

	VertInfo	vinfo[MAX_VERT_INFO];
	size_t		vinfoN = 0;

	int	PValuesParIdx = -1;

	for (size_t pi=3; pi < mParams.size(); ++pi)
	{
		if NOT( mParams[pi].IsString() )
		{
			DASSTHROW( 0, ("Expecting parameter name !") );
			return;
		}

		const char *pNameParam = mParams[pi].PChar();

		if ( 0 == strcasecmp( pNameParam, "P" ) )
		{
			PValuesParIdx = pi+1;
		}

		vinfo[vinfoN].pName	= pNameParam;
		vinfo[vinfoN].parIdx= pi + 1;
		++vinfoN;
	}

	if ( PValuesParIdx == -1 )
	{
		DASSTHROW( 0, ("Missing 'P'") );
		return;
	}

	const FltVec	&paramP = mParams[PValuesParIdx].NumVec();
	
	int	last	= (int)paramP.size()/3 - 1;
	int	start	= 1;
	int	end		= DMIN( (int)3, last );
	
	Vector3	patchVerts[4];
	
	while ( (end - start) == 2 )
	{
		patchVerts[0].Set( &paramP[3 * 0] );
		patchVerts[1].Set( &paramP[3 * start] );
		patchVerts[2].Set( &paramP[3 * end] );
		patchVerts[3].Set( &paramP[3 * (start+1)] );

		fwork.InsertSplitted(
				new PatchBilinear( mParams, patchVerts ),
				*this );
		
		start	= end;
		end		= DMIN( start+2, last );
	}

	if ( (end - start) == 1 )
	{	
		splitAddTriangle(
						fwork,
						Vector3( &paramP[3 * 0] ),
						Vector3( &paramP[3 * start] ),
						Vector3( &paramP[3 * end] )
					);
	}
}

//==================================================================
}