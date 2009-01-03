/*
 *  RI_Primitive.cpp
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
#include "RI_Primitive.h"

const static int NSUBDIVS = 16;

//==================================================================
namespace RI
{

//==================================================================
GState::GState( const Options		&opt,
				const Attributes	&attr,
				const Transform		&xform )
{
	mMtxLocalHomo	= xform.mMatrix * opt.mMtxViewHomo;
	mHalfXRes		= opt.mXRes * 0.5f;
	mHalfYRes		= opt.mYRes * 0.5f;
}

//==================================================================
inline void GState::AddVertex( float x, float y, float z, float r, float g, float b )
{
	Vector4	homoPos = Vector3( x, y, z ) * mMtxLocalHomo;

	float	oow = 1.0f / homoPos.w;

	float sx = mHalfXRes + mHalfXRes * oow * homoPos.x;
	float sy = mHalfYRes - mHalfYRes * oow * homoPos.y;
	float sz = oow * homoPos.z;

	printf( "  vtx-scr: %f %f %f\n", sx, sy, sz );
	
	glColor3f( r, g, b );
	glVertex3f( sx, sy, sz );
}

//==================================================================
void Cylinder::Render( GState &gstate )
{
	puts( "* Cylinder" );
	
	glBegin( GL_TRIANGLE_STRIP );

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		for (float v=0; v <= 1.0f; v += 1.0f)
		{
			float	theta = u * mThetamax;
			float	x = mRadius * cosf( theta );
			float	y = mRadius * sinf( theta );
			float	z = v * (mZMax - mZMin);

			gstate.AddVertex( x, y, z, u, v, 0 );
		}
	}
	
	glEnd();
}

//==================================================================
void Cone::Render( GState &gstate )
{
	puts( "* Cone" );

	glBegin( GL_TRIANGLE_STRIP );

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		for (float v=0; v <= 1.0f; v += 1.0f)
		{
			float	theta = u * mThetamax;
			float	x = mRadius * (1 - v) * cosf( theta );
			float	y = mRadius * (1 - v) * sinf( theta );
			float	z = v * mHeight;

			gstate.AddVertex( x, y, z, u, v, 0 );
		}
	}
	
	glEnd();
}

//==================================================================
}