/*
 *  RI_Primitive_Poly.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/04.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLUT/glut.h>
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive_Patch.h"

//==================================================================
namespace RI
{

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const SymbolList &staticSymbols ) :
	Primitive(PATCHBILINEAR),
	mParams(params)
{
	bool	gotP = ParamsFindP( params, staticSymbols, mHullPos, 4 );
	
	DASSTHROW( gotP, ("Missing hull parameter") );
}

//==================================================================
PatchBilinear::PatchBilinear( ParamList &params, const Vector3 hull[4] ) :
	Primitive(PATCHBILINEAR),
	mParams(params)
{
	for (int i=0; i < 4; ++i)
		mHullPos[i] = hull[i];
}

//==================================================================
void PatchBilinear::Render( GState &gstate )
{
	PUTPRIMNAME( "* PatchBilinear" );

	GVert	buffer[NSUBDIVS+1];
		
	//glBegin( GL_TRIANGLE_STRIP );
	for (int vI=0; vI <= NSUBDIVS; ++vI)
	{
		float	v = vI / (float)NSUBDIVS;

		Vector3	left = mHullPos[0] + v * (mHullPos[2] - mHullPos[0]);
		Vector3	right= mHullPos[1] + v * (mHullPos[3] - mHullPos[1]);

		glBegin( GL_TRIANGLE_STRIP );
		for (int uI=0; uI <= NSUBDIVS; ++uI)
		{
			float	u = uI / (float)NSUBDIVS;

			Vector3	P = left + u * (right - left);
			
			GVert	vert;
			vert.x	= P.x;
			vert.y	= P.y;
			vert.z	= P.z;

			vert.u	= u;
			vert.v	= v;

			if ( vI > 0 )
			{
				gstate.AddVertex( buffer[uI] );
				gstate.AddVertex( vert );
			}

			buffer[uI] = vert;
		}
		glEnd();
	}
	//glEnd();
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params, const Attributes &attr, const SymbolList &staticSymbols ) :
	Primitive(PATCHBICUBIC),
	mParams(params)
{
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();

	bool	gotP = ParamsFindP( params, staticSymbols, mHullPos, 16 );
	
	DASSTHROW( gotP, ("Missing hull parameter") );
}

//==================================================================
PatchBicubic::PatchBicubic( ParamList &params,
							const Vector3 hull[16],
						    const Attributes &attr,
							const SymbolList &staticSymbols ) :
	Primitive(PATCHBICUBIC),
	mParams(params)
{
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();

	for (int i=0; i < 16; ++i)
		mHullPos[i] = hull[i];
}

//==================================================================
static Vector3 spline( float t,
					const RtBasis &b,
					const Vector3 &p0,
					const Vector3 &p1,
					const Vector3 &p2,
					const Vector3 &p3 )
{
	return
		 (b.u.m44[0][0]*p0 + b.u.m44[0][1]*p1 + b.u.m44[0][2]*p2 + b.u.m44[0][3]*p3)*t*t*t
		+(b.u.m44[1][0]*p0 + b.u.m44[1][1]*p1 + b.u.m44[1][2]*p2 + b.u.m44[1][3]*p3)*t*t
		+(b.u.m44[2][0]*p0 + b.u.m44[2][1]*p1 + b.u.m44[2][2]*p2 + b.u.m44[2][3]*p3)*t
		+(b.u.m44[3][0]*p0 + b.u.m44[3][1]*p1 + b.u.m44[3][2]*p2 + b.u.m44[3][3]*p3);
}

//==================================================================
void PatchBicubic::Render( GState &gstate )
{
	PUTPRIMNAME( "* PatchBicubic" );

	GVert	buffer[NSUBDIVS+1];

	Vector3	bottom[NSUBDIVS+1];
	Vector3	mid1[NSUBDIVS+1];
	Vector3	mid2[NSUBDIVS+1];
	Vector3	top[NSUBDIVS+1];
	
	const RtBasis	&uBasis = *mpUBasis;
	const RtBasis	&vBasis = *mpVBasis;

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		bottom[uI]	= spline(u, uBasis, mHullPos[ 0], mHullPos[ 1], mHullPos[ 2], mHullPos[ 3]);
		mid1[uI]	= spline(u, uBasis, mHullPos[ 4], mHullPos[ 5], mHullPos[ 6], mHullPos[ 7]);
		mid2[uI]	= spline(u, uBasis, mHullPos[ 8], mHullPos[ 9], mHullPos[10], mHullPos[11]);
		top[uI]		= spline(u, uBasis, mHullPos[12], mHullPos[13], mHullPos[14], mHullPos[15]);
	}
		
	//glBegin( GL_TRIANGLE_STRIP );
	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		glBegin( GL_TRIANGLE_STRIP );
		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			Vector3	P = spline( v, vBasis, bottom[uI], mid1[uI], mid2[uI], top[uI] );
			
			GVert	vert;
			vert.x	= P.x;
			vert.y	= P.y;
			vert.z	= P.z;

			vert.u	= u;
			vert.v	= v;

			if ( uI > 0 )
			{
				gstate.AddVertex( buffer[vI] );
				gstate.AddVertex( vert );
			}

			buffer[vI] = vert;
		}
		glEnd();
	}
	//glEnd();
}

//==================================================================
}