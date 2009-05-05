//==================================================================
/// DMathBase.h
///
/// Created by Davide Pasca - 2009/5/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMATHBASE_H
#define DMATHBASE_H

#include <math.h>
#include <float.h>

#define DEG2RAD	(3.14159265f/180)

//==================================================================
template <class T>	inline T DMix( const T &a, const T &b, float t ){ return a + (b - a) * t; }

//==================================================================
template <class T>	inline T		DCos( const T &a )				{ DASSERT( 0 );	return 0;	}
template <>			inline float	DCos<float>( const float &a )	{ return cosf( a );	}
template <>			inline double	DCos<double>( const double &a )	{ return cos( a );	}

//==================================================================
template <class T>	inline T		DSin( const T &a )				{ DASSERT( 0 );	return 0;	}
template <>			inline float	DSin<float>( const float &a )	{ return sinf( a );	}
template <>			inline double	DSin<double>( const double &a )	{ return sin( a );	}

//==================================================================
template <class T>	inline T		DASin( const T &a )				{ DASSERT( 0 );	return 0;	}
template <>			inline float	DASin<float>( const float &a )	{ return asinf( a );	}
template <>			inline double	DASin<double>( const double &a ){ return asin( a );	}

//==================================================================
template <class T>	inline T		DSqrt( const T &a )				{ DASSERT( 0 );	return 0;	}
template <>			inline float	DSqrt<float>( const float &a )	{ return sqrtf( a );	}
template <>			inline double	DSqrt<double>( const double &a ){ return sqrt( a );	}

//==================================================================
template <class T>	inline T		DRSqrt( const T &a )			 { DASSERT( 0 );	return 0;	}
template <>			inline float	DRSqrt<float>( const float &a )	 { return 1.0f / sqrtf( a );	}
template <>			inline double	DRSqrt<double>( const double &a ){ return 1.0 / sqrt( a );	}

//==================================================================
template <class T>	inline T		DSign( const T &a )
{
	if ( a < 0 )	return T( -1 ); else
	if ( a > 0 )	return T(  1 ); else
					return T(  0 );
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