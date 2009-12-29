//==================================================================
/// DVector.h
///
/// Created by Davide Pasca - 2009/5/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DVECTOR_H
#define DVECTOR_H

#include "DMathBase.h"
#include "DMT_VecN.h"

//==================================================================
/// Vec2
//==================================================================
template <class _S>
class Vec2
{
public:
	_S	v2[2];

	//==================================================================
	Vec2()								{}
	Vec2( const Vec2 &v )				{ v2[0] = v[0]; v2[1] = v[1];	}

	template <class _T>
	Vec2( const Vec2<_T> &v_ )			{ v2[0] = v_[0]; v2[1] = v_[1]; }

	Vec2( const _S& a_ )				{ v2[0] = a_; v2[1] = a_;		}
	Vec2( const _S& x_, const _S& y_ )	{ v2[0] = x_; v2[1] = y_;		}
	Vec2( const _S *p_ )				{ v2[0] = p_[0]; v2[1] = p_[1]; }

	void Set( const _S& x_, const _S& y_ )
	{
		v2[0] = x_;
		v2[1] = y_;
	}
	
	void Set( const _S *p_ )
	{
		v2[0] = p_[0];
		v2[1] = p_[1];
	}

	void SetZero()
	{
		v2[0] = v2[1] = 0.f;
	}

	Vec2 operator + (const _S& rval) const { return Vec2( v2[0] + rval, v2[1] + rval ); }
	Vec2 operator - (const _S& rval) const { return Vec2( v2[0] - rval, v2[1] - rval ); }
	Vec2 operator * (const _S& rval) const { return Vec2( v2[0] * rval, v2[1] * rval ); }
	Vec2 operator / (const _S& rval) const { return Vec2( v2[0] / rval, v2[1] / rval ); }
	Vec2 operator + (const Vec2 &rval) const { return Vec2( v2[0] + rval[0], v2[1] + rval[1] ); }
	Vec2 operator - (const Vec2 &rval) const { return Vec2( v2[0] - rval[0], v2[1] - rval[1] ); }
	Vec2 operator * (const Vec2 &rval) const { return Vec2( v2[0] * rval[0], v2[1] * rval[1] ); }
	Vec2 operator / (const Vec2 &rval) const { return Vec2( v2[0] / rval[0], v2[1] / rval[1] ); }

	Vec2 operator -() const	{ return Vec2( -v2[0], -v2[1] ); }

	Vec2 operator +=(const Vec2 &rval)	{ *this = *this + rval; return *this; }

	const _S &x() const { return v2[0];	}
	const _S &y() const { return v2[1];	}
	_S &x() { return v2[0];	}
	_S &y() { return v2[1];	}

	const _S &operator [] (size_t i) const	{ return v2[i]; }
		  _S &operator [] (size_t i)		{ return v2[i]; }
};

//==================================================================
/// Vec3
//==================================================================
template <class _S>
class Vec3
{
public:
	_S	v3[3];

	//==================================================================
	Vec3()											{}
	Vec3( const Vec3 &v_ )							{ v3[0] = v_[0]; v3[1] = v_[1]; v3[2] = v_[2]; }

	template <class _T>
	Vec3( const Vec3<_T> &v_ )						{ v3[0] = v_[0]; v3[1] = v_[1]; v3[2] = v_[2]; }

	Vec3( const _S& a_ )							{ v3[0] = a_; v3[1] = a_; v3[2] = a_; }
	Vec3( const _S& x_, const _S& y_, const _S& z_ ){ v3[0] = x_; v3[1] = y_; v3[2] = z_; }
	Vec3( const _S *p_ )							{ v3[0] = p_[0]; v3[1] = p_[1]; v3[2] = p_[2]; }

	void Set( const _S& x_, const _S& y_, const _S& z_ )
	{
		v3[0] = x_;
		v3[1] = y_;
		v3[2] = z_;
	}

	void Set( const _S *p_ )
	{
		v3[0] = p_[0];
		v3[1] = p_[1];
		v3[2] = p_[2];
	}

	void SetZero()
	{
		v3[0] = v3[1] = v3[2] = 0.f;
	}

	Vec3 operator + (const _S& rval) const { return Vec3( v3[0] + rval, v3[1] + rval, v3[2] + rval ); }
	Vec3 operator - (const _S& rval) const { return Vec3( v3[0] - rval, v3[1] - rval, v3[2] - rval ); }
	Vec3 operator * (const _S& rval) const { return Vec3( v3[0] * rval, v3[1] * rval, v3[2] * rval ); }
	Vec3 operator / (const _S& rval) const { return Vec3( v3[0] / rval, v3[1] / rval, v3[2] / rval ); }
	Vec3 operator + (const Vec3 &rval) const { return Vec3( v3[0] + rval[0], v3[1] + rval[1], v3[2] + rval[2] ); }
	Vec3 operator - (const Vec3 &rval) const { return Vec3( v3[0] - rval[0], v3[1] - rval[1], v3[2] - rval[2] ); }
	Vec3 operator * (const Vec3 &rval) const { return Vec3( v3[0] * rval[0], v3[1] * rval[1], v3[2] * rval[2] ); }
	Vec3 operator / (const Vec3 &rval) const { return Vec3( v3[0] / rval[0], v3[1] / rval[1], v3[2] / rval[2] ); }

	Vec3 operator -() const	{ return Vec3( -v3[0], -v3[1], -v3[2] ); }

	Vec3 operator +=(const Vec3 &rval)	{ *this = *this + rval; return *this; }

	Vec3 GetNormalized() const
	{
		return *this * DRSqrt( v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2] );
	}

	Vec3 GetCross( const Vec3 &r_ ) const
	{
		return Vec3(
			v3[1] * r_[2] - v3[2] * r_[1],
			v3[2] * r_[0] - v3[0] * r_[2],
			v3[0] * r_[1] - v3[1] * r_[0]
		);
	}

	_S GetDot( const Vec3 &r_ ) const
	{
		return  v3[0] * r_[0] + v3[1] * r_[1] + v3[2] * r_[2];
	}

	friend Vec3	DAbs( const Vec3 &a )
	{
		return Vec3(DAbs( a.v3[0] ),
					DAbs( a.v3[1] ),
					DAbs( a.v3[2] ) );
	}

	friend Vec3	DMin( const Vec3 &a, const Vec3 &b )
	{
		return Vec3(DMin( a.v3[0], b.v3[0] ),
					DMin( a.v3[1], b.v3[1] ),
					DMin( a.v3[2], b.v3[2] ) );
	}

	friend Vec3	DMax( const Vec3 &a, const Vec3 &b )
	{
		return Vec3(DMax( a.v3[0], b.v3[0] ),
					DMax( a.v3[1], b.v3[1] ),
					DMax( a.v3[2], b.v3[2] ) );
	}

	const _S &x() const { return v3[0];	}
	const _S &y() const { return v3[1];	}
	const _S &z() const { return v3[2];	}
	_S &x() { return v3[0];	}
	_S &y() { return v3[1];	}
	_S &z() { return v3[2];	}

	const _S &operator [] (size_t i) const	{ return v3[i]; }
		  _S &operator [] (size_t i)		{ return v3[i]; }
};

//==================================================================
/// Vec4
//==================================================================
template <class _S>
class Vec4
{
public:
	_S	v4[4];

	//==================================================================
	Vec4()												{}
	Vec4( const Vec4 &v_ )								{ v4[0] = v_[0]; v4[1] = v_[1]; v4[2] = v_[2]; v4[3] = v_[3]; }

	template <class _T>
	Vec4( const Vec4<_T> &v_ )							{ v4[0] = v_[0]; v4[1] = v_[1]; v4[2] = v_[2]; v4[3] = v_[3]; }

	Vec4( const _S& a_ )								{ v4[0] = a_; v4[1] = a_; v4[2] = a_; v4[3] = a_;			  }
	Vec4( const _S& x_, const _S& y_, const _S& z_, const _S& w_ )	{ v4[0] = x_; v4[1] = y_; v4[2] = z_; v4[3] = w_; }
	Vec4( const _S *p_ )								{ v4[0] = p_[0]; v4[1] = p_[1]; v4[2] = p_[2]; v4[3] = p_[3]; }

	void Set( const _S& x_, const _S& y_, const _S& z_, const _S& w_ )
	{
		v4[0] = x_;
		v4[1] = y_;
		v4[2] = z_;
		v4[3] = w_;
	}

	void Set( const _S *p_ )
	{
		v4[0] = p_[0];
		v4[1] = p_[1];
		v4[2] = p_[2];
		v4[3] = p_[3];
	}

	void SetZero()
	{
		v4[0] = v4[1] = v4[2] = v4[3] = 0.f;
	}

	Vec3<_S> GetAsV3() const
	{
		return Vec3<_S>( v4 );
	}

	Vec4 operator + (const _S& rval) const { return Vec4( v4[0] + rval, v4[1] + rval, v4[2] + rval, v4[3] + rval ); }
	Vec4 operator - (const _S& rval) const { return Vec4( v4[0] - rval, v4[1] - rval, v4[2] - rval, v4[3] - rval ); }
	Vec4 operator * (const _S& rval) const { return Vec4( v4[0] * rval, v4[1] * rval, v4[2] * rval, v4[3] * rval ); }
	Vec4 operator / (const _S& rval) const { return Vec4( v4[0] / rval, v4[1] / rval, v4[2] / rval, v4[3] / rval ); }
	Vec4 operator + (const Vec4 &rval) const { return Vec4( v4[0] + rval.v4[0], v4[1] + rval.v4[1], v4[2] + rval.v4[2], v4[3] + rval.v4[3] ); }
	Vec4 operator - (const Vec4 &rval) const { return Vec4( v4[0] - rval.v4[0], v4[1] - rval.v4[1], v4[2] - rval.v4[2], v4[3] - rval.v4[3] ); }
	Vec4 operator * (const Vec4 &rval) const { return Vec4( v4[0] * rval.v4[0], v4[1] * rval.v4[1], v4[2] * rval.v4[2], v4[3] * rval.v4[3] ); }
	Vec4 operator / (const Vec4 &rval) const { return Vec4( v4[0] / rval.v4[0], v4[1] / rval.v4[1], v4[2] / rval.v4[2], v4[3] / rval.v4[3] ); }

	Vec4 operator -() const	{ return Vec4( -v4[0], -v4[1], -v4[2], -v4[3] ); }

	Vec4 operator +=(const Vec4 &rval)	{ *this = *this + rval; return *this; }

	const _S &x() const { return v4[0];	}
	const _S &y() const { return v4[1];	}
	const _S &z() const { return v4[2];	}
	const _S &w() const { return v4[3];	}
	_S &x() { return v4[0];	}
	_S &y() { return v4[1];	}
	_S &z() { return v4[2];	}
	_S &w() { return v4[3];	}

	const _S &operator [] (size_t i) const	{ return v4[i]; }
		  _S &operator [] (size_t i)		{ return v4[i]; }
};

//==================================================================
template<class _S> Vec2<_S> operator + (const _S &lval, const Vec2<_S> &rval) { return rval + lval; }
template<class _S> Vec2<_S> operator - (const _S &lval, const Vec2<_S> &rval) { return rval - lval; }
template<class _S> Vec2<_S> operator * (const _S &lval, const Vec2<_S> &rval) { return rval * lval; }
template<class _S> Vec2<_S> operator / (const _S &lval, const Vec2<_S> &rval) { return rval / lval; }

template<class _S> Vec3<_S> operator + (const _S &lval, const Vec3<_S> &rval) { return rval + lval; }
template<class _S> Vec3<_S> operator - (const _S &lval, const Vec3<_S> &rval) { return rval - lval; }
template<class _S> Vec3<_S> operator * (const _S &lval, const Vec3<_S> &rval) { return rval * lval; }
template<class _S> Vec3<_S> operator / (const _S &lval, const Vec3<_S> &rval) { return rval / lval; }

template<class _S> Vec4<_S> operator + (const _S &lval, const Vec4<_S> &rval) { return rval + lval; }
template<class _S> Vec4<_S> operator - (const _S &lval, const Vec4<_S> &rval) { return rval - lval; }
template<class _S> Vec4<_S> operator * (const _S &lval, const Vec4<_S> &rval) { return rval * lval; }
template<class _S> Vec4<_S> operator / (const _S &lval, const Vec4<_S> &rval) { return rval / lval; }

//==================================================================
/// float implementations
//==================================================================
typedef	Vec2<float>							Vec2f;
typedef	Vec3<float>							Vec3f;
typedef	Vec4<float>							Vec4f;
typedef	VecN<float,DMT_SIMD_FLEN>			VecSIMDf;
typedef	VecN<int,DMT_SIMD_FLEN>				VecSIMDi;

typedef	Vec2< VecN<float,DMT_SIMD_FLEN> >	Vec2xSIMDf;
typedef	Vec3< VecN<float,DMT_SIMD_FLEN> >	Vec3xSIMDf;
typedef	Vec4< VecN<float,DMT_SIMD_FLEN> >	Vec4xSIMDf;

//==================================================================
typedef	Vec2f		Point2;
typedef	Vec3f		Point3;
typedef	Vec4f		Point4;
typedef	Vec3f		Color;

#endif