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
template <class _TS>
class Vec2
{
public:
	_TS	v2[2];

	//==================================================================
	Vec2()								{}
	Vec2( const Vec2 &v )				{ v2[0] = v[0]; v2[1] = v[1];	}

	template <class _T>
	Vec2( const Vec2<_T> &v_ )			{ v2[0] = v_[0]; v2[1] = v_[1]; }

	Vec2( const _TS& a_ )				{ v2[0] = a_; v2[1] = a_;		}
	Vec2( const _TS& x_, const _TS& y_ )	{ v2[0] = x_; v2[1] = y_;		}
	Vec2( const _TS *p_ )				{ v2[0] = p_[0]; v2[1] = p_[1]; }

	void Set( const _TS& x_, const _TS& y_ )
	{
		v2[0] = x_;
		v2[1] = y_;
	}

	void Set( const _TS *p_ )
	{
		v2[0] = p_[0];
		v2[1] = p_[1];
	}

	void SetZero()
	{
		v2[0] = v2[1] = (_TS)0;
	}

	Vec2 operator + (const _TS& rval) const { return Vec2( v2[0] + rval, v2[1] + rval ); }
	Vec2 operator - (const _TS& rval) const { return Vec2( v2[0] - rval, v2[1] - rval ); }
	Vec2 operator * (const _TS& rval) const { return Vec2( v2[0] * rval, v2[1] * rval ); }
	Vec2 operator / (const _TS& rval) const { return Vec2( v2[0] / rval, v2[1] / rval ); }
	Vec2 operator + (const Vec2 &rval) const { return Vec2( v2[0] + rval[0], v2[1] + rval[1] ); }
	Vec2 operator - (const Vec2 &rval) const { return Vec2( v2[0] - rval[0], v2[1] - rval[1] ); }
	Vec2 operator * (const Vec2 &rval) const { return Vec2( v2[0] * rval[0], v2[1] * rval[1] ); }
	Vec2 operator / (const Vec2 &rval) const { return Vec2( v2[0] / rval[0], v2[1] / rval[1] ); }

	Vec2 operator -() const	{ return Vec2( -v2[0], -v2[1] ); }

	Vec2 operator +=(const Vec2 &rval)	{ *this = *this + rval; return *this; }
	Vec2 operator -=(const Vec2 &rval)	{ *this = *this - rval; return *this; }
	Vec2 operator *=(const Vec2 &rval)	{ *this = *this * rval; return *this; }


	_TS GetLengthSqr() const
	{
		return v2[0] * v2[0] + v2[1] * v2[1];
	}

	_TS GetLength() const
	{
		return DSqrt( GetLengthSqr() );
	}
	_TS GetDot( const Vec2 &r_ ) const
	{
		return  v2[0] * r_[0] + v2[1] * r_[1];
	}

	Vec2 GetNormalized() const
	{
		return *this * DRSqrt( v2[0] * v2[0] + v2[1] * v2[1]);
	}

	const _TS &x() const { return v2[0];	}
	const _TS &y() const { return v2[1];	}
	_TS &x() { return v2[0];	}
	_TS &y() { return v2[1];	}

	const _TS &operator [] (size_t i) const	{ return v2[i]; }
		  _TS &operator [] (size_t i)		{ return v2[i]; }
};

//==================================================================
/// Vec3
//==================================================================
template <class _TS>
class Vec3
{
public:
	_TS	v3[3];

	//==================================================================
	Vec3()											{}
	Vec3( const Vec3 &v_ )							{ v3[0] = v_[0]; v3[1] = v_[1]; v3[2] = v_[2]; }
	Vec3( const Vec2<_TS> &v_ )						{ v3[0] = v_[0]; v3[1] = v_[1]; v3[2] = 0; }

	template <class _T>
	Vec3( const Vec3<_T> &v_ )						{ v3[0] = v_[0]; v3[1] = v_[1]; v3[2] = v_[2]; }

	Vec3( const _TS& a_ )							{ v3[0] = a_; v3[1] = a_; v3[2] = a_; }
	Vec3( const _TS& x_, const _TS& y_, const _TS& z_ ){ v3[0] = x_; v3[1] = y_; v3[2] = z_; }
	Vec3( const _TS *p_ )							{ v3[0] = p_[0]; v3[1] = p_[1]; v3[2] = p_[2]; }

	void Set( const _TS& x_, const _TS& y_, const _TS& z_ )
	{
		v3[0] = x_;
		v3[1] = y_;
		v3[2] = z_;
	}

	void Set( const _TS *p_ )
	{
		v3[0] = p_[0];
		v3[1] = p_[1];
		v3[2] = p_[2];
	}

	void SetZero()
	{
		v3[0] = v3[1] = v3[2] = (_TS)0;
	}

	Vec3 operator + (const _TS& rval) const { return Vec3( v3[0] + rval, v3[1] + rval, v3[2] + rval ); }
	Vec3 operator - (const _TS& rval) const { return Vec3( v3[0] - rval, v3[1] - rval, v3[2] - rval ); }
	Vec3 operator * (const _TS& rval) const { return Vec3( v3[0] * rval, v3[1] * rval, v3[2] * rval ); }
	Vec3 operator / (const _TS& rval) const { return Vec3( v3[0] / rval, v3[1] / rval, v3[2] / rval ); }
	Vec3 operator + (const Vec3 &rval) const { return Vec3( v3[0] + rval[0], v3[1] + rval[1], v3[2] + rval[2] ); }
	Vec3 operator - (const Vec3 &rval) const { return Vec3( v3[0] - rval[0], v3[1] - rval[1], v3[2] - rval[2] ); }
	Vec3 operator * (const Vec3 &rval) const { return Vec3( v3[0] * rval[0], v3[1] * rval[1], v3[2] * rval[2] ); }
	Vec3 operator / (const Vec3 &rval) const { return Vec3( v3[0] / rval[0], v3[1] / rval[1], v3[2] / rval[2] ); }

	Vec3 operator -() const	{ return Vec3( -v3[0], -v3[1], -v3[2] ); }

	Vec3 operator +=(const Vec3 &rval)	{ *this = *this + rval; return *this; }
	Vec3 operator -=(const Vec3 &rval)	{ *this = *this - rval; return *this; }
	Vec3 operator *=(const Vec3 &rval)	{ *this = *this * rval; return *this; }

	friend bool operator ==( const Vec3 &lval, const Vec3 &rval ) { return (lval.v3[0] == rval.v3[0]) && (lval.v3[1] == rval.v3[1]) && (lval.v3[2] == rval.v3[2]); }
	friend bool operator !=( const Vec3 &lval, const Vec3 &rval ) { return (lval.v3[0] != rval.v3[0]) || (lval.v3[1] != rval.v3[1]) || (lval.v3[2] != rval.v3[2]); }

	bool IsSimilar( const Vec3 &r_, float eps ) const
	{
		Vec3	diff = DAbs( *this - r_ );

		return diff.x() < eps || diff.y() < eps || diff.z() < eps;
	}

	Vec3 GetNormalized() const
	{
		return *this * DRSqrt( v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2] );
	}

	_TS GetLengthSqr() const
	{
		return v3[0] * v3[0] + v3[1] * v3[1] + v3[2] * v3[2];
	}

	_TS GetLength() const
	{
		return DSqrt( GetLengthSqr() );
	}

	Vec3 GetCross( const Vec3 &r_ ) const
	{
		return Vec3(
			v3[1] * r_[2] - v3[2] * r_[1],
			v3[2] * r_[0] - v3[0] * r_[2],
			v3[0] * r_[1] - v3[1] * r_[0]
		);
	}

	_TS GetDot( const Vec3 &r_ ) const
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

	const _TS &x() const { return v3[0];	}
	const _TS &y() const { return v3[1];	}
	const _TS &z() const { return v3[2];	}
	_TS &x() { return v3[0];	}
	_TS &y() { return v3[1];	}
	_TS &z() { return v3[2];	}

	const _TS &operator [] (size_t i) const	{ return v3[i]; }
		  _TS &operator [] (size_t i)		{ return v3[i]; }
};

//==================================================================
/// Vec4
//==================================================================
template <class _TS>
class Vec4
{
public:
	_TS	v4[4];

	//==================================================================
	Vec4()												{}
	Vec4( const Vec4 &v_ )								{ v4[0] = v_[0]; v4[1] = v_[1]; v4[2] = v_[2]; v4[3] = v_[3]; }

	template <class _T>
	Vec4( const Vec3<_T> &v_, const _T& w_ )			{ v4[0] = v_[0]; v4[1] = v_[1]; v4[2] = v_[2]; v4[3] = w_; }

	template <class _T>
	Vec4( const Vec4<_T> &v_ )							{ v4[0] = v_[0]; v4[1] = v_[1]; v4[2] = v_[2]; v4[3] = v_[3]; }

	Vec4( const _TS& a_ )								{ v4[0] = a_; v4[1] = a_; v4[2] = a_; v4[3] = a_;			  }
	Vec4( const _TS& x_, const _TS& y_, const _TS& z_, const _TS& w_ )	{ v4[0] = x_; v4[1] = y_; v4[2] = z_; v4[3] = w_; }
	Vec4( const _TS *p_ )								{ v4[0] = p_[0]; v4[1] = p_[1]; v4[2] = p_[2]; v4[3] = p_[3]; }

	void Set( const _TS& x_, const _TS& y_, const _TS& z_, const _TS& w_ )
	{
		v4[0] = x_;
		v4[1] = y_;
		v4[2] = z_;
		v4[3] = w_;
	}

	void Set( const _TS *p_ )
	{
		v4[0] = p_[0];
		v4[1] = p_[1];
		v4[2] = p_[2];
		v4[3] = p_[3];
	}

	void SetZero()
	{
		v4[0] = v4[1] = v4[2] = v4[3] = (_TS)0;
	}

	Vec3<_TS> GetAsV3() const
	{
		return Vec3<_TS>( v4 );
	}

	Vec2<_TS> GetAsV2() const
	{
		return Vec2<_TS>( v4 );
	}

	Vec4 operator + (const _TS& rval) const { return Vec4( v4[0] + rval, v4[1] + rval, v4[2] + rval, v4[3] + rval ); }
	Vec4 operator - (const _TS& rval) const { return Vec4( v4[0] - rval, v4[1] - rval, v4[2] - rval, v4[3] - rval ); }
	Vec4 operator * (const _TS& rval) const { return Vec4( v4[0] * rval, v4[1] * rval, v4[2] * rval, v4[3] * rval ); }
	Vec4 operator / (const _TS& rval) const { return Vec4( v4[0] / rval, v4[1] / rval, v4[2] / rval, v4[3] / rval ); }
	Vec4 operator + (const Vec4 &rval) const { return Vec4( v4[0] + rval.v4[0], v4[1] + rval.v4[1], v4[2] + rval.v4[2], v4[3] + rval.v4[3] ); }
	Vec4 operator - (const Vec4 &rval) const { return Vec4( v4[0] - rval.v4[0], v4[1] - rval.v4[1], v4[2] - rval.v4[2], v4[3] - rval.v4[3] ); }
	Vec4 operator * (const Vec4 &rval) const { return Vec4( v4[0] * rval.v4[0], v4[1] * rval.v4[1], v4[2] * rval.v4[2], v4[3] * rval.v4[3] ); }
	Vec4 operator / (const Vec4 &rval) const { return Vec4( v4[0] / rval.v4[0], v4[1] / rval.v4[1], v4[2] / rval.v4[2], v4[3] / rval.v4[3] ); }

	Vec4 operator -() const	{ return Vec4( -v4[0], -v4[1], -v4[2], -v4[3] ); }

	Vec4 operator +=(const Vec4 &rval)	{ *this = *this + rval; return *this; }

	friend bool operator ==( const Vec4 &lval, const Vec4 &rval ) { return (lval.v4[0] == rval.v4[0]) && (lval.v4[1] == rval.v4[1]) && (lval.v4[2] == rval.v4[2]) && (lval.v4[3] == rval.v4[3]); }
	friend bool operator !=( const Vec4 &lval, const Vec4 &rval ) { return (lval.v4[0] != rval.v4[0]) || (lval.v4[1] != rval.v4[1]) || (lval.v4[2] != rval.v4[2]) || (lval.v4[3] != rval.v4[3]); }

	const _TS &x() const { return v4[0];	}
	const _TS &y() const { return v4[1];	}
	const _TS &z() const { return v4[2];	}
	const _TS &w() const { return v4[3];	}
	_TS &x() { return v4[0];	}
	_TS &y() { return v4[1];	}
	_TS &z() { return v4[2];	}
	_TS &w() { return v4[3];	}

	const _TS &operator [] (size_t i) const	{ return v4[i]; }
		  _TS &operator [] (size_t i)		{ return v4[i]; }
};

//==================================================================
template<class _TS> Vec2<_TS> operator + (const _TS &lval, const Vec2<_TS> &rval) { return rval + lval; }
template<class _TS> Vec2<_TS> operator - (const _TS &lval, const Vec2<_TS> &rval) { return rval - lval; }
template<class _TS> Vec2<_TS> operator * (const _TS &lval, const Vec2<_TS> &rval) { return rval * lval; }
template<class _TS> Vec2<_TS> operator / (const _TS &lval, const Vec2<_TS> &rval) { return rval / lval; }

template<class _TS> Vec3<_TS> operator + (const _TS &lval, const Vec3<_TS> &rval) { return rval + lval; }
template<class _TS> Vec3<_TS> operator - (const _TS &lval, const Vec3<_TS> &rval) { return rval - lval; }
template<class _TS> Vec3<_TS> operator * (const _TS &lval, const Vec3<_TS> &rval) { return rval * lval; }
template<class _TS> Vec3<_TS> operator / (const _TS &lval, const Vec3<_TS> &rval) { return rval / lval; }

template<class _TS> Vec4<_TS> operator + (const _TS &lval, const Vec4<_TS> &rval) { return rval + lval; }
template<class _TS> Vec4<_TS> operator - (const _TS &lval, const Vec4<_TS> &rval) { return rval - lval; }
template<class _TS> Vec4<_TS> operator * (const _TS &lval, const Vec4<_TS> &rval) { return rval * lval; }
template<class _TS> Vec4<_TS> operator / (const _TS &lval, const Vec4<_TS> &rval) { return rval / lval; }

template<class _TS>
VecNMask CmpMaskEQ( const Vec3<_TS> &lval, const Vec3<_TS> &rval )
{
	return	  CmpMaskEQ( lval[0], rval[0] )
			& CmpMaskEQ( lval[1], rval[1] )
			& CmpMaskEQ( lval[2], rval[2] );
}

template<class _TS>
VecNMask CmpMaskNE( const Vec3<_TS> &lval, const Vec3<_TS> &rval )
{
	return	  CmpMaskNE( lval[0], rval[0] )
			| CmpMaskNE( lval[1], rval[1] )
			| CmpMaskNE( lval[2], rval[2] );
}

template<class _TS>
VecNMask CmpMaskEQ( const Vec4<_TS> &lval, const Vec4<_TS> &rval )
{
	return	  CmpMaskEQ( lval[0], rval[0] )
			& CmpMaskEQ( lval[1], rval[1] )
			& CmpMaskEQ( lval[2], rval[2] )
			& CmpMaskEQ( lval[3], rval[3] );
}

template<class _TS>
VecNMask CmpMaskNE( const Vec4<_TS> &lval, const Vec4<_TS> &rval )
{
	return	  CmpMaskNE( lval[0], rval[0] )
			| CmpMaskNE( lval[1], rval[1] )
			| CmpMaskNE( lval[2], rval[2] )
			| CmpMaskNE( lval[3], rval[3] );
}

//===============================================================
#define _DTPL template <class _TS> DFORCEINLINE
_DTPL _TS		DDot( const Vec2<_TS>& a, const Vec2<_TS>& b ) { return a[0] * b[0] + a[1] * b[1]; }
_DTPL _TS		DDot( const Vec3<_TS>& a, const Vec3<_TS>& b ) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
_DTPL _TS		DDot( const Vec4<_TS>& a, const Vec4<_TS>& b ) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2] + a[3] * b[3]; }
_DTPL _TS		DLengthSqr( const Vec2<_TS>& a ) { return DDot( a, a ); }
_DTPL _TS		DLengthSqr( const Vec3<_TS>& a ) { return DDot( a, a ); }
_DTPL _TS		DLengthSqr( const Vec4<_TS>& a ) { return DDot( a, a ); }
_DTPL _TS		DLength( const Vec2<_TS>& a ) { return DSqrt( DLengthSqr( a ) ); }
_DTPL _TS		DLength( const Vec3<_TS>& a ) { return DSqrt( DLengthSqr( a ) ); }
_DTPL _TS		DLength( const Vec4<_TS>& a ) { return DSqrt( DLengthSqr( a ) ); }
_DTPL Vec2<_TS> DNormalize( const Vec2<_TS>& v ) { return v * DRSqrt( DLengthSqr( v ) ); }
_DTPL Vec3<_TS> DNormalize( const Vec3<_TS>& v ) { return v * DRSqrt( DLengthSqr( v ) ); }
_DTPL Vec4<_TS> DNormalize( const Vec4<_TS>& v ) { return v * DRSqrt( DLengthSqr( v ) ); }
_DTPL Vec3<_TS> DCross( const Vec3<_TS>& a, const Vec3<_TS>& b ) { return Vec3<_TS>(
																		a[1] * b[2] - a[2] * b[1],
																		a[2] * b[0] - a[0] * b[2],
																		a[0] * b[1] - a[1] * b[0] ); }
#undef _DTPL


//==================================================================
/// float implementations
//==================================================================
//==================================================================
#if defined(_MSC_VER)
#define DVECTOR_SIMD_ALIGN( _X_ )				__declspec(align(DMT_SIMD_ALIGN_SIZE))	_X_
#define DVECTOR_SIMD_ALIGN_ARRAY( _X_ )			__declspec(align(DMT_SIMD_ALIGN_SIZE))	_X_ []
#define DVECTOR_SIMD_ALIGN_ARRAYN( _X_, _N_ )	__declspec(align(DMT_SIMD_ALIGN_SIZE))	_X_ [_N_]

typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) VecN<float,DMT_SIMD_FLEN>			Float_;
typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) VecN<int,DMT_SIMD_FLEN>			Int_;
typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) VecN<DU8,DMT_SIMD_FLEN>			Bool_;

typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) Vec2< VecN<float,DMT_SIMD_FLEN> >	Float2_;
typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) Vec3< VecN<float,DMT_SIMD_FLEN> >	Float3_;
typedef __declspec(align(DMT_SIMD_ALIGN_SIZE)) Vec4< VecN<float,DMT_SIMD_FLEN> >	Float4_;

#elif defined(__GNUC__)
#define DVECTOR_SIMD_ALIGN( _X_ )				_X_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)))
#define DVECTOR_SIMD_ALIGN_ARRAY( _X_ )			_X_ [] __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)))
#define DVECTOR_SIMD_ALIGN_ARRAYN( _X_, _N_ )	_X_ [_N_] __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)))

typedef	VecN<float,DMT_SIMD_FLEN>			Float_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));
typedef	VecN<int,DMT_SIMD_FLEN>				Int_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));
typedef	VecN<DU8,DMT_SIMD_FLEN>				Bool_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));

typedef	Vec2< VecN<float,DMT_SIMD_FLEN> >	Float2_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));
typedef	Vec3< VecN<float,DMT_SIMD_FLEN> >	Float3_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));
typedef	Vec4< VecN<float,DMT_SIMD_FLEN> >	Float4_ __attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)));

#endif

typedef	Vec2<float>												Float2;
typedef	Vec3<float>												Float3;
typedef	Vec4<float>												Float4;

typedef	Vec2<int>												Int2;
typedef	Vec3<int>												Int3;
typedef	Vec4<int>												Int4;

//==================================================================
typedef	Float2		Point2;
typedef	Float3		Point3;
typedef	Float4		Point4;
typedef	Float3		Color;

#endif
