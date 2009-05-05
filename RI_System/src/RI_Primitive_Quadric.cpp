//==================================================================
/// RI_Primitive_Quadric.cpp
///
/// Created by Davide Pasca - 2009/1/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

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
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	float	theta = u * mThetamaxRad;

	out_pt.Set(
			mRadius * DCos( theta ),
			mRadius * DSin( theta ),
			mZMin + v * (mZMax - mZMin) );

	if ( out_dPdu )
	{
		float	tmp = mThetamaxRad * mRadius;

		out_dPdu->Set(
					tmp * -DSin( theta ),
					tmp *  DCos( theta ),
					0 );

		out_dPdv->Set(
					0,
					0,
					mZMax - mZMin );
	}
}

//==================================================================
void Cone::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	float	theta = u * mThetamaxRad;
	float	cosUTheta = DCos( theta );
	float	sinUTheta = DSin( theta );

	float	rad1V = mRadius * (1 - v);
	
	out_pt.Set(
		rad1V * cosUTheta,
		rad1V * sinUTheta,
		v * mHeight );

	if ( out_dPdu )
	{
		float	tmp = mThetamaxRad * rad1V;

		out_dPdu->Set(
			tmp * -sinUTheta,
			tmp *  cosUTheta,
			0 );

		out_dPdv->Set(
			mRadius * -cosUTheta,
			mRadius * -sinUTheta,
			mHeight );
	}
}

//==================================================================
void Sphere::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	// $$$ following 2 are "uniform"
	float	alphamin	= DASin( mZMin / mRadius );
	float	alphadelta	= DASin( mZMax / mRadius ) - alphamin;

	float	theta = u * mThetamaxRad;
	float	alpha = alphamin + v * alphadelta;

	out_pt.Set(
		mRadius * DCos( alpha ) * DCos( theta ),
		mRadius * DCos( alpha ) * DSin( theta ),
		mRadius * DSin( alpha ) );
	
	if ( out_dPdu )
	{
		float	cosAlpha = DCos( alpha );
		float	cosUTheta = DCos( theta );
		float	sinUTheta = DSin( theta );

		float	tmp1 = mThetamaxRad * mRadius;
		out_dPdu->Set(
			tmp1 * -sinUTheta * cosAlpha,
			tmp1 *  cosUTheta * cosAlpha,
			0 );

		float	tmp2 = alphadelta * mRadius;
		float	tmp3 = tmp2 * -DSin( alpha );
		out_dPdv->Set(
			tmp3 * cosUTheta,
			tmp3 * sinUTheta,
			tmp2 * cosAlpha );
	}
}

//==================================================================
void Hyperboloid::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = DCos( uTheta );
	float	sinUTheta = DSin( uTheta );

	Vec3f	p1p2v = DMix( mP1, mP2, v );

	out_pt.Set(
		p1p2v.x() * cosUTheta - p1p2v.y() * sinUTheta,
		p1p2v.x() * sinUTheta + p1p2v.y() * cosUTheta,
		p1p2v.z() );

	if ( out_dPdu )
	{
		out_dPdu->Set(
			mThetamaxRad * (p1p2v.x() * -sinUTheta - p1p2v.y() *  cosUTheta),
			mThetamaxRad * (p1p2v.x() *  cosUTheta + p1p2v.y() * -sinUTheta),
			0 );

		Vec3f	dp = mP2 - mP1;

		out_dPdv->Set(
			dp.x() * cosUTheta - dp.y() * sinUTheta,
			dp.x() * sinUTheta + dp.y() * cosUTheta,
			dp.z() );
	}
}

//==================================================================
void Paraboloid::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = DCos( uTheta );
	float	sinUTheta = DSin( uTheta );
	float	scale = mRmax / DSqrt( mZmax );
	float	z = DMix( mZmin, mZmax, v );
	float	r = scale * DSqrt( z );

	out_pt.Set(
			r * cosUTheta,
			r * sinUTheta,
			z );

	if ( out_dPdu )
	{
		float	tmp1 = r * mThetamaxRad;
		out_dPdu->Set(
			tmp1 * -sinUTheta,
			tmp1 *  cosUTheta,
			0 );

		float	dz = mZmax - mZmin;
		float	dx = 0.5f * dz / DSqrt( z ) * scale;

		out_dPdv->Set(
			dx * cosUTheta,
			dx * sinUTheta,
			dz );
	}
}

//==================================================================
void Torus::Eval_dPdu_dPdv(
			float u,
			float v,
			Point3 &out_pt,
			Vec3f *out_dPdu,
			Vec3f *out_dPdv ) const
{
	float	uTheta = u * mThetamaxRad;
	float	cosUTheta = DCos( uTheta );
	float	sinUTheta = DSin( uTheta );
	float	phi = DMix( mPhiminRad, mPhimaxRad, v );
	float	sx = DCos( phi ) * mMinRadius + mMaxRadius;

	out_pt.Set(
			sx * cosUTheta,
			sx * sinUTheta,
			mMinRadius * DSin( phi ) );

	if ( out_dPdu )
	{
		out_dPdu->Set(
			sx * mThetamaxRad * -sinUTheta,
			sx * mThetamaxRad *  cosUTheta,
			0 );

		float dphi	= mPhimaxRad - mPhiminRad;
		float dsx	= dphi * -DSin( phi ) * mMinRadius;

		out_dPdv->Set(
			dsx * cosUTheta,
			dsx * sinUTheta,
			dphi * DCos( phi ) * mMinRadius );
	}
}

//==================================================================
inline Point3 polar( float radius, float theta )
{
	return Point3( DCos(theta)*radius, DSin(theta)*radius, 0 );
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
	
	if ( tmin < FM_PI_2 && tmax > FM_PI_2 )
		out_bound.Expand( polar( rmax, FM_PI_2 ) );

	if ( tmin < FM_PI && tmax > FM_PI )
		out_bound.Expand( polar( rmax, FM_PI ) );

	if ( tmin < (FM_PI+FM_PI_2) && tmax > (FM_PI+FM_PI_2) )
		out_bound.Expand( polar( rmax, (FM_PI+FM_PI_2) ) );
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
		float	phi = i * FM_PI_2;
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
	float	r = DSqrt( p.x() * p.x() + p.y() * p.y() );
	float	delta = atan2f( p.y(), p.x() );

	theMin += delta;
	theMax += delta;
	
	out_bound.Expand( polar(r, theMin) );
	out_bound.Expand( polar(r, theMax) );

	for (int i=-1; i < 6; ++i)
	{
		float	the = i * FM_PI_2;
		if ( theMin < the && theMax > the )
			out_bound.Expand( polar( r, the ) );
	}
}

//==================================================================
bool Cylinder::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	out_bound.Reset();
	bounds2DSweepL( out_bound, mRadius, mRadius, tuMin, tuMax );
	out_bound.mBox[0].z() = DMix( mZMin, mZMax, mVRange[0] );
	out_bound.mBox[1].z() = DMix( mZMin, mZMax, mVRange[1] );
	return true;
}

bool Cone::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	rMin = mRadius * (1 - mVRange[1]);
	float	rMax = mRadius * (1 - mVRange[0]);
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z() = mVRange[0] * mHeight;
	out_bound.mBox[1].z() = mVRange[1] * mHeight;
	return true;
}

bool Sphere::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	alphaMin = DASin( mZMin / mRadius );
	float	alphaMax = DASin( mZMax / mRadius );

	float	aVMin = DMix( alphaMin, alphaMax, mVRange[0] );
	float	aVMax = DMix( alphaMin, alphaMax, mVRange[1] );

	float	rVMin = DCos( aVMin ) * mRadius;
	float	rVMax = DCos( aVMax ) * mRadius;
	float	rMin = DMIN( rVMin, rVMax );
	
	float	rMax;
	
	if ( aVMin < 0 && aVMax > 0 )
		rMax = mRadius;
	else
		rMax = DMAX( rVMin, rVMax );
	
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );

	out_bound.mBox[0].z() = DSin( aVMin ) * mRadius;
	out_bound.mBox[1].z() = DSin( aVMax ) * mRadius;
	return true;
}

bool Hyperboloid::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	Point3	pMin; EvalP( 0, mVRange[0], pMin );
	Point3	pMax; EvalP( 0, mVRange[1], pMax );

	out_bound.Reset();
	bounds2DSweepP( out_bound, pMin, tuMin, tuMax );
	bounds2DSweepP( out_bound, pMax, tuMin, tuMax );
	
	out_bound.mBox[0].z() = DMIN( pMin.z(), pMax.z() );
	out_bound.mBox[1].z() = DMAX( pMin.z(), pMax.z() );
	return true;
}

bool Paraboloid::MakeBound( Bound &out_bound ) const
{
	float	scale = mRmax / DSqrt( mZmax );

	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	float	zVMin = mZmin + mVRange[0] * (mZmax-mZmin);
	float	zVMax = mZmin + mVRange[1] * (mZmax-mZmin);
	float	rMin = DSqrt( zVMin ) * scale;
	float	rMax = DSqrt( zVMax ) * scale;
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z() = zVMin;
	out_bound.mBox[1].z() = zVMax;
	return true;
}

bool Torus::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	phiVMin = DMix( mPhiminRad, mPhimaxRad, mVRange[0] );
	float	phiVMax = DMix( mPhiminRad, mPhimaxRad, mVRange[1] );

	Bound	a;
	a.Reset();
	bounds2DSweepR( a, mMinRadius, phiVMin, phiVMax );
	float	rMin = a.mBox[0].x() + mMaxRadius;
	float	rMax = a.mBox[1].x() + mMaxRadius;

	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z() = a.mBox[0].y();
	out_bound.mBox[1].z() = a.mBox[1].y();
	return true;
}

//==================================================================
}