/*
 *  DMatrix44.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/22.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef DMATRIX44_H
#define DMATRIX44_H

#include <string.h>
#include <math.h>
#include "DVector.h"
#include "DTypes.h"

//==================================================================
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

	void Identity()
	{
		memset( u.m16, 0, sizeof(float) * 16 );
		u.m44[0][0] = u.m44[1][1] = u.m44[2][2] = u.m44[3][3] = 1.0f;
	}
		
	inline static Matrix44 Scale( float sx, float sy, float sz );
	inline static Matrix44 Translate( float tx, float ty, float tz );
	inline static Matrix44 Rot( float ang, float ax, float ay, float az );
	inline static Matrix44 Perspective( float fov, float aspect, float n, float f );

	void CopyRowMajor( const float *pSrcMtx )
	{
		memcpy( u.m16, pSrcMtx, sizeof(float) * 16 );
	}
	
	void PrintOut() const;
};

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
inline Matrix44 operator * (const Matrix44 &m1, const Matrix44 &m2)
{
	Matrix44	tmp;
	for (size_t r=0; r < 4; ++r)
	{
		for (size_t c=0; c < 4; ++c)
		{
			float	sum = 0;
			for (size_t i=0; i < 4; ++i)
				sum += m1.u.m44[r][i] * m2.u.m44[i][c];

			tmp.u.m44[r][c] = sum;
		}
		
	}
	return tmp;
}

//==================================================================
inline Vector4 operator * ( const Vector4 &v, const Matrix44 &a )
{
	float	x = v.x, y = v.y, z = v.z, w = v.w;

	return Vector4(
		a.u.m44[0][0] * x + a.u.m44[1][0] * y + a.u.m44[2][0] * z + a.u.m44[3][0] * w,
		a.u.m44[0][1] * x + a.u.m44[1][1] * y + a.u.m44[2][1] * z + a.u.m44[3][1] * w,
		a.u.m44[0][2] * x + a.u.m44[1][2] * y + a.u.m44[2][2] * z + a.u.m44[3][2] * w,
		a.u.m44[0][3] * x + a.u.m44[1][3] * y + a.u.m44[2][3] * z + a.u.m44[3][3] * w
	);
}

//==================================================================
inline Vector4 operator * ( const Vector3 &v, const Matrix44 &a )
{
	float	x = v.x, y = v.y, z = v.z;

	return Vector4(
		a.u.m44[0][0] * x + a.u.m44[1][0] * y + a.u.m44[2][0] * z + a.u.m44[3][0],
		a.u.m44[0][1] * x + a.u.m44[1][1] * y + a.u.m44[2][1] * z + a.u.m44[3][1],
		a.u.m44[0][2] * x + a.u.m44[1][2] * y + a.u.m44[2][2] * z + a.u.m44[3][2],
		a.u.m44[0][3] * x + a.u.m44[1][3] * y + a.u.m44[2][3] * z + a.u.m44[3][3]
	);
}


#endif