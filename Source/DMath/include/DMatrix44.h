//==================================================================
/// DMatrix44.h
///
/// Created by Davide Pasca - 2008/12/22
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMATRIX44_H
#define DMATRIX44_H

#include <string.h>
#include <math.h>
#include "DMath/include/DVector.h"
#include "DSystem/include/DTypes.h"
#include "DSystem/include/DUtils.h"

//==================================================================
//#define DMATRIX44_ROWMTX_MODE

#define DMT_MTX44_USE_VEC_TEMPLATE

//==================================================================
//__declspec(align(64))
#if defined(_MSC_VER)
__declspec(align(DMT_SIMD_ALIGN_SIZE))
#endif
class Matrix44
{
public:
#ifdef DMT_MTX44_USE_VEC_TEMPLATE
	VecN<float,16>	v16;
#else
	float	v16[16];
#endif

	Matrix44()
	{
	}

	Matrix44( bool setToIdentity )
	{
		if ( setToIdentity )
			Identity();
	}

	Matrix44( const float *pSrcMtx )
	{
		CopyRowMajor( pSrcMtx );
	}

	Matrix44(
		float m00_, float m01_, float m02_, float m03_,
		float m10_, float m11_, float m12_, float m13_,
		float m20_, float m21_, float m22_, float m23_,
		float m30_, float m31_, float m32_, float m33_ )
	{
		mij(0,0) = m00_; mij(0,1) = m01_; mij(0,2) = m02_; mij(0,3) = m03_;
		mij(1,0) = m10_; mij(1,1) = m11_; mij(1,2) = m12_; mij(1,3) = m13_;
		mij(2,0) = m20_; mij(2,1) = m21_; mij(2,2) = m22_; mij(2,3) = m23_;
		mij(3,0) = m30_; mij(3,1) = m31_; mij(3,2) = m32_; mij(3,3) = m33_;
	}

	const float &mij( size_t y, size_t x ) const	{ return v16[ y * 4 + x ];	}
		  float &mij( size_t y, size_t x )			{ return v16[ y * 4 + x ];	}

	Float3 GetV3( size_t idx ) const
	{
		return Float3( mij(idx,0), mij(idx,1), mij(idx,2) );
	}

	void SetV3( size_t idx, const Float3 &v )
	{
		mij(idx,0) = v.v3[0];
		mij(idx,1) = v.v3[1];
		mij(idx,2) = v.v3[2];
	}

	void Identity()
	{
	#ifdef DMT_MTX44_USE_VEC_TEMPLATE
		v16 = VecN<float,16>( 0.f );
	#else
		memset( v16, 0, sizeof(v16) );
	#endif

		mij(0,0) = mij(1,1) = mij(2,2) = mij(3,3) = 1.0f;
	}
		
	inline Matrix44 GetTranspose() const;
	inline Matrix44 GetAs33() const;
	inline Matrix44 GetOrthonormal() const;

	Matrix44 GetInverse( bool *out_pSuccess=0 ) const;

	inline static Matrix44 Scale( float sx, float sy, float sz );
	inline static Matrix44 Translate( float tx, float ty, float tz );
	inline static Matrix44 Rot( float ang, float ax, float ay, float az );
	inline static Matrix44 Perspective( float fov, float aspect, float n, float f );
	inline static Matrix44 PerspectiveRH0( float fov, float aspect, float n, float f );

	void CopyRowMajor( const float *pSrcMtx )
	{
	#ifdef DMT_MTX44_USE_VEC_TEMPLATE
		v16 = VecN<float,16>( pSrcMtx );
	#else
		memcpy( v16, pSrcMtx, sizeof(v16) );
	#endif
	}
	
	Float3 GetTranslation() const
	{
		return Float3(
			mij(3,0),
			mij(3,1),
			mij(3,2)
		);
	}

	void PrintOut() const;

#ifdef DMT_MTX44_USE_VEC_TEMPLATE
	Matrix44 operator + (const Matrix44 &rval) const { Matrix44 ret; ret.v16 = v16 + rval.v16; return ret; }
	Matrix44 operator - (const Matrix44 &rval) const { Matrix44 ret; ret.v16 = v16 - rval.v16; return ret; }
//	Matrix44 operator * (const Matrix44 &rval) const { Matrix44 ret; ret.v16 = v16 * rval.v16; return ret; }
	Matrix44 operator / (const Matrix44 &rval) const { Matrix44 ret; ret.v16 = v16 / rval.v16; return ret; }

	Matrix44 operator *  (const float rval) const	 { Matrix44 ret; ret.v16 = v16 * rval; return ret; }
#else
	Matrix44 operator *  (const float rval) const	 { Matrix44 ret; for (size_t i=0; i < 16; ++i) ret.v16[i] = v16[i] * rval; return ret; }
#endif
	//void	 operator *= (const float rval)			 { v16 *= rval; }

#ifdef DMT_MTX44_USE_VEC_TEMPLATE
	friend bool operator ==( const Matrix44 &lval, const Matrix44 &rval ) { return lval.v16 == rval.v16; }
	friend bool operator !=( const Matrix44 &lval, const Matrix44 &rval ) { return lval.v16 != rval.v16; }
#endif
}
#if defined(__GNUC__)
__attribute__ ((aligned(DMT_SIMD_ALIGN_SIZE)))
#endif
;

//==================================================================
inline Matrix44 Matrix44::GetTranspose() const
{
	Matrix44	out;

	for (int i=0; i < 4; ++i)
		for (int j=0; j < 4; ++j)
			out.mij(i,j) = mij(j,i);

	return out;
}

//==================================================================
inline Matrix44 Matrix44::GetAs33() const
{
	Matrix44	out( true );

	for (int i=0; i < 3; ++i)
		for (int j=0; j < 3; ++j)
			out.mij(i,j) = mij(i,j);

	return out;
}

//==================================================================
inline Matrix44 Matrix44::GetOrthonormal() const
{
	// TODO: verify that this actually works !!
	Matrix44	out( true );

	Float3	v0 = GetV3(0);
	Float3	v1 = GetV3(1);
	Float3	v2 = GetV3(2);

	v0 = v0.GetNormalized(); 
	v1 = v2.GetCross( v0 );
	v1 = v1.GetNormalized(); 
	v2 = v0.GetCross( v1 );

	out.SetV3( 0, v0 );
	out.SetV3( 1, v1 );
	out.SetV3( 2, v2 );
	out.SetV3( 3, GetV3(3) );

	return out;
}

//==================================================================
inline Matrix44 Matrix44::Scale( float sx, float sy, float sz )
{
	Matrix44	m;
#ifdef DMT_MTX44_USE_VEC_TEMPLATE
	m.v16 = VecN<float,16>( 0.f );
#else
	memset( m.v16, 0, sizeof(m.v16) );
#endif
	m.mij(0,0) = sx;
	m.mij(1,1) = sy;
	m.mij(2,2) = sz;
	m.mij(3,3) = 1.0f;
	
	return m;
}
//==================================================================
inline Matrix44 Matrix44::Translate( float tx, float ty, float tz )
{
	Matrix44	m( true );
	m.mij(3,0) = tx;
	m.mij(3,1) = ty;
	m.mij(3,2) = tz;
	m.mij(3,3) = 1.0f;
	
	return m;
}
//==================================================================
inline Matrix44 Matrix44::Rot( float ang, float ax, float ay, float az )
{
	float   xx, yy, zz, xy, yz, zx, xs, ys, zs;

	float s = sinf( ang );
	float c = cosf( ang );

	xx = ax * ax;   yy = ay * ay;   zz = az * az;
	xy = ax * ay;   yz = ay * az;   zx = az * ax;
	xs = ax * s;    ys = ay * s;    zs = az * s;
	float one_c = 1 - c;

	return Matrix44(
			(one_c * xx) + c,	(one_c * xy) + zs,	(one_c * zx) - ys,	0,
			(one_c * xy) - zs,	(one_c * yy) + c,	(one_c * yz) + xs,	0,
			(one_c * zx) + ys,	(one_c * yz) - xs,	(one_c * zz) + c,	0,
			0,					0,					0,					1 );
}

//==================================================================
inline Matrix44 Matrix44::Perspective( float fov, float aspect, float n, float f )
{
	float   ootan2 = tanf( fov * 0.5f );
	DASSERT( ootan2 != 0 );
	ootan2 = 1.0f / ootan2;

	DASSERT( f != n );

	return Matrix44(
			ootan2/aspect,	0,		0,			0,
			0,				ootan2,	0,			0,
			0,				0,		f/(f-n),	1,
			0,				0,		n*f/(n-f),	0 );
}

//==================================================================
inline Matrix44 Matrix44::PerspectiveRH0( float fov, float aspect, float n, float f )
{
	float   ootan2 = tanf( fov * 0.5f );
	DASSERT( ootan2 != 0 );
	ootan2 = 1.0f / ootan2;

	DASSERT( f != n );

	return Matrix44(
			ootan2/aspect,	0,		0,			0,
			0,				ootan2,	0,			0,
			0,				0,		f/(n-f),	-1,
			0,				0,		n*f/(n-f),	0 );
}

//==================================================================
inline Matrix44 operator * (const Matrix44 &m1, const Matrix44 &m2)
{
	Matrix44	tmp;
	for (size_t r=0; r < 4; ++r)
	{
		for (size_t c=0; c < 4; ++c)
		{
			float	sum = 0;
			for (size_t i=0; i < 4; ++i)
				#ifdef DMATRIX44_ROWMTX_MODE
				sum += m1.mij(i,c) * m2.mij(r,i);
				#else
				sum += m1.mij(r,i) * m2.mij(i,c);
				#endif

			tmp.mij(r,c) = sum;
		}
		
	}
	return tmp;
}

//==================================================================
inline Float4 V4__M44_Mul_V3W1( const Matrix44 &a, const Float3 &v )
{
	float	x = v.v3[0], y = v.v3[1], z = v.v3[2];

	return Float4(
		a.mij(0,0) * x + a.mij(0,1) * y + a.mij(0,2) * z + a.mij(0,3),
		a.mij(1,0) * x + a.mij(1,1) * y + a.mij(1,2) * z + a.mij(1,3),
		a.mij(2,0) * x + a.mij(2,1) * y + a.mij(2,2) * z + a.mij(2,3),
		a.mij(3,0) * x + a.mij(3,1) * y + a.mij(3,2) * z + a.mij(3,3)
	);
}

//==================================================================
inline Float3 V3__M44_Mul_V3W1( const Matrix44 &a, const Float3 &v )
{
	float	x = v.v3[0], y = v.v3[1], z = v.v3[2];

	return Float3(
		a.mij(0,0) * x + a.mij(0,1) * y + a.mij(0,2) * z + a.mij(0,3),
		a.mij(1,0) * x + a.mij(1,1) * y + a.mij(1,2) * z + a.mij(1,3),
		a.mij(2,0) * x + a.mij(2,1) * y + a.mij(2,2) * z + a.mij(2,3)
	);
}

//==================================================================
template <class _T>
inline Vec4<_T> V4__V3W1_Mul_M44( const Vec3<_T> &v, const Matrix44 &a )
{
	const _T	&x = v.v3[0];
	const _T	&y = v.v3[1];
	const _T	&z = v.v3[2];

	return Vec4<_T>(
		x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0) + a.mij(3,0),
		x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1) + a.mij(3,1),
		x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2) + a.mij(3,2),
		x * a.mij(0,3) + y * a.mij(1,3) + z * a.mij(2,3) + a.mij(3,3)
	);
}

//==================================================================
template <class _T>
inline Vec4<_T> V4__V4_Mul_M44( const Vec4<_T> &v, const Matrix44 &a )
{
	const _T	&x = v.v4[0];
	const _T	&y = v.v4[1];
	const _T	&z = v.v4[2];
	const _T	&w = v.v4[3];

	return Vec4<_T>(
		x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0) + w * a.mij(3,0),
		x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1) + w * a.mij(3,1),
		x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2) + w * a.mij(3,2),
		x * a.mij(0,3) + y * a.mij(1,3) + z * a.mij(2,3) + w * a.mij(3,3)
	);
}

//==================================================================
template <class _S4, class _S3, class _T>
inline _S4 V4__V3W0_Mul_M44( const _S3 &v, const Matrix44 &a )
{
	const _T	&x = v.v3[0];
	const _T	&y = v.v3[1];
	const _T	&z = v.v3[2];

	return _S4(
		x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0),
		x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1),
		x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2),
		x * a.mij(0,3) + y * a.mij(1,3) + z * a.mij(2,3)
	);
}

//==================================================================
template <class _S>
inline Vec3<_S> V3__V3W1_Mul_M44( const Vec3<_S> &v, const Matrix44 &a )
{
	const _S	&x = v.v3[0];
	const _S	&y = v.v3[1];
	const _S	&z = v.v3[2];

	return Vec3<_S>(
		x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0) + a.mij(3,0),
		x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1) + a.mij(3,1),
		x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2) + a.mij(3,2)
	);
}

//==================================================================
template <class _S>
inline Vec3<_S> V3__V3W0_Mul_M44( const Vec3<_S> &v, const Matrix44 &a )
{
	const _S	&x = v.v3[0];
	const _S	&y = v.v3[1];
	const _S	&z = v.v3[2];

	return Vec3<_S>(
		x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0),
		x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1),
		x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2)
	);
}

#endif
