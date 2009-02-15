/*
 *  RI_Primitive_Quadric.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/18.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLUT/glut.h>
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive.h"
#include "RI_Primitive_Quadric.h"

//==================================================================
namespace RI
{

//==================================================================
void Cylinder::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	float	theta = u * mThetamaxRad;

	Point3	Po;
	Po.x = mRadius * cosf( theta );
	Po.y = mRadius * sinf( theta );
	Po.z = mZMin + v * (mZMax - mZMin);
	
	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
void Cone::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	float	theta = u * mThetamaxRad;

	Point3	Po;
	Po.x = mRadius * (1 - v) * cosf( theta );
	Po.y = mRadius * (1 - v) * sinf( theta );
	Po.z = v * mHeight;
	
	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
void Sphere::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	// $$$ following 2 are "uniform"
	float	alphamin	= asinf( mZMin / mRadius );
	float	alphadelta	= asinf( mZMax / mRadius ) - alphamin;

	float	theta = u * mThetamaxRad;
	float	alpha = alphamin + v * alphadelta;

	Point3	Po;
	Po.x = mRadius * cosf( alpha ) * cosf( theta );
	Po.y = mRadius * cosf( alpha ) * sinf( theta );
	Po.z = mRadius * sinf( alpha );
	
	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
void Hyperboloid::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	float	theta = u * mThetamaxRad;

	float	x = mP1.x + (mP2.x - mP1.x) * v;
	float	y = mP1.y + (mP2.y - mP1.y) * v;
	float	z = mP1.z + (mP2.z - mP1.z) * v;

	Point3	Po;
	Po.x = x * cosf( theta ) - y * sinf( theta );
	Po.y = x * sinf( theta ) + y * cosf( theta );
	Po.z = z;
	
	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
void Paraboloid::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	float	theta = u * mThetamaxRad;

	float	z = (mZmax - mZmin) * v;
	float	r = mRmax * sqrtf( z / mZmax );

	Point3	Po;
	Po.x = r * cosf( theta );
	Po.y = r * sinf( theta );
	Po.z = z;

	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
void Torus::EvalP(
			float uGrid,
			float vGrid,
			Point3 &out_pt,
			const Matrix44 &mtxObjectCurrent ) const
{
	float	u = mURange[0] + (mURange[1] - mURange[0]) * uGrid;
	float	v = mVRange[0] + (mVRange[1] - mVRange[0]) * vGrid;

	float	theta = u * mThetamaxRad;

	float	phi = mPhiminRad + (mPhimaxRad - mPhiminRad) * v;
	float	r = mMinRadius * cosf( phi );

	Point3	Po;
	Po.x = (mMaxRadius + r) * cosf( theta );
	Po.y = (mMaxRadius + r) * sinf( theta );
	Po.z = mMinRadius * sinf( phi );
	
	out_pt = MultiplyMV3( Po, mtxObjectCurrent );
}

//==================================================================
inline Point3 polar( float radius, float theta )
{
	return Point3( cosf(theta)*radius, sinf(theta)*radius, 0 );
}

//==================================================================
inline void bounds2DSweepL(
					Bound &out_bound,
					float rmin,
					float rmax,
					float tmin,
					float tmax )
{
	out_bound.Reset();
	out_bound.Expand( polar(rmin,tmin) );
	out_bound.Expand( polar(rmax,tmin) );
	out_bound.Expand( polar(rmin,tmax) );
	out_bound.Expand( polar(rmax,tmax) );
	
	if ( tmin < (float)M_PI_2 && tmax > (float)M_PI_2 )
		out_bound.Expand( polar( rmax, M_PI_2 ) );

	if ( tmin < (float)M_PI && tmax > (float)M_PI )
		out_bound.Expand( polar( rmax, M_PI ) );

	if ( tmin < (float)(M_PI+M_PI_2) && tmax > (float)(M_PI+M_PI_2) )
		out_bound.Expand( polar( rmax, (float)(M_PI+M_PI_2) ) );
}

//==================================================================
void Cylinder::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	bounds2DSweepL( out_bound, mRadius, mRadius, tuMin, tuMax );
	out_bound.mBox[0].z = mZMin + mVRange[0]*(mZMax - mZMin);
	out_bound.mBox[1].z = mZMin + mVRange[1]*(mZMax - mZMin);
}

void Cone::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	float	rMin = mRadius * (1 - mVRange[1]);
	float	rMax = mRadius * (1 - mVRange[0]);
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z = mVRange[0] * mHeight;
	out_bound.mBox[1].z = mVRange[1] * mHeight;
}

void Sphere::MakeBound( Bound &out_bound )
{
	out_bound.Reset();
}

void Hyperboloid::MakeBound( Bound &out_bound )
{
	out_bound.Reset();
}

void Paraboloid::MakeBound( Bound &out_bound )
{
	out_bound.Reset();
}

void Torus::MakeBound( Bound &out_bound )
{
	out_bound.Reset();
}

/*
void Disk::MakeBound( Bound &out_bound )
{
}
*/

/*
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
*/

//==================================================================
}