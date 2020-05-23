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
#include "DVector.h"
#include "DTypes.h"
#include "DUtils.h"

//==================================================================
//#define DMATRIX44_ROWMTX_MODE

#define DMT_MTX44_USE_VEC_TEMPLATE

// disable for clang for now
#if defined(_ARM_ARCH_7) && !defined(__clang__)
# define DMT_USE_NEON_ASM
#endif

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

    DFORCEINLINE Matrix44( bool setToIdentity )
    {
        DASSERT( setToIdentity == true );
            Identity();
    }

    DFORCEINLINE Matrix44( const float *pSrcMtx )
    {
        CopyRowMajor( pSrcMtx );
    }

    DFORCEINLINE Matrix44(
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

    DFORCEINLINE Float3 GetV3( size_t idx ) const
    {
        return Float3( mij(idx,0), mij(idx,1), mij(idx,2) );
    }

    DFORCEINLINE void SetV3( size_t idx, const Float3 &v )
    {
        mij(idx,0) = v.v3[0];
        mij(idx,1) = v.v3[1];
        mij(idx,2) = v.v3[2];
    }

    DFORCEINLINE void Identity()
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
    inline static Matrix44 Translate( const Float3 &tra );
    inline static Matrix44 Translate( float tx, float ty, float tz );
    inline static Matrix44 Rot( float ang, float ax, float ay, float az );
    inline static Matrix44 OrthoRH(
                        float left,
                        float right,
                        float bottom,
                        float top,
                        float nearr,
                        float farr );
    inline static Matrix44 Perspective( float fov, float aspect, float n, float f );
    inline static Matrix44 PerspectiveRH0( float fov, float aspect, float n, float f );

    static Matrix44 LookAt( const Float3 &at, const Float3 &eye, const Float3 &up );

    void CopyRowMajor( const float *pSrcMtx )
    {
    #ifdef DMT_MTX44_USE_VEC_TEMPLATE
        v16 = VecN<float,16>( pSrcMtx );
    #else
        memcpy( v16, pSrcMtx, sizeof(v16) );
    #endif
    }

    DFORCEINLINE Float3 GetTranslation() const
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
DFORCEINLINE Matrix44 Matrix44::GetTranspose() const
{
    Matrix44	out;

    for (int i=0; i < 4; ++i)
        for (int j=0; j < 4; ++j)
            out.mij(i,j) = mij(j,i);

    return out;
}

//==================================================================
DFORCEINLINE Matrix44 Matrix44::GetAs33() const
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
DFORCEINLINE Matrix44 Matrix44::Scale( float sx, float sy, float sz )
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
inline Matrix44 Matrix44::Translate( const Float3 &tra )
{
    return Translate( tra.x(), tra.y(), tra.z() );
}
//==================================================================
DFORCEINLINE Matrix44 Matrix44::Translate( float tx, float ty, float tz )
{
    Matrix44	m( true );
    m.mij(3,0) = tx;
    m.mij(3,1) = ty;
    m.mij(3,2) = tz;
    m.mij(3,3) = 1.0f;

    return m;
}
//==================================================================
DFORCEINLINE Matrix44 Matrix44::Rot( float ang, float ax, float ay, float az )
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
inline Matrix44 Matrix44::OrthoRH(
                        float left,
                        float right,
                        float bottom,
                        float top,
                        float nearr,
                        float farr )
{
    DASSERT( right != left && top != bottom && farr != nearr );

    float	rl = right - left;
    float	tb = top - bottom;
    float	fn = farr - nearr;

    float tx = -(right + left) / rl;
    float ty = -(top + bottom) / tb;
    float tz = -(farr + nearr) / fn;

    return Matrix44(
            2 / rl,	0,		0,			0,
            0,		2 / tb,	0,			0,
            0,		0,		-2 / fn,	0,
            tx,		ty,		tz,			1 );
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
#if defined(DMT_USE_NEON_ASM)

//==================================================================
// Note: from oolong engine
DFORCEINLINE void NEON_Matrix4Mul( const float* a, const float* b, float* output )
{
    asm volatile
    (
    // Store A & B leaving room at top of registers for result (q0-q3)
    "vldmia %1, { q4-q7 }  \n\t"
    "vldmia %2, { q8-q11 } \n\t"

    // result = first column of B x first row of A
    "vmul.f32 q0, q8, d8[0]\n\t"
    "vmul.f32 q1, q8, d10[0]\n\t"
    "vmul.f32 q2, q8, d12[0]\n\t"
    "vmul.f32 q3, q8, d14[0]\n\t"

    // result += second column of B x second row of A
    "vmla.f32 q0, q9, d8[1]\n\t"
    "vmla.f32 q1, q9, d10[1]\n\t"
    "vmla.f32 q2, q9, d12[1]\n\t"
    "vmla.f32 q3, q9, d14[1]\n\t"

    // result += third column of B x third row of A
    "vmla.f32 q0, q10, d9[0]\n\t"
    "vmla.f32 q1, q10, d11[0]\n\t"
    "vmla.f32 q2, q10, d13[0]\n\t"
    "vmla.f32 q3, q10, d15[0]\n\t"

    // result += last column of B x last row of A
    "vmla.f32 q0, q11, d9[1]\n\t"
    "vmla.f32 q1, q11, d11[1]\n\t"
    "vmla.f32 q2, q11, d13[1]\n\t"
    "vmla.f32 q3, q11, d15[1]\n\t"

    // output = result registers
    "vstmia %0, { q0-q3 }"
    : // no output
    : "r" (output), "r" (a), "r" (b)       // input - note *value* of pointer doesn't change
    : "memory", "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7", "q8", "q9", "q11" //clobber
    );
}

//==================================================================
DFORCEINLINE void NEON_Matrix4Vector3Mul(const float* m, const float* v, float* output)
{
    asm volatile
    (
     // Store m & v leaving room at top of registers for result (q0)
     "vldmia %1, {q1-q4 }   \n\t"   // q2-q5 = m
     "vldmia %2, {q5}               \n\t"   // q1    = v
     
     // result = first column of A x V.x
     "vmul.f32 q0, q1, d10[0]\n\t"
     
     // result += second column of A x V.y
     "vmla.f32 q0, q2, d10[1]\n\t"
     
     // result += third column of A x V.z
     "vmla.f32 q0, q3, d11[0]\n\t"
     
     //// result += last column of A x V.w
     //"vmla.f32 q0, q4, d11[1]\n\t"

     // result += last column of A
     "vadd.f32 q0, q0, q4\n\t"
     
     // output = result registers
     "vstmia %0, {q0}"
     
     : // no output
     : "r" (output), "r" (m), "r" (v)       // input - note *value* of pointer doesn't change
     : "memory", "q0", "q1", "q2", "q3", "q4", "q5" //clobber
     );
}

#endif

//==================================================================
DFORCEINLINE Matrix44 operator * (const Matrix44 &m1, const Matrix44 &m2)
{
    Matrix44        tmp;
#if !defined(DMT_USE_NEON_ASM)
    for (size_t r=0; r < 4; ++r)
    {
        for (size_t c=0; c < 4; ++c)
        {
            float   sum = 0;
            for (size_t i=0; i < 4; ++i)
#ifdef DMATRIX44_ROWMTX_MODE
                sum += m1.mij(i,c) * m2.mij(r,i);
#else
            sum += m1.mij(r,i) * m2.mij(i,c);
#endif
            
            tmp.mij(r,c) = sum;
        }
        
    }
#else
    
    NEON_Matrix4Mul( (const float *)&m1, (const float *)&m2, (float *)&tmp );
    
#endif
    return tmp;
}

//==================================================================
DFORCEINLINE Float4 V4__M44_Mul_V3W1( const Matrix44 &a, const Float3 &v )
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
DFORCEINLINE Float3 V3__M44_Mul_V3W0( const Matrix44 &a, const Float3 &v )
{
    float	x = v.v3[0], y = v.v3[1], z = v.v3[2];

    return Float3(
        a.mij(0,0) * x + a.mij(0,1) * y + a.mij(0,2) * z,
        a.mij(1,0) * x + a.mij(1,1) * y + a.mij(1,2) * z,
        a.mij(2,0) * x + a.mij(2,1) * y + a.mij(2,2) * z
    );
}
//==================================================================
DFORCEINLINE Float3 V3__M44_Mul_V3W1( const Matrix44 &a, const Float3 &v )
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
DFORCEINLINE Vec4<_T> V4__V3W1_Mul_M44( const Vec3<_T> &v, const Matrix44 &a )
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
DFORCEINLINE Vec4<_T> V4__V4_Mul_M44( const Vec4<_T> &v, const Matrix44 &a )
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
DFORCEINLINE _S4 V4__V3W0_Mul_M44( const _S3 &v, const Matrix44 &a )
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
template <class _TS>
DFORCEINLINE Vec3<_TS> V3__V3W1_Mul_M44( const Vec3<_TS> &v, const Matrix44 &a )
{
    const _TS	&x = v.v3[0];
    const _TS	&y = v.v3[1];
    const _TS	&z = v.v3[2];

    return Vec3<_TS>(
        x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0) + a.mij(3,0),
        x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1) + a.mij(3,1),
        x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2) + a.mij(3,2)
    );
}

//==================================================================
/*
#if defined(DMT_USE_NEON_ASM)
template <>
DFORCEINLINE Vec3<float> V3__V3W1_Mul_M44( const Vec3<float> &v, const Matrix44 &a )
{
    Vec3<float>	outVec;
    
    NEON_Matrix4Vector3Mul( (const float *)&a, &v[0], &outVec[0] );
    
    return outVec;
}
#endif
*/

//==================================================================
template <class _TS>
DFORCEINLINE Vec3<_TS> V3__V3W0_Mul_M44( const Vec3<_TS> &v, const Matrix44 &a )
{
    const _TS	&x = v.v3[0];
    const _TS	&y = v.v3[1];
    const _TS	&z = v.v3[2];

    return Vec3<_TS>(
        x * a.mij(0,0) + y * a.mij(1,0) + z * a.mij(2,0),
        x * a.mij(0,1) + y * a.mij(1,1) + z * a.mij(2,1),
        x * a.mij(0,2) + y * a.mij(1,2) + z * a.mij(2,2)
    );
}

//==================================================================
template <class _TS>
DFORCEINLINE Vec3<_TS> V3__V2_Mul_M44( const Vec2<_TS> &v, const Matrix44 &a )
{
    const _TS	&x = v.v2[0];
    const _TS	&y = v.v2[1];

    return Vec3<_TS>(
        x * a.mij(0,0) + y * a.mij(1,0),
        x * a.mij(0,1) + y * a.mij(1,1),
        x * a.mij(0,2) + y * a.mij(1,2)
    );
}

//==================================================================
template <class _TS>
DFORCEINLINE Vec2<_TS> V2__V2_Mul_M44( const Vec2<_TS> &v, const Matrix44 &a )
{
    const _TS	&x = v.v2[0];
    const _TS	&y = v.v2[1];

    return Vec2<_TS>(
        x * a.mij(0,0) + y * a.mij(1,0),
        x * a.mij(0,1) + y * a.mij(1,1)
    );
}

//==================================================================
template <class _TS>
DFORCEINLINE Vec2<_TS> V2__V2W1_Mul_M44( const Vec2<_TS> &v, const Matrix44 &a )
{
    const _TS	&x = v.v2[0];
    const _TS	&y = v.v2[1];

    return Vec2<_TS>(
        x * a.mij(0,0) + y * a.mij(1,0) + a.mij(3,0),
        x * a.mij(0,1) + y * a.mij(1,1) + a.mij(3,1)
    );
}

#endif
