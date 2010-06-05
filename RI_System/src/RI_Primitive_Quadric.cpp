//==================================================================
/// RI_Primitive_Quadric.cpp
///
/// Created by Davide Pasca - 2009/1/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
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
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float_	theta = uv[0] * mThetamaxRad;

	out_pt.Set(
			DCos( theta ) * mRadius,
			DSin( theta ) * mRadius,
			DMix( Float_( mZMin ), Float_( mZMax ), Float_( uv[1] ) ) );

	if ( out_dPdu )
	{
		Float_	tmp = mThetamaxRad * mRadius;

		out_dPdu->Set(
					tmp * -DSin( theta ),
					tmp *  DCos( theta ),
					0.f );

		out_dPdv->Set(
					0.f,
					0.f,
					mZMax - mZMin );
	}
}

//==================================================================
void Cone::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float_	theta = uv[0] * mThetamaxRad;
	Float_	cosUTheta = DCos( theta );
	Float_	sinUTheta = DSin( theta );

	Float_	rad1V = mRadius * (Float_( 1 ) - uv[1]);
	
	out_pt.Set(
		rad1V * cosUTheta,
		rad1V * sinUTheta,
		uv[1] * mHeight );

	if ( out_dPdu )
	{
		Float_	tmp = mThetamaxRad * rad1V;

		out_dPdu->Set(
			tmp * -sinUTheta,
			tmp *  cosUTheta,
			0.f );

		out_dPdv->Set(
			mRadius * -cosUTheta,
			mRadius * -sinUTheta,
			mHeight );
	}
}

//==================================================================
void Sphere::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	// $$$ following 2 are "uniform"
	Float_	alphamin	= DASin( mZMin / mRadius );
	Float_	alphadelta	= Float_( DASin( mZMax / mRadius ) ) - alphamin;

	Float_	theta = uv[0] * mThetamaxRad;
	Float_	alpha = alphamin + uv[1] * alphadelta;

	out_pt.Set(
		mRadius * DCos( alpha ) * DCos( theta ),
		mRadius * DCos( alpha ) * DSin( theta ),
		mRadius * DSin( alpha ) );
	
	if ( out_dPdu )
	{
		Float_	cosAlpha = DCos( alpha );
		Float_	cosUTheta = DCos( theta );
		Float_	sinUTheta = DSin( theta );

		Float_	tmp1 = mThetamaxRad * mRadius;
		out_dPdu->Set(
			tmp1 * -sinUTheta * cosAlpha,
			tmp1 *  cosUTheta * cosAlpha,
			0.f );

		Float_	tmp2 = alphadelta * mRadius;
		Float_	tmp3 = tmp2 * -DSin( alpha );
		out_dPdv->Set(
			tmp3 * cosUTheta,
			tmp3 * sinUTheta,
			tmp2 * cosAlpha );
	}
}

//==================================================================
void Hyperboloid::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float_	uTheta = uv[0] * mThetamaxRad;
	Float_	cosUTheta = DCos( uTheta );
	Float_	sinUTheta = DSin( uTheta );

	Float3_	p1p2v = DMix( Float3_( mP1 ), Float3_( mP2 ), uv[1] );

	out_pt.Set(
		p1p2v.x() * cosUTheta - p1p2v.y() * sinUTheta,
		p1p2v.x() * sinUTheta + p1p2v.y() * cosUTheta,
		p1p2v.z() );

	if ( out_dPdu )
	{
		out_dPdu->Set(
			mThetamaxRad * (p1p2v.x() * -sinUTheta - p1p2v.y() *  cosUTheta),
			mThetamaxRad * (p1p2v.x() *  cosUTheta + p1p2v.y() * -sinUTheta),
			0.f );

		Float3_	dp = mP2 - mP1;

		out_dPdv->Set(
			dp.x() * cosUTheta - dp.y() * sinUTheta,
			dp.x() * sinUTheta + dp.y() * cosUTheta,
			dp.z() );
	}
}

//==================================================================
void Paraboloid::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float_	uTheta = uv[0] * mThetamaxRad;
	Float_	cosUTheta = DCos( uTheta );
	Float_	sinUTheta = DSin( uTheta );
	Float_	scale = mRmax / DSqrt( mZmax );
	Float_	z = DMix( Float_( mZmin ), Float_( mZmax ), uv[1] );
	Float_	r = scale * DSqrt( z );

	out_pt.Set(
			r * cosUTheta,
			r * sinUTheta,
			z );

	if ( out_dPdu )
	{
		Float_	tmp1 = r * mThetamaxRad;
		out_dPdu->Set(
			tmp1 * -sinUTheta,
			tmp1 *  cosUTheta,
			0.f );

		Float_	dz = mZmax - mZmin;
		Float_	dx = 0.5f * dz / DSqrt( z ) * scale;

		out_dPdv->Set(
			dx * cosUTheta,
			dx * sinUTheta,
			dz );
	}
}

//==================================================================
void Torus::Eval_dPdu_dPdv(
				const Float2_ &uv,
				Float3_ &out_pt,
				Float3_ *out_dPdu,
				Float3_ *out_dPdv ) const
{
	Float_	uTheta = uv[0] * mThetamaxRad;
	Float_	cosUTheta = DCos( uTheta );
	Float_	sinUTheta = DSin( uTheta );
	Float_	phi = DMix( Float_( mPhiminRad ), Float_( mPhimaxRad ), uv[1] );
	Float_	sx = DCos( phi ) * mMinRadius + mMaxRadius;

	out_pt.Set(
			sx * cosUTheta,
			sx * sinUTheta,
			mMinRadius * DSin( phi ) );

	if ( out_dPdu )
	{
		out_dPdu->Set(
			sx * mThetamaxRad * -sinUTheta,
			sx * mThetamaxRad *  cosUTheta,
			0.f );

		Float_ dphi	= mPhimaxRad - mPhiminRad;
		Float_ dsx	= dphi * -DSin( phi ) * mMinRadius;

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
					float x,
					float y,
					float theMin,
					float theMax )
{
	float	r = DSqrt( x * x + y * y );
	float	delta = atan2f( y, x );

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
void Cylinder::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];
	out_bound.Reset();
	bounds2DSweepL( out_bound, mRadius, mRadius, tuMin, tuMax );
	out_bound.mBox[0].z() = DMix( mZMin, mZMax, mVRange[0] );
	out_bound.mBox[1].z() = DMix( mZMin, mZMax, mVRange[1] );
}

void Cone::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	float	rMin = mRadius * (1 - mVRange[1]);
	float	rMax = mRadius * (1 - mVRange[0]);
	out_bound.Reset();
	bounds2DSweepL( out_bound, rMin, rMax, tuMin, tuMax );
	out_bound.mBox[0].z() = mVRange[0] * mHeight;
	out_bound.mBox[1].z() = mVRange[1] * mHeight;
}

void Sphere::MakeBound( Bound &out_bound ) const
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
}

void Hyperboloid::MakeBound( Bound &out_bound ) const
{
	float	tuMin = mThetamaxRad * mURange[0];
	float	tuMax = mThetamaxRad * mURange[1];

	Float2_	uvMin( 0.f, mVRange[0] );
	Float2_	uvMax( 0.f, mVRange[1] );
	Float3_	pMin; EvalP( uvMin, pMin );
	Float3_	pMax; EvalP( uvMax, pMax );

	out_bound.Reset();
	bounds2DSweepP( out_bound, pMin.x()[0], pMin.y()[0], tuMin, tuMax );
	bounds2DSweepP( out_bound, pMax.x()[0], pMax.y()[0], tuMin, tuMax );
	
	out_bound.mBox[0].z() = DMIN( pMin.z()[0], pMax.z()[0] );
	out_bound.mBox[1].z() = DMAX( pMin.z()[0], pMax.z()[0] );
}

void Paraboloid::MakeBound( Bound &out_bound ) const
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
}

void Torus::MakeBound( Bound &out_bound ) const
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
}

//==================================================================
}