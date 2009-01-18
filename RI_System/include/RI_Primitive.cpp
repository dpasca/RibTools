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

#define PUTPRIMNAME	//puts

//==================================================================
namespace RI
{

//==================================================================
/// GState
//==================================================================
void GState::Setup(
				const Options		&opt,
				const Attributes	&attr,
				const Transform		&xform,
				const Matrix44		&mtxWorldCamera )
{
	//mpOpts	= &opt;
	//mpAttrs	= &attr;
	//,mpXForm(&xform)

	mMtxLocalHomo	= xform.GetMatrix() * mtxWorldCamera * opt.mMtxViewHomo;
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

	//printf( "  vtx-scr: %f %f %f %f\n", sx, sy, sz, oow );
	
	//glColor3f( (int)(vert.u*8)/8.0f + vert.v/8, 0, 0 );
	glColor3f( vert.u, vert.v, 0 );
	glVertex3f( sx, sy, oow );
}

//==================================================================
void Cylinder::Render( GState &gstate )
{
	PUTPRIMNAME( "* Cylinder" );
	
	glBegin( GL_TRIANGLE_STRIP );

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float	theta = u * mThetamaxRad;
		
		for (float v=0; v <= 1.0f; v += 1.0f)
		{
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
	PUTPRIMNAME( "* Cone" );

	glBegin( GL_TRIANGLE_STRIP );

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float	theta = u * mThetamaxRad;

		for (float v=0; v <= 1.0f; v += 1.0f)
		{
			GVert	vert;

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
	PUTPRIMNAME( "* Sphere" );
	
	glBegin( GL_TRIANGLE_STRIP );

	float	alphamin	= asinf( mZMin / mRadius );
	float	alphadelta	= asinf( mZMax / mRadius ) - alphamin;

	GVert	buffer[NSUBDIVS+1];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float	theta = u * mThetamaxRad;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	alpha = alphamin + v * alphadelta;

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
	PUTPRIMNAME( "* Hyperboloid" );
	
	glBegin( GL_TRIANGLE_STRIP );

	GVert	buffer[NSUBDIVS+1];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float theta = u * mThetamaxRad;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	x = mP1.x + (mP2.x - mP1.x) * v;
			float	y = mP1.y + (mP2.y - mP1.y) * v;
			float	z = mP1.z + (mP2.z - mP1.z) * v;

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
void Paraboloid::Render( GState &gstate )
{
	PUTPRIMNAME( "* Paraboloid" );

	glBegin( GL_TRIANGLE_STRIP );

	GVert	buffer[NSUBDIVS+1];

	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float theta = u * mThetamaxRad;

		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	z = (mZmax - mZmin) * v;

			float	r = mRmax * sqrtf( z / mZmax );

			GVert	vert;
			vert.x = r * cosf( theta );
			vert.y = r * sinf( theta );
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
void Torus::Render( GState &gstate )
{
	PUTPRIMNAME( "* Torus" );

	GVert	buffer[NSUBDIVS+1];

	glBegin( GL_TRIANGLE_STRIP );
	for (int uI=0; uI <= NSUBDIVS; ++uI)
	{
		float	u = uI / (float)NSUBDIVS;

		float theta = u * mThetamaxRad;

		//glBegin( GL_TRIANGLE_STRIP );
		for (int vI=0; vI <= NSUBDIVS; ++vI)
		{
			float	v = vI / (float)NSUBDIVS;

			float	phi = mPhiminRad + (mPhimaxRad - mPhiminRad) * v;
			
			GVert	vert;

			float	r = mMinRadius * cosf( phi );
			vert.z	  = mMinRadius * sinf( phi );
			
			vert.x	= (mMaxRadius + r) * cosf( theta );
			vert.y	= (mMaxRadius + r) * sinf( theta );

			vert.u	= u;
			vert.v	= v;

			if ( uI > 0 )
			{
				gstate.AddVertex( buffer[vI] );
				gstate.AddVertex( vert );
			}

			buffer[vI] = vert;
		}
		//glEnd();
	}
	glEnd();
}

//==================================================================
static void getHullFromParam( Vector3 *pDestHull, int hullN, ParamList &params, int parIdx )
{
	const float *pHull = params[parIdx].PFlt( 3 * hullN );
	for (int hi=0; hi < hullN; ++hi)
		pDestHull[hi] = Vector3( pHull + hi*3 );	
}

//==================================================================
PatchBicubic::PatchBicubic( RtToken type, ParamList &params, const Attributes &attr, const SymbolList &staticSymbols ) :
	Primitive(PATCHBICUBIC),
	mParams(params)
{
	mpyIntplType = staticSymbols.FindVoid( type );
	mpUBasis = &attr.GetUBasis();
	mpVBasis = &attr.GetVBasis();
	mUSteps = attr.mUSteps;
	mVSteps = attr.mVSteps;

	bool	gotP = false;

	// expect an odd number of params (patch type param 0 and then couples
	DASSTHROW( params.size() >= 3 && ((params.size()-1) & 1) == 0,
			   "Wrong number of parameters" );
 
	for (int i=1; i < (int)params.size(); i += 2)
	{
		DASSERT( params[i].type == Param::STR );

		CPSymVoid pyToken = staticSymbols.FindVoid( params[i] );
		if ( pyToken && pyToken->IsNameI( "P" ) )
		{
			gotP = true;

			getHullFromParam( mHullPos, 16, params, i+1 );
		}
	}
	
	DASSTHROW( gotP, "Missing hull parameter" );
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