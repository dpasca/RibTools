/*
 *  RI_Primitive_Quadric.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/18.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "stdafx.h"
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
void Cylinder::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	float	theta = u * mThetamaxRad;

	out_pt.x = mRadius * cosf( theta );
	out_pt.y = mRadius * sinf( theta );
	out_pt.z = mZMin + v * (mZMax - mZMin);

	if ( out_dPdu )
	{
		float	tmp = mThetamaxRad * mRadius;

		out_dPdu->x = tmp * -sinf( theta );
		out_dPdu->y = tmp *  cosf( theta );
		out_dPdu->z = 0;

		out_dPdv->x = 0;
		out_dPdv->y = 0;
		out_dPdv->z = mZMax - mZMin;
	}
}

//==================================================================
void Cone::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	float	theta = u * mThetamaxRad;
	float	cosUTheta = cosf( theta );
	float	sinUTheta = sinf( theta );

	float	rad1V = mRadius * (1 - v);
	
	out_pt.x = rad1V * cosUTheta;
	out_pt.y = rad1V * sinUTheta;
	out_pt.z = v * mHeight;

	if ( out_dPdu )
	{
		float	tmp = mThetamaxRad * rad1V;

		out_dPdu->x = tmp * -sinUTheta;
		out_dPdu->y = tmp *  cosUTheta;
		out_dPdu->z = 0;

		out_dPdv->x = mRadius * -cosUTheta;
		out_dPdv->y = mRadius * -sinUTheta;
		out_dPdv->z = mHeight;
	}
}

//==================================================================
void Sphere::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	// $$$ following 2 are "uniform"
	float	alphamin	= asinf( mZMin / mRadius );
	float	alphadelta	= asinf( mZMax / mRadius ) - alphamin;

	float	theta = u * mThetamaxRad;
	float	alpha = alphamin + v * alphadelta;

	out_pt.x = mRadius * cosf( alpha ) * cosf( theta );
	out_pt.y = mRadius * cosf( alpha ) * sinf( theta );
	out_pt.z = mRadius * sinf( alpha );
	
	if ( out_dPdu )
	{
		float	cosAlpha = cosf( alpha );
		float	cosUTheta = cosf( theta );
		float	sinUTheta = sinf( theta );

		float	tmp1 = mThetamaxRad * mRadius;
		out_dPdu->x = tmp1 * -sinUTheta * cosAlpha;
		out_dPdu->y = tmp1 *  cosUTheta * cosAlpha;
		out_dPdu->z = 0;

		float	tmp2 = alphadelta * mRadius;
		float	tmp3 = tmp2 * -sinf( alpha );
		out_dPdv->x = tmp3 * cosUTheta;
		out_dPdv->y = tmp3 * sinUTheta;
		out_dPdv->z = tmp2 * cosAlpha;
	}
}

//==================================================================
void Hyperboloid::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = cosf( uTheta );
	float	sinUTheta = sinf( uTheta );

	Vector3	p1p2v = DMix( mP1, mP2, v );

	out_pt.x = p1p2v.x * cosUTheta - p1p2v.y * sinUTheta;
	out_pt.y = p1p2v.x * sinUTheta + p1p2v.y * cosUTheta;
	out_pt.z = p1p2v.z;

	if ( out_dPdu )
	{
		out_dPdu->x = mThetamaxRad * (p1p2v.x * -sinUTheta - p1p2v.y *  cosUTheta);
		out_dPdu->y = mThetamaxRad * (p1p2v.x *  cosUTheta + p1p2v.y * -sinUTheta);
		out_dPdu->z = 0;

		Vector3	dp = mP2 - mP1;

		out_dPdv->x = dp.x * cosUTheta - dp.y * sinUTheta;
		out_dPdv->y = dp.y * sinUTheta + dp.y * cosUTheta;
		out_dPdv->z = dp.z;
	}
}

//==================================================================
void Paraboloid::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = cosf( uTheta );
	float	sinUTheta = sinf( uTheta );
	float	scale = mRmax / sqrtf( mZmax );
	float	z = DMix( mZmin, mZmax, v );
	float	r = scale * sqrtf( z );

	out_pt.x = r * cosUTheta;
	out_pt.y = r * sinUTheta;
	out_pt.z = z;

	if ( out_dPdu )
	{
		float	tmp1 = r * mThetamaxRad;
		out_dPdu->x = tmp1 * -sinUTheta;
		out_dPdu->y = tmp1 *  cosUTheta;
		out_dPdu->z = 0;

		float	dz = mZmax - mZmin;
		float	dx = 0.5f * dz / sqrtf( z ) * scale;

		out_dPdv->x = dx * cosUTheta;
		out_dPdv->y = dx * sinUTheta;
		out_dPdv->z = dz;
	}
}

//==================================================================
void Torus::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vector3 *out_dPdu,
			Vector3 *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = cosf( uTheta );
	float	sinUTheta = sinf( uTheta );
	float	phi = DMix( mPhiminRad, mPhimaxRad, v );
	float	sx = cosf( phi ) * mMinRadius + mMaxRadius;

	out_pt.x = sx * cosUTheta;
	out_pt.y = sx * sinUTheta;
	out_pt.z = mMinRadius * sinf( phi );

	if ( out_dPdu )
	{
		out_dPdu->x = sx * mThetamaxRad * -sinUTheta;
		out_dPdu->y = sx * mThetamaxRad *  cosUTheta;
		out_dPdu->z = 0;

		float dphi	= mPhimaxRad - mPhiminRad;
		float dsx	= dphi * -sinf( phi ) * mMinRadius;

		out_dPdv->x = dsx * cosUTheta;
		out_dPdv->y = dsx * sinUTheta;
		out_dPdv->z = dphi * cosf( phi ) * mMinRadius;
	}
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
	out_bound.Expand( polar(rmin,tmin) );
	out_bound.Expand( polar(rmax,tmin) );
	out_bound.Expand( polar(rmin,tmax) );
	out_bound.Expand( polar(rmax,tmax) );
	
	if ( tmin < (float)M_PI_2 && tmax > (float)M_PI_2 )
		out_bound.Expand( polar( rmax, (float)M_PI_2 ) );

	if ( tmin < (float)M_PI && tmax > (float)M_PI )
		out_bound.Expand( polar( rmax, (float)M_PI ) );

	if ( tmin < (float)(M_PI+M_PI_2) && tmax > (float)(M_PI+M_PI_2) )
		out_bound.Expand( polar( rmax, (float)(M_PI+M_PI_2) ) );
}

//==================================================================
inline void bounds2DSweepR(
					Bound &out_bound,
					float r,
					float phiMin,
					float phiMax )
{
	out_bound.Expand( polar(r, phiMin) );
	out_bound.Expand( polar(r, phiMax) );

	for (int i=-3; i < 4; ++i)
	{
		float	phi = i * (float)M_PI_2;
		if ( phiMin < phi && phiMax > phi )
			out_bound.Expand( polar( r, phi ) );
	}
}

//==================================================================
inline void bounds2DSweepP(
					Bound &out_bound,
					const Point3 &p,
					float theMin,
					float theMax )
{
	float	r = sqrtf( p.x * p.x + p.y * p.y );
	float	delta = atan2f( p.y, p.x );
	
	theMin += delta;
	theMax += delta;
	
	out_bound.Expand( polar(r, theMin) );
	out_bound.Expand( polar(r, theMax) );

	for (int i=-1; i < 6; ++i)
	{
		float	the = i * (float)M_PI_2;
		if ( theMin < the && theMax > the )
			out_bound.Expand( polar( r, the ) );
	}
}

//==================================================================
void Cylinder::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	out_bound.Reset();
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
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z = mVRange[0] * mHeight;
	out_bound.mBox[1].z = mVRange[1] * mHeight;
}

void Sphere::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	alphaMin = asinf( mZMin / mRadius );
	float	alphaMax = asinf( mZMax / mRadius );

	float	aVMin = DMix( alphaMin, alphaMax, mVRange[0] );
	float	aVMax = DMix( alphaMin, alphaMax, mVRange[1] );

	float	rVMin = cosf( aVMin ) * mRadius;
	float	rVMax = cosf( aVMax ) * mRadius;
	float	rMin = DMIN( rVMin, rVMax );
	
	float	rMax;
	
	if ( aVMin < 0 && aVMax > 0 )
		rMax = mRadius;
	else
		rMax = DMAX( rVMin, rVMax );
	
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );

	out_bound.mBox[0].z = sinf( aVMin ) * mRadius;
	out_bound.mBox[1].z = sinf( aVMax ) * mRadius;
}

void Hyperboloid::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	Point3	pMin; EvalP( 0, mVRange[0], pMin );
	Point3	pMax; EvalP( 0, mVRange[1], pMax );

	out_bound.Reset();
	bounds2DSweepP( out_bound, pMin, tuMin, tuMax );
	bounds2DSweepP( out_bound, pMax, tuMin, tuMax );
	
	out_bound.mBox[0].z = DMIN( pMin.z, pMax.z );
	out_bound.mBox[1].z = DMAX( pMin.z, pMax.z );
}

void Paraboloid::MakeBound( Bound &out_bound )
{
	float	scale = mRmax / sqrtf( mZmax );

	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	float	zVMin = mZmin + mVRange[0] * (mZmax-mZmin);
	float	zVMax = mZmin + mVRange[1] * (mZmax-mZmin);
	float	rMin = sqrtf( zVMin ) * scale;
	float	rMax = sqrtf( zVMax ) * scale;
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z = zVMin;
	out_bound.mBox[1].z = zVMax;
}

void Torus::MakeBound( Bound &out_bound )
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	phiVMin = DMix( mPhiminRad, mPhimaxRad, mVRange[0] );
	float	phiVMax = DMix( mPhiminRad, mPhimaxRad, mVRange[1] );

	Bound	a;
	a.Reset();
	bounds2DSweepR( a, mMinRadius, phiVMin, phiVMax );
	float	rMin = a.mBox[0].x + mMaxRadius;
	float	rMax = a.mBox[1].x + mMaxRadius;

	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z = a.mBox[0].y;
	out_bound.mBox[1].z = a.mBox[1].y;
}

//==================================================================
}