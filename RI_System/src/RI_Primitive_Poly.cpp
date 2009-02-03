/*
 *  RI_Primitive_Poly.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/27.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLUT/glut.h>
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive_Poly.h"
#include "RI_Primitive_Patch.h"
#include "RI_Framework.h"

//==================================================================
namespace RI
{

//==================================================================
Polygon::Polygon( ParamList &params, const SymbolList &staticSymbols ) :
	Primitive(POLYGON)
	, mParams(params)
{
}

//==================================================================
void Polygon::splitAddTriangle(
				Framework &fwork,
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
void Polygon::Split( Framework &fwork )
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

/*
//==================================================================
void Polygon::Render( GState &gstate )
{
	PUTPRIMNAME( "* Polygon" );
	
	glBegin( GL_POLYGON );
	for (size_t i=0; i < mVertsP.size(); ++i)
	{
		GVert	gvert;
		
		gvert.x = mVertsP[i].x;
		gvert.y = mVertsP[i].y;
		gvert.z = mVertsP[i].z;
		
		gvert.u	= i * 0.5f;
		gvert.v = i * 0.05f;

		gstate.AddVertex( gvert );
	}

	glEnd();
}
*/

//==================================================================
}