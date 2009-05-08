//==================================================================
/// RI_Primitive_Quadric.h
///
/// Created by Davide Pasca - 2009/01/18
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_QUADRIC_H
#define RI_PRIMITIVE_QUADRIC_H

#include "DSystem/include/DMemory.h"
#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

//==================================================================
/// Cylinder
//==================================================================
class Cylinder : public SimplePrimitiveBase
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Cylinder( float radius, float zmin, float zmax, float thetamax ) :
		SimplePrimitiveBase(CYLINDER),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		Cylinder *Clone() const {	return DNEW Cylinder( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// Cone
//==================================================================
class Cone : public SimplePrimitiveBase
{
public:
	float	mHeight;
	float	mRadius;
	float	mThetamaxRad;

public:
	Cone( float height, float radius, float thetamax ) :
		SimplePrimitiveBase(CONE),
		mHeight(height),
		mRadius(radius),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		Cone *Clone() const {	return DNEW Cone( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// Sphere
//==================================================================
class Sphere : public SimplePrimitiveBase
{
public:
	float	mRadius;
	float	mZMin;
	float	mZMax;
	float	mThetamaxRad;

public:
	Sphere( float radius, float zmin, float zmax, float thetamax ) :
		SimplePrimitiveBase(SPHERE),
		mRadius(radius),
		mZMin(zmin),
		mZMax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		Sphere *Clone() const {	return DNEW Sphere( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// Hyperboloid
//==================================================================
class Hyperboloid : public SimplePrimitiveBase
{
public:
	Vec3f	mP1;
	Vec3f	mP2;
	float	mThetamaxRad;

public:
	Hyperboloid( const Vec3f &p1, const Vec3f &p2, float thetamax ) :
		SimplePrimitiveBase(HYPERBOLOID),
		mP1(p1),
		mP2(p2),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		Hyperboloid *Clone() const { return DNEW Hyperboloid( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// Paraboloid
//==================================================================
class Paraboloid : public SimplePrimitiveBase
{
public:
	float	mRmax;
	float	mZmin;
	float	mZmax;
	float	mThetamaxRad;

public:
	Paraboloid( float rmax, float zmin, float zmax, float thetamax ) :
		SimplePrimitiveBase(PARABOLOID),
		mRmax(rmax),
		mZmin(zmin),
		mZmax(zmax),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}

		Paraboloid *Clone() const {	return DNEW Paraboloid( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
/// Torus
//==================================================================
class Torus : public SimplePrimitiveBase
{
public:
	float	mMinRadius;
	float	mMaxRadius;
	float	mPhiminRad;
	float	mPhimaxRad;
	float	mThetamaxRad;

public:
	Torus( float maxRadius, float minRadius,
		   float phimin, float phimax,
		   float thetamax ) :
		SimplePrimitiveBase(TORUS),
		mMaxRadius(maxRadius),
		mMinRadius(minRadius),
		mPhiminRad(phimin*DEG2RAD),
		mPhimaxRad(phimax*DEG2RAD),
		mThetamaxRad(thetamax*DEG2RAD)
	{
	}
	
		Torus *Clone() const {	return DNEW Torus( *this ); }

		bool MakeBound( Bound &out_bound ) const;

		void Eval_dPdu_dPdv(
					float u,
					float v,
					Point3 &out_pt,
					Vec3f *out_dPdu,
					Vec3f *out_dPdv ) const;

		void Eval_dPdu_dPdv(
						const SlVec2 &uv,
						SlVec3 &out_pt,
						SlVec3 *out_dPdu,
						SlVec3 *out_dPdv ) const;
};

//==================================================================
}

#endif