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

//==================================================================
//__declspec(align(64))
class Matrix44
{
public:
	union {
		float	m16[16];
		float	m44[4][4];
	} u;

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
		u.m44[0][0] = m00_; u.m44[0][1] = m01_; u.m44[0][2] = m02_; u.m44[0][3] = m03_;
		u.m44[1][0] = m10_; u.m44[1][1] = m11_; u.m44[1][2] = m12_; u.m44[1][3] = m13_;
		u.m44[2][0] = m20_; u.m44[2][1] = m21_; u.m44[2][2] = m22_; u.m44[2][3] = m23_;
		u.m44[3][0] = m30_; u.m44[3][1] = m31_; u.m44[3][2] = m32_; u.m44[3][3] = m33_;
	}

	Vec3f GetV3( size_t idx ) const
	{
		return Vec3f( u.m44[idx] );
	}

	void SetV3( size_t idx, const Vec3f &v )
	{
		u.m44[idx][0] = v.v3[0];
		u.m44[idx][1] = v.v3[1];
		u.m44[idx][2] = v.v3[2];
	}

	void Identity()
	{
		memset( u.m16, 0, sizeof(float) * 16 );
		u.m44[0][0] = u.m44[1][1] = u.m44[2][2] = u.m44[3][3] = 1.0f;
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
		memcpy( u.m16, pSrcMtx, sizeof(float) * 16 );
	}
	
	Vec3f GetTranslation() const
	{
		return Vec3f(
			u.m44[3][0],
			u.m44[3][1],
			u.m44[3][2]
		);
	}

	void PrintOut() const;

	Matrix44 operator + (const Matrix44 &rval) const { Matrix44 m_; for(int i=0;i<16;++i) m_.u.m16[i]=u.m16[i]+rval.u.m16[i]; return m_; }
	Matrix44 operator - (const Matrix44 &rval) const { Matrix44 m_; for(int i=0;i<16;++i) m_.u.m16[i]=u.m16[i]-rval.u.m16[i]; return m_; }
//	Matrix44 operator * (const Matrix44 &rval) const { Matrix44 m_; for(int i=0;i<16;++i) m_.u.m16[i]=u.m16[i]*rval.u.m16[i]; return m_; }
	Matrix44 operator / (const Matrix44 &rval) const { Matrix44 m_; for(int i=0;i<16;++i) m_.u.m16[i]=u.m16[i]/rval.u.m16[i]; return m_; }

	Matrix44 operator *  (const float rval) const	 { Matrix44 m_; for(int i=0;i<16;++i) m_.u.m16[i]=u.m16[i]*rval; return m_; }
	void	 operator *= (const float rval)			 { for(int i=0;i<16;++i) u.m16[i] *= u.m16[i]*rval; }
};

//==================================================================
inline Matrix44 Matrix44::GetTranspose() const
{
	Matrix44	out;

	for (int i=0; i < 4; ++i)
		for (int j=0; j < 4; ++j)
			out.u.m44[i][j] = u.m44[j][i];

	return out;
}

//==================================================================
inline Matrix44 Matrix44::GetAs33() const
{
	Matrix44	out( true );

	for (int i=0; i < 3; ++i)
		for (int j=0; j < 3; ++j)
			out.u.m44[i][j] = u.m44[i][j];

	return out;
}

//==================================================================
inline Matrix44 Matrix44::GetOrthonormal() const
{
	// TODO: verify that this actually works !!
	Matrix44	out( true );

	Vec3f	v0 = GetV3(0);
	Vec3f	v1 = GetV3(1);
	Vec3f	v2 = GetV3(2);

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
	memset( m.u.m16, 0, sizeof(float) * 16 );
	m.u.m44[0][0] = sx;
	m.u.m44[1][1] = sy;
	m.u.m44[2][2] = sz;
	m.u.m44[3][3] = 1.0f;
	
	return m;
}
//==================================================================
inline Matrix44 Matrix44::Translate( float tx, float ty, float tz )
{
	Matrix44	m( true );
	m.u.m44[3][0] = tx;
	m.u.m44[3][1] = ty;
	m.u.m44[3][2] = tz;
	m.u.m44[3][3] = 1.0f;
	
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
				sum += m1.u.m44[i][c] * m2.u.m44[r][i];
				#else
				sum += m1.u.m44[r][i] * m2.u.m44[i][c];
				#endif

			tmp.u.m44[r][c] = sum;
		}
		
	}
	return tmp;
}

//==================================================================
inline Vec4f V4__M44_Mul_V3W1( const Matrix44 &a, const Vec3f &v )
{
	float	x = v.v3[0], y = v.v3[1], z = v.v3[2];

	return Vec4f(
#ifdef DMATRIX44_ROWMTX_MODE
		a.u.m44[0][0] * x + a.u.m44[1][0] * y + a.u.m44[2][0] * z + a.u.m44[3][0],
		a.u.m44[0][1] * x + a.u.m44[1][1] * y + a.u.m44[2][1] * z + a.u.m44[3][1],
		a.u.m44[0][2] * x + a.u.m44[1][2] * y + a.u.m44[2][2] * z + a.u.m44[3][2],
		a.u.m44[0][3] * x + a.u.m44[1][3] * y + a.u.m44[2][3] * z + a.u.m44[3][3]
#else
		a.u.m44[0][0] * x + a.u.m44[0][1] * y + a.u.m44[0][2] * z + a.u.m44[0][3],
		a.u.m44[1][0] * x + a.u.m44[1][1] * y + a.u.m44[1][2] * z + a.u.m44[1][3],
		a.u.m44[2][0] * x + a.u.m44[2][1] * y + a.u.m44[2][2] * z + a.u.m44[2][3],
		a.u.m44[3][0] * x + a.u.m44[3][1] * y + a.u.m44[3][2] * z + a.u.m44[3][3]
#endif
	);
}

//==================================================================
inline Vec3f V3__M44_Mul_V3W1( const Matrix44 &a, const Vec3f &v )
{
	float	x = v.v3[0], y = v.v3[1], z = v.v3[2];

	return Vec3f(
#ifdef DMATRIX44_ROWMTX_MODE
		a.u.m44[0][0] * x + a.u.m44[1][0] * y + a.u.m44[2][0] * z + a.u.m44[3][0],
		a.u.m44[0][1] * x + a.u.m44[1][1] * y + a.u.m44[2][1] * z + a.u.m44[3][1],
		a.u.m44[0][2] * x + a.u.m44[1][2] * y + a.u.m44[2][2] * z + a.u.m44[3][2]
#else
		a.u.m44[0][0] * x + a.u.m44[0][1] * y + a.u.m44[0][2] * z + a.u.m44[0][3],
		a.u.m44[1][0] * x + a.u.m44[1][1] * y + a.u.m44[1][2] * z + a.u.m44[1][3],
		a.u.m44[2][0] * x + a.u.m44[2][1] * y + a.u.m44[2][2] * z + a.u.m44[2][3]
#endif
	);
}

//==================================================================
template <class _S4, class _S3, class _T>
inline _S4 V4__V3W1_Mul_M44( const _S3 &v, const Matrix44 &a )
{
	const _T	&x = v.v3[0];
	const _T	&y = v.v3[1];
	const _T	&z = v.v3[2];

	return _S4(
#ifdef DMATRIX44_ROWMTX_MODE
		x * a.u.m44[0][0] + y * a.u.m44[0][1] + z * a.u.m44[0][2] + a.u.m44[0][3],
		x * a.u.m44[1][0] + y * a.u.m44[1][1] + z * a.u.m44[1][2] + a.u.m44[1][3],
		x * a.u.m44[2][0] + y * a.u.m44[2][1] + z * a.u.m44[2][2] + a.u.m44[2][3],
		x * a.u.m44[3][0] + y * a.u.m44[3][1] + z * a.u.m44[3][2] + a.u.m44[3][3]
#else											 
		x * a.u.m44[0][0] + y * a.u.m44[1][0] + z * a.u.m44[2][0] + a.u.m44[3][0],
		x * a.u.m44[0][1] + y * a.u.m44[1][1] + z * a.u.m44[2][1] + a.u.m44[3][1],
		x * a.u.m44[0][2] + y * a.u.m44[1][2] + z * a.u.m44[2][2] + a.u.m44[3][2],
		x * a.u.m44[0][3] + y * a.u.m44[1][3] + z * a.u.m44[2][3] + a.u.m44[3][3]
#endif
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
#ifdef DMATRIX44_ROWMTX_MODE
		x * a.u.m44[0][0] + y * a.u.m44[0][1] + z * a.u.m44[0][2] + a.u.m44[0][3],
		x * a.u.m44[1][0] + y * a.u.m44[1][1] + z * a.u.m44[1][2] + a.u.m44[1][3],
		x * a.u.m44[2][0] + y * a.u.m44[2][1] + z * a.u.m44[2][2] + a.u.m44[2][3]
#else											 
		x * a.u.m44[0][0] + y * a.u.m44[1][0] + z * a.u.m44[2][0] + a.u.m44[3][0],
		x * a.u.m44[0][1] + y * a.u.m44[1][1] + z * a.u.m44[2][1] + a.u.m44[3][1],
		x * a.u.m44[0][2] + y * a.u.m44[1][2] + z * a.u.m44[2][2] + a.u.m44[3][2]
#endif
	);
}

#endif