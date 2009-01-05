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
inline void GState::AddVertex( const GVert &vert )
{
	Vector4	homoPos = Vector3( vert.x, vert.y, vert.z ) * mMtxLocalHomo;

	float	oow = 1.0f / homoPos.w;

	float sx = mHalfXRes + mHalfXRes * oow * homoPos.x;
	float sy = mHalfYRes - mHalfYRes * oow * homoPos.y;
	float sz = oow * homoPos.z;

	//printf( "  vtx-scr: %f %f %f\n", sx, sy, sz );
	
	glColor3f( vert.u, vert.v, 0 );
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
			
			GVert	vert;

			vert.x = mRadius * cosf( theta );
			vert.y = mRadius * sinf( theta );
			vert.z = mZMin + v * (mZMax - mZMin);
			vert.u = u;
			vert.v = v;

			gstate.AddVertex( vert );
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
			GVert	vert;

			float	theta = u * mThetamaxRad;
			vert.x = mRadius * (1 - v) * cosf( theta );
			vert.y = mRadius * (1 - v) * sinf( theta );
			vert.z = v * mHeight;
			vert.u = u;
			vert.v = v;

			gstate.AddVertex( vert );
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

	GVert	buffer[NSUBDIVS+1];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	alpha = alphamin + v * alphadelta;
			float	theta = u * mThetamaxRad;

			GVert	vert;
			vert.x = mRadius * cosf( alpha ) * cosf( theta );
			vert.y = mRadius * cosf( alpha ) * sinf( theta );
			vert.z = mRadius * sinf( alpha );
			vert.u = u;
			vert.v = v;

			if ( uI > 0 )
			{
				gstate.AddVertex( buffer[vI] );
				gstate.AddVertex( vert );
			}

			buffer[vI] = vert;
		}
	}
	
	glEnd();
}

//==================================================================
void Hyperboloid::Render( GState &gstate )
{
	puts( "* Hyperboloid" );
	
	glBegin( GL_TRIANGLE_STRIP );

	GVert	buffer[NSUBDIVS+1];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	x = mP1.x + (mP2.x - mP1.x) * v;
			float	y = mP1.y + (mP2.y - mP1.y) * v;
			float	z = mP1.z + (mP2.z - mP1.z) * v;

			float theta = u * mThetamaxRad;

			GVert	vert;
			vert.x = x * cosf( theta ) - y * sinf( theta );
			vert.y = x * sinf( theta ) + y * cosf( theta );
			vert.z = z;
			vert.u = u;
			vert.v = v;

			if ( uI > 0 )
			{
				gstate.AddVertex( buffer[vI] );
				gstate.AddVertex( vert );
			}

			buffer[vI] = vert;
		}
	}

	glEnd();
}

//==================================================================
}