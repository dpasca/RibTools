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

	//printf( "  vtx-scr: %f %f %f\n", sx, sy, sz );
	
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
			float	theta = u * mThetamaxRad;
			float	x = mRadius * cosf( theta );
			float	y = mRadius * sinf( theta );
			float	z = mZMin + v * (mZMax - mZMin);

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
			float	theta = u * mThetamaxRad;
			float	x = mRadius * (1 - v) * cosf( theta );
			float	y = mRadius * (1 - v) * sinf( theta );
			float	z = v * mHeight;

			gstate.AddVertex( x, y, z, u, v, 0 );
		}
	}
	
	glEnd();
}

//==================================================================
void Sphere::Render( GState &gstate )
{
	puts( "* Sphere" );
	
	glBegin( GL_TRIANGLE_STRIP );

	float	alphamin	= asinf( mZMin / mRadius );
	float	alphadelta	= asinf( mZMax / mRadius ) - alphamin;

	float	buffer[NSUBDIVS+1][6];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	alpha = alphamin + v * alphadelta;
			float	theta = u * mThetamaxRad;

			float	x = mRadius * cosf( alpha ) * cosf( theta );
			float	y = mRadius * cosf( alpha ) * sinf( theta );
			float	z = mRadius * sinf( alpha );

			if ( uI > 0 )
			{
				gstate.AddVertex(
					buffer[vI][0],
					buffer[vI][1],
					buffer[vI][2],
					buffer[vI][3],
					buffer[vI][4],
					buffer[vI][5]
				);

				gstate.AddVertex( x, y, z, u, v, 0 );
			}

			buffer[vI][0] = x;
			buffer[vI][1] = y;
			buffer[vI][2] = z;
			buffer[vI][3] = u;
			buffer[vI][4] = v;
			buffer[vI][5] = 0;
		}
	}
	
	glEnd();
}


//==================================================================
}