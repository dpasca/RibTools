//==================================================================
/// DMatrix44.cpp
///
/// Created by Davide Pasca - 2008/12/22
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DUtils.h"
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
			printf( " %f", mij(r,c) );
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
	a1 = out.mij(0,0); b1 = out.mij(0,1); c1 = out.mij(0,2); d1 = out.mij(0,3);
	a2 = out.mij(1,0); b2 = out.mij(1,1); c2 = out.mij(1,2); d2 = out.mij(1,3);
	a3 = out.mij(2,0); b3 = out.mij(2,1); c3 = out.mij(2,2); d3 = out.mij(2,3);
	a4 = out.mij(3,0); b4 = out.mij(3,1); c4 = out.mij(3,2); d4 = out.mij(3,3);

	float det = det4x4(a1,a2,a3,a4,b1,b2,b3,b4,c1,c2,c3,c4,d1,d2,d3,d4);

	if ( fabs(det) < 1e-11f )
	{
		DASSERT( 0 );
		return out;
	}

	out.mij(0,0) =   det3x3( b2, b3, b4, c2, c3, c4, d2, d3, d4);
	out.mij(1,0) = - det3x3( a2, a3, a4, c2, c3, c4, d2, d3, d4);
	out.mij(2,0) =   det3x3( a2, a3, a4, b2, b3, b4, d2, d3, d4);
	out.mij(3,0) = - det3x3( a2, a3, a4, b2, b3, b4, c2, c3, c4);

	out.mij(0,1) = - det3x3( b1, b3, b4, c1, c3, c4, d1, d3, d4);
	out.mij(1,1) =   det3x3( a1, a3, a4, c1, c3, c4, d1, d3, d4);
	out.mij(2,1) = - det3x3( a1, a3, a4, b1, b3, b4, d1, d3, d4);
	out.mij(3,1) =   det3x3( a1, a3, a4, b1, b3, b4, c1, c3, c4);

	out.mij(0,2) =   det3x3( b1, b2, b4, c1, c2, c4, d1, d2, d4);
	out.mij(1,2) = - det3x3( a1, a2, a4, c1, c2, c4, d1, d2, d4);
	out.mij(2,2) =   det3x3( a1, a2, a4, b1, b2, b4, d1, d2, d4);
	out.mij(3,2) = - det3x3( a1, a2, a4, b1, b2, b4, c1, c2, c4);

	out.mij(0,3) = - det3x3( b1, b2, b3, c1, c2, c3, d1, d2, d3);
	out.mij(1,3) =   det3x3( a1, a2, a3, c1, c2, c3, d1, d2, d3);
	out.mij(2,3) = - det3x3( a1, a2, a3, b1, b2, b3, d1, d2, d3);
	out.mij(3,3) =   det3x3( a1, a2, a3, b1, b2, b3, c1, c2, c3);

	out = out * (1/det);

	if ( out_pSuccess )
		*out_pSuccess = true;

	return out;
}

//==================================================================
Matrix44 Matrix44::LookAt( const Float3 &at, const Float3 &eye, const Float3 &up )
{
	//return Matrix44::Translate( eye );

	if ( DLengthSqr( at - eye ) < 0.0001f )
	{
		DASSERT(0);
		return Matrix44(true);
	}

    DASSERT( DLengthSqr( up ) >= 0.0001f );

	Float3	za = DNormalize( eye - at );
	Float3	xa = DNormalize( DCross( up, za ) );
	Float3	ya = DCross( za, xa );

	Matrix44 mtx(
		xa[0], ya[0], za[0], 0,
		xa[1], ya[1], za[1], 0,
		xa[2], ya[2], za[2], 0,
		0, 0, 0, 1 );

	Float3	v = V3__V3W0_Mul_M44( -eye, mtx );

	mtx.SetV3( 3, v );

	return mtx;
}
