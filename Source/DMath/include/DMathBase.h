//==================================================================
/// DMathBase.h
///
/// Created by Davide Pasca - 2009/5/6
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DMATHBASE_H
#define DMATHBASE_H

#if defined(__linux__) || (__APPLE__)
	#include <inttypes.h>
#endif

#define DMATH_USE_M128
//#define DMATH_USE_M512

#if defined(DMATH_USE_M128)
	#include <xmmintrin.h>

	#define DMT_SIMD_FLEN	4
	#define DMT_SIMD_ALIGN_SIZE	16	//	DMT_SIMD_FLEN * 4

	// better reciprocal square root found on the Internet 8)
	inline __m128 _mm_rsqrtnr_ps( __m128 x )
	{
	  __m128 t = _mm_rsqrt_ps(x);
	  return _mm_mul_ps(  _mm_set_ps1( 0.5f ),
						  _mm_mul_ps(_mm_sub_ps(  _mm_set_ps1( 3.0f ),
												  _mm_mul_ps(_mm_mul_ps(x,t),t) ),t)  );
	}

	//==================================================================
	struct VecNMask
	{
		union
		{
			unsigned int	m[4];
			__m128			v;
		} u;

		VecNMask()						{}

		VecNMask( const __m128 &from )	{	u.v = from; }

		VecNMask( unsigned int a, unsigned int b, unsigned int c, unsigned int d )
		{
			u.m[0] = a;
			u.m[1] = b;
			u.m[2] = c;
			u.m[3] = d;
		}

		friend VecNMask CmpMaskEQ( const VecNMask &lval, const VecNMask &rval ) { return _mm_cmpeq_ps( lval.u.v, rval.u.v );  }
		friend VecNMask CmpMaskNE( const VecNMask &lval, const VecNMask &rval ) { return _mm_cmpneq_ps( lval.u.v, rval.u.v ); }

		friend bool operator ==( const VecNMask &lval, const VecNMask &rval )
		{
			__m128	tmp = _mm_xor_ps( lval.u.v, rval.u.v );

			unsigned int	folded =  ((const unsigned int *)&tmp)[0]
									| ((const unsigned int *)&tmp)[1]
									| ((const unsigned int *)&tmp)[2]
									| ((const unsigned int *)&tmp)[3];
			return !folded;
		}

		friend bool operator !=( const VecNMask &lval, const VecNMask &rval )
		{
			return !(lval == rval);
		}

		VecNMask operator & ( const VecNMask &rval ) {	return _mm_and_ps( u.v, rval.u.v ); }
		VecNMask operator | ( const VecNMask &rval ) {	return _mm_or_ps( u.v, rval.u.v ); }
	};

	static const VecNMask DMT_SIMD_ALLONE( 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF );
	static const VecNMask DMT_SIMD_ALLZERO( 0, 0, 0, 0 );

	static const VecNMask VecNMaskFull = DMT_SIMD_ALLONE.u.v;
	static const VecNMask VecNMaskEmpty = DMT_SIMD_ALLZERO.u.v;

	inline void VecNMask_Broadcast0Lane( VecNMask &val )
	{
		val.u.v = _mm_shuffle_ps( val.u.v, val.u.v, _MM_SHUFFLE(0,0,0,0) );
	}

#else

	#if defined(DMATH_USE_M512)

			#define USE_C_PROTOTYPE_PRIMITIVES 0

			#include "external/lrb/lrb_prototype_primitives.inl"

			#define DMT_SIMD_FLEN	16
			#define DMT_SIMD_ALIGN_SIZE	64	//	DMT_SIMD_FLEN * 4

			typedef __mmask	VecNMask;	// only need 16 bits

			inline void VecNMask_Broadcast0Lane( VecNMask &val )
			{
				// expand the least significant bit
				val = (VecNMask)(((signed short)(val << (DMT_SIMD_FLEN-1))) >> (DMT_SIMD_FLEN-1));
			}

	#else

			#define DMT_SIMD_FLEN		1
			#define DMT_SIMD_ALIGN_SIZE	4	//	DMT_SIMD_FLEN * 4

			typedef unsigned short	VecNMask;	// only need 4 bits (round to 1 byte)

			inline void VecNMask_Broadcast0Lane( VecNMask &val )
			{
				// expand the least significant bit
				val = (VecNMask)(((signed char)(val << (DMT_SIMD_FLEN-1))) >> (DMT_SIMD_FLEN-1));
			}

	#endif

		static const VecNMask VecNMaskFull = (VecNMask)-1;
		static const VecNMask VecNMaskEmpty = (VecNMask)0;

		inline VecNMask CmpMaskEQ( const VecNMask &lval, const VecNMask &rval ) { return lval == rval; }
		inline VecNMask CmpMaskNE( const VecNMask &lval, const VecNMask &rval ) { return lval != rval; }

#endif

// gives the number of SIMD blocks necessary for _SIZE_ elements
#define	DMT_SIMD_BLOCKS(_SIZE_)			(((unsigned)(_SIZE_) + (DMT_SIMD_FLEN-1)) / DMT_SIMD_FLEN)

// gives the block index from the global index _GLOB_IDX_
#define	DMT_SIMD_BLK_IDX(_GLOB_IDX_)	(((unsigned)(_GLOB_IDX_)) / DMT_SIMD_FLEN)

// gives the index of a SIMD block element from the global index _GLOB_IDX_
#define	DMT_SIMD_SUB_IDX(_GLOB_IDX_)	(((unsigned)(_GLOB_IDX_)) & (DMT_SIMD_FLEN-1))

// gives the size padded to the SIMD length
#define	DMT_SIMD_PADSIZE(_SIZE_)		(((unsigned)(_SIZE_) + (DMT_SIMD_FLEN-1)) & ~(DMT_SIMD_FLEN-1))


#include <math.h>
#include <float.h>
#include <assert.h>

#define DEG2RAD(_X_)	((_X_) * (3.14159265f/180))

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
template <class T>	inline T		DPow( const T &a,  const T &b )					 { assert( 0 );	return T();	}
template <>			inline float	DPow<float>( const float &a,  const float &b )	 { return powf( a, b );	}
template <>			inline double	DPow<double>( const double &a,  const double &b ){ return pow( a, b );	}

//==================================================================
template <class T>	inline T		DFloor( const T &a )			 { assert( 0 );	return T();	}
template <class T>	inline T		DCeil( const T &a )				 { assert( 0 );	return T();	}

#if defined(__GNUC__)
template <>			inline float	DFloor<float>( const float &a )	 { return __builtin_floorf( a );}
template <>			inline double	DFloor<double>( const double &a ){ return __builtin_floor( a );	}
template <>			inline float	DCeil<float>( const float &a )	 { return __builtin_ceilf( a );	}
template <>			inline double	DCeil<double>( const double &a ) { return __builtin_ceil( a );	}
#else
template <>			inline float	DFloor<float>( const float &a )	 { return floorf( a );	}
template <>			inline double	DFloor<double>( const double &a ){ return floor( a );	}
template <>			inline float	DCeil<float>( const float &a )	 { return ceilf( a );	}
template <>			inline double	DCeil<double>( const double &a ) { return ceil( a );	}
#endif

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
template <class T>	inline T        DMin3( const T &a, const T &b, const T &c )
{
    if ( a < b ) return a < c ? a : c; else
                 return b < c ? b : c;
}
//==================================================================
template <class T>	inline T		DMax( const T &a, const T &b )
{
	if ( a > b )	return a;	else
					return b;
}
//==================================================================
template <class T>	inline T        DMax3( const T &a, const T &b, const T &c )
{
    if ( a > b ) return a > c ? a : c; else
                 return b > c ? b : c;
}
//==================================================================
template <class T>	inline T		DClamp( const T& a, const T& x0, const T& x1 )
{
	if ( a < x0 )	return x0;	else
	if ( a > x1 )	return x1;	else
					return a;
}
//==================================================================
template < typename _TA , typename _TB >
inline _TA DMix( const _TA &a, const _TA &b,  const _TB &t )
{
    return (_TA)(a + (b - a) * t);
}
template < typename _TA , typename _TB >
inline _TA DLerp( const _TA &a, const _TA &b,  const _TB &t )
{
    return (_TA)(a + (b - a) * t);
}
template <typename _TA, typename _TB>
inline _TA DInvLerp( const _TA &left, const _TA &right, const _TB &x )
{
    if ( x < left  ) return _TA(0); else
    if ( x > right ) return _TA(1);

	return DClamp( (x - left) / (right - left), _TA(0), _TA(1) );
}
template <typename _TA , typename _TB>
inline _TA DSmoothStep( const _TA &a, const _TA &b,  const _TB &t )
{
    auto x = DInvLerp( a, b, t );
    return x*x*(_TA(3) - _TA(2)*x);
}

#define FM_E        ((float)M_E       )
#define FM_LOG2E    ((float)M_LOG2E   )
#define FM_LOG10E   ((float)M_LOG10E  )
#define FM_LN2      ((float)M_LN2     )
#define FM_LN10     ((float)M_LN10    )
#define FM_PI       ((float)M_PI      )
#define FM_2PI      ((float)(M_PI * 2))
#define FM_PI_2     ((float)M_PI_2    )
#define FM_PI_4     ((float)M_PI_4    )
#define FM_1_PI     ((float)M_1_PI    )
#define FM_2_PI     ((float)M_2_PI    )
#define FM_2_SQRTPI ((float)M_2_SQRTPI)
#define FM_SQRT2    ((float)M_SQRT2   )
#define FM_SQRT1_2  ((float)M_SQRT1_2 )

typedef unsigned char		DU8;
typedef unsigned int		DU32;

#if defined(__linux__) || (__APPLE__) || defined(ANDROID) || defined(NACL)
typedef uint64_t			DU64;
#else
typedef unsigned __int64	DU64;
#endif

#endif

