/*
 *  Engine1.cpp
 *  Polygoner1
 *
 *  Created by Davide Pasca on 09/02/22.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "Engine1.h"

//===============================================================
static const u_int	CCODE_LFT = 0x01;
static const u_int	CCODE_RGT = 0x02;
static const u_int	CCODE_TOP = 0x04;
static const u_int	CCODE_BOT = 0x08;
static const u_int	CCODE_NEA = 0x10;
static const u_int	CCODE_FAR = 0x20;

//===============================================================
void RendMesh( const Mesh &mesh, const Matrix44 &mtxProjLocal )
{
	const float *pPos		= &mesh.mVerts[Mesh::VT_POS][0];
	size_t		 posSize	= mesh.mVerts[Mesh::VT_POS].size();
	for (size_t i=0; i < posSize; i += 3)
	{
		Vector4	posProj =
			MultiplyV4M( mtxProjLocal, Vector4( pPos[i+0], pPos[i+1], pPos[i+2], 1 ) );
		
		u_int	ccode = 0;
		if ( posProj.x < -posProj.w )	ccode |= CCODE_LFT;
		if ( posProj.x >  posProj.w )	ccode |= CCODE_RGT;
		if ( posProj.y < -posProj.w )	ccode |= CCODE_BOT;
		if ( posProj.y >  posProj.w )	ccode |= CCODE_TOP;
		if ( posProj.z <  0			)	ccode |= CCODE_NEA;
		if ( posProj.z >  posProj.w )	ccode |= CCODE_FAR;
		
	}
}
