//==================================================================
/// RI_Noise.cpmP
///
/// Created by Davide Pasca - 2009/5/17
/// based on http://mrl.nyu.edu/~perlin/doc/oscar.html
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include <stdlib.h>
#include "RI_Noise.h"

//==================================================================
namespace RI
{

//==================================================================
int		Noise::mP[B + B + 2];
float	Noise::mG3[B + B + 2][3];
float	Noise::mG2[B + B + 2][2];
float	Noise::mG1[B + B + 2];
bool	Noise::mStart = true;

//==================================================================
#define s_curve(t) ( t * t * (3.f - 2.f * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
#define setup(i,b0,b1,r0,r1)\
    t = vec[i] + N;\
    b0 = ((int)t) & BM;\
    b1 = (b0+1) & BM;\
    r0 = t - (int)t;\
    r1 = r0 - 1.f;

//==================================================================
inline void normalize2( float v[2] )
{
    float s = DSqrt(v[0] * v[0] + v[1] * v[1]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
}

//==================================================================
inline void normalize3( float v[3] )
{
    float s = DSqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] = v[0] / s;
    v[1] = v[1] / s;
    v[2] = v[2] / s;
}

//==================================================================
float Noise::noise1( const float arg )
{
    int bx0, bx1;
    float rx0, rx1, sx, t, u, v, vec[1];

    vec[0] = arg;
    if (mStart) {
        mStart = 0;
        init();
    }

    setup(0, bx0,bx1, rx0,rx1);

    sx = s_curve(rx0);

    u = rx0 * mG1[ mP[ bx0 ] ];
    v = rx1 * mG1[ mP[ bx1 ] ];

    return lerp(sx, u, v);
}

//==================================================================
float Noise::noise2( const float vec[2] )
{
    int bx0, bx1, by0, by1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;

    if (mStart) {
        mStart = 0;
        init();
    }

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);

    int i = mP[ bx0 ];
    int j = mP[ bx1 ];

    b00 = mP[ i + by0 ];
    b10 = mP[ j + by0 ];
    b01 = mP[ i + by1 ];
    b11 = mP[ j + by1 ];

    sx = s_curve(rx0);
    sy = s_curve(ry0);

#define at2(rx,ry) ( rx * q[0] + ry * q[1] )

    q = mG2[ b00 ] ; u = at2(rx0,ry0);
    q = mG2[ b10 ] ; v = at2(rx1,ry0);
    a = lerp(sx, u, v);

    q = mG2[ b01 ] ; u = at2(rx0,ry1);
    q = mG2[ b11 ] ; v = at2(rx1,ry1);
    b = lerp(sx, u, v);

    return lerp(sy, a, b);
}

//==================================================================
float Noise::noise3( const float vec[3] )
{
    int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
    float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;

    if (mStart) {
        mStart = 0;
        init();
    }

    setup(0, bx0,bx1, rx0,rx1);
    setup(1, by0,by1, ry0,ry1);
    setup(2, bz0,bz1, rz0,rz1);

    int i = mP[ bx0 ];
    int j = mP[ bx1 ];

    b00 = mP[ i + by0 ];
    b10 = mP[ j + by0 ];
    b01 = mP[ i + by1 ];
    b11 = mP[ j + by1 ];

    t  = s_curve(rx0);
    sy = s_curve(ry0);
    sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

    q = mG3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
    q = mG3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
    a = lerp(t, u, v);

    q = mG3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
    q = mG3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
    b = lerp(t, u, v);

    c = lerp(sy, a, b);

    q = mG3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
    q = mG3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
    a = lerp(t, u, v);

    q = mG3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
    q = mG3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
    b = lerp(t, u, v);

    d = lerp(sy, a, b);

    return lerp(sz, c, d);
}

//==================================================================
void Noise::init()
{
    int i, j, k;

    srand( 0x56781804 );

    for (i = 0 ; i < B ; i++) {
        mP[i] = i;

        mG1[i] = (float)((rand() % (B + B)) - B) / B;

        for (j = 0 ; j < 2 ; j++)
            mG2[i][j] = (float)((rand() % (B + B)) - B) / B;
        normalize2(mG2[i]);

        for (j = 0 ; j < 3 ; j++)
            mG3[i][j] = (float)((rand() % (B + B)) - B) / B;
        normalize3(mG3[i]);
    }

    while (--i) {
        k = mP[i];
        mP[i] = mP[j = rand() % B];
        mP[j] = k;
    }

    for (i = 0 ; i < B + 2 ; i++) {
        mP[B + i] = mP[i];
        mG1[B + i] = mG1[i];
        for (j = 0 ; j < 2 ; j++)
            mG2[B + i][j] = mG2[i][j];
        for (j = 0 ; j < 3 ; j++)
            mG3[B + i][j] = mG3[i][j];
    }
}

//==================================================================
}
