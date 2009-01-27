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

//==================================================================
namespace RI
{

//==================================================================
Polygon::Polygon( ParamList &params, const SymbolList &staticSymbols ) :
	Primitive(POLYGON)
	//, mParams(params)
{
	bool	gotP = ParamsFindP( params, staticSymbols, mVertsP );

	DASSTHROW( gotP, ("No 'P' token found !") );
}

//==================================================================
Polygon::Polygon( ParamList &params, const Vector3 *pVertsP, int vertsN ) :
	Primitive(POLYGON)
	//, mParams(params)
{
	mVertsP.resize( vertsN );
	
	for (size_t i=0; i < vertsN; ++i)
		mVertsP[i] = pVertsP[i];
}

//==================================================================
void Polygon::Render( GState &gstate )
{
	PUTPRIMNAME( "* Polygon" );

}

//==================================================================
}