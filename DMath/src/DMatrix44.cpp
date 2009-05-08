//==================================================================
/// DMatrix44.cpp
///
/// Created by Davide Pasca - 2008/12/22
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DUtils.h"
#include "DMatrix44.h"

//==================================================================
void Matrix44::PrintOut() const
{
	printf( "[" );
	for (size_t r=0; r < 4; ++r)
	{
		if ( r != 0 )
			printf( " |" );

		for (size_t c=0; c < 4; ++c)
		{
			printf( " %f", u.m44[r][c] );
		}
	}
	printf( "]\n" );
}

//==================================================================
inline float det2x2(float a, float b,
					float c, float d)
{
	return a * d - b * c;
}

//==================================================================
inline float det3x3(float a1,float a2,float a3,
					float b1,float b2,float b3,
					float c1,float c2,float c3)
{
	return
		a1 * det2x2( b2, b3, c2, c3 )
		- b1 * det2x2( a2, a3, c2, c3 )
		+ c1 * det2x2( a2, a3, b2, b3 );
}

//==================================================================
inline float det4x4(float a1, float a2, float a3, float a4, 
					float b1, float b2, float b3, float b4, 
					float c1, float c2, float c3, float c4, 
					float d1, float d2, float d3, float d4)
{
	return 
		a1 * det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4)
		- b1 * det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4)
		+ c1 * det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4)
		- d1 * det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);
}

//==================================================================
Matrix44 Matrix44::GetInverse( bool *out_pSuccess ) const
{
	Matrix44 out( *this );

	if ( out_pSuccess )
		*out_pSuccess = false;

	float a1, a2, a3, a4, b1, b2, b3, b4, c1, c2, c3, c4, d1, d2, d3, d4;
	a1 = out.u.m44[0][0]; b1 = out.u.m44[0][1]; c1 = out.u.m44[0][2]; d1 = out.u.m44[0][3];
	a2 = out.u.m44[1][0]; b2 = out.u.m44[1][1]; c2 = out.u.m44[1][2]; d2 = out.u.m44[1][3];
	a3 = out.u.m44[2][0]; b3 = out.u.m44[2][1]; c3 = out.u.m44[2][2]; d3 = out.u.m44[2][3];
	a4 = out.u.m44[3][0]; b4 = out.u.m44[3][1]; c4 = out.u.m44[3][2]; d4 = out.u.m44[3][3];

	float det = det4x4(a1,a2,a3,a4,b1,b2,b3,b4,c1,c2,c3,c4,d1,d2,d3,d4);

	if ( fabs(det) < 0.000001f )
	{
		DASSERT( 0 );
		return out;
	}

	out.u.m44[0][0] =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
	out.u.m44[1][0] = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
	out.u.m44[2][0] =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
	out.u.m44[3][0] = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

	out.u.m44[0][1] = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
	out.u.m44[1][1] =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
	out.u.m44[2][1] = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
	out.u.m44[3][1] =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

	out.u.m44[0][2] =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
	out.u.m44[1][2] = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
	out.u.m44[2][2] =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
	out.u.m44[3][2] = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

	out.u.m44[0][3] = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
	out.u.m44[1][3] =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
	out.u.m44[2][3] = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
	out.u.m44[3][3] =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);

	out *= 1/det;

	if ( out_pSuccess )
		*out_pSuccess = true;

	return out;
}
