//==================================================================
/// DMathBase.h
///
/// Created by Davide Pasca - 2009/5/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMATHBASE_H
#define DMATHBASE_H

#define DMATH_USE_M128
//#define DMATH_USE_M512

#if defined(DMATH_USE_M128)
	#include <xmmintrin.h>

	#define DMT_SIMD_FLEN	4

#elif defined(DMATH_USE_M512)

	#define USE_C_PROTOTYPE_PRIMITIVES 0

	#include "external/lrb/lrb_prototype_primitives.inl"

	#define DMT_SIMD_FLEN	16

#else

	#define DMT_SIMD_FLEN	4

#endif

#include <math.h>
#include <float.h>
#include <assert.h>

#define DEG2RAD	(3.14159265f/180)

//==================================================================
template <class _S,class _T> inline _S DMix( const _S &a, const _S &b, const _T &t ){ return a + (b - a) * t; }

//==================================================================
template <class T>	inline T		DCos( const T &a )				{ assert( 0 );	return T();	}
template <>			inline float	DCos<float>( const float &a )	{ return cosf( a );	}
template <>			inline double	DCos<double>( const double &a )	{ return cos( a );	}

//==================================================================
template <class T>	inline T		DSin( const T &a )				{ assert( 0 );	return T();	}
template <>			inline float	DSin<float>( const float &a )	{ return sinf( a );	}
template <>			inline double	DSin<double>( const double &a )	{ return sin( a );	}

//==================================================================
template <class T>	inline T		DASin( const T &a )				{ assert( 0 );	return T();	}
template <>			inline float	DASin<float>( const float &a )	{ return asinf( a );	}
template <>			inline double	DASin<double>( const double &a ){ return asin( a );	}

//==================================================================
template <class T>	inline T		DSqrt( const T &a )				{ assert( 0 );	return T();	}
template <>			inline float	DSqrt<float>( const float &a )	{ return sqrtf( a );	}
template <>			inline double	DSqrt<double>( const double &a ){ return sqrt( a );	}

//==================================================================
template <class T>	inline T		DRSqrt( const T &a )			 { assert( 0 );	return T();	}
template <>			inline float	DRSqrt<float>( const float &a )	 { return 1.0f / sqrtf( a );	}
template <>			inline double	DRSqrt<double>( const double &a ){ return 1.0 / sqrt( a );	}

//==================================================================
template <class T>	inline T		DSign( const T &a )
{
	if ( a < 0 )	return T( -1 ); else
	if ( a > 0 )	return T(  1 ); else
					return T(  0 );
}

//==================================================================
template <class T>	inline T		DAbs( const T &a )
{
	if ( a < T(0) )	return -a;	else
					return  a;
}

//==================================================================
template <class T>	inline T		DMin( const T &a, const T &b )
{
	if ( a < b )	return a;	else
					return b;
}

//==================================================================
template <class T>	inline T		DMax( const T &a, const T &b )
{
	if ( a > b )	return a;	else
					return b;
}

#define FM_E        ((float)M_E       )
#define FM_LOG2E    ((float)M_LOG2E   )
#define FM_LOG10E   ((float)M_LOG10E  )
#define FM_LN2      ((float)M_LN2     )
#define FM_LN10     ((float)M_LN10    )
#define FM_PI       ((float)M_PI      )
#define FM_PI_2     ((float)M_PI_2    )
#define FM_PI_4     ((float)M_PI_4    )
#define FM_1_PI     ((float)M_1_PI    )
#define FM_2_PI     ((float)M_2_PI    )
#define FM_2_SQRTPI ((float)M_2_SQRTPI)
#define FM_SQRT2    ((float)M_SQRT2   )
#define FM_SQRT1_2  ((float)M_SQRT1_2 )

#endif