//==================================================================
/// DMT_VecN.h
///
/// Created by Davide Pasca - 2009/5/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DMT_VECN_H
#define DMT_VECN_H

#include "DMathBase.h"

//==================================================================
/// VecN
//==================================================================

//==================================================================
#define FOR_I_N	for (int i=0; i<_N; ++i)

template<class _S, size_t _N>
class VecN
{
public:
	_S	v[_N];

	//==================================================================
	VecN()						{}
	VecN( const VecN &v_ )		{ FOR_I_N v[i] = v_.v[i]; }
	VecN( const _S& a_ )		{ FOR_I_N v[i] = a_;		}
	VecN( const _S *p_ )		{ FOR_I_N v[i] = p_[i];	}

	void Set( const _S *p_ )	{ FOR_I_N v[i] = p_[i];	}
	void SetZero()				{ FOR_I_N v[i] = 0;		}

	VecN operator + (const _S& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] + rval; return tmp; }
	VecN operator - (const _S& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] - rval; return tmp; }
	VecN operator * (const _S& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] * rval; return tmp; }
	VecN operator / (const _S& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] / rval; return tmp; }
	VecN operator + (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] + rval.v[i]; return tmp; }
	VecN operator - (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] - rval.v[i]; return tmp; }
	VecN operator * (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] * rval.v[i]; return tmp; }
	VecN operator / (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] / rval.v[i]; return tmp; }

	VecN operator -() const	{ VecN tmp; FOR_I_N tmp.v[i] = -v[i]; return tmp; }

	VecN operator +=(const VecN &rval)	{ *this = *this + rval; return *this; }

	const _S &operator [] (size_t i) const	{ return v[i]; }
		  _S &operator [] (size_t i)		{ return v[i]; }

	friend VecN	DSqrt( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DSqrt( a[i] ); return tmp; }
	friend VecN	DRSqrt( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DRSqrt( a[i] ); return tmp; }
	friend VecN	DSign( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DSign( a[i] ); return tmp; }
	friend VecN	DAbs( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DAbs( a[i] ); return tmp; }
	friend VecN	DSin( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a )	{ VecN tmp; FOR_I_N tmp[i] = DCos( a[i] ); return tmp; }
};

template <class _S, size_t _N>
inline VecN<_S,_N> operator * (const _S &lval, const VecN<_S,_N> &rval) { return rval * lval; }

#undef FOR_I_N

//==================================================================
/// 128 bit 4-way float SIMD
//==================================================================
#if defined(DMATH_USE_M128)

//==================================================================
/// VecN <float,4>
//==================================================================
template <>
class VecN <float,4>
{
public:
	__m128	v;

	//==================================================================
	VecN()						{}
	VecN( const __m128 &v_ )	{ v = v_; }
	VecN( const VecN &v_ )		{ v = v_.v; }
	VecN( const float& a_ )		{ v = _mm_set_ps1( a_ );	}
	VecN( const float *p_ )		{ v = _mm_loadu_ps( p_ );	}	// unaligned

	void Set( const float *p_ )	{ v = _mm_loadu_ps( p_ );	}	// unaligned
	void SetZero()				{ v = _mm_setzero_ps();		}

	VecN operator + (const float& rval) const	{ return _mm_add_ps( v, _mm_set_ps1( rval )	); }
	VecN operator - (const float& rval) const	{ return _mm_sub_ps( v, _mm_set_ps1( rval )	); }
	VecN operator * (const float& rval) const	{ return _mm_mul_ps( v, _mm_set_ps1( rval )	); }
	VecN operator / (const float& rval) const	{ return _mm_div_ps( v, _mm_set_ps1( rval )	); }
	VecN operator + (const VecN &rval) const	{ return _mm_add_ps( v, rval.v	); }
	VecN operator - (const VecN &rval) const	{ return _mm_sub_ps( v, rval.v	); }
	VecN operator * (const VecN &rval) const	{ return _mm_mul_ps( v, rval.v	); }
	VecN operator / (const VecN &rval) const	{ return _mm_div_ps( v, rval.v	); }

	VecN operator -() const						{ return _mm_sub_ps( _mm_setzero_ps(), v ); }

	VecN operator +=(const VecN &rval)			{ *this = *this + rval; return *this; }

	const float &operator [] (size_t i) const	{ return v.m128_f32[i]; }
		  float &operator [] (size_t i)			{ return v.m128_f32[i]; }


	friend VecN	DSqrt( const VecN &a )	{ return _mm_sqrt_ps( a.v );	}
	friend VecN	DRSqrt( const VecN &a )	{ return _mm_rsqrt_ps( a.v );	}

	//==================================================================
	friend VecN	DSign( const VecN &a )
	{
		const __m128	zero		= _mm_setzero_ps();
		const __m128	selectPos	= _mm_cmpgt_ps( a.v, zero );	// > 0
		const __m128	selectNeg	= _mm_cmplt_ps( a.v, zero );	// < 0

		__m128	res =		  _mm_and_ps( selectPos, _mm_set_ps1(  1.0f ) );
		res = _mm_or_ps( res, _mm_and_ps( selectNeg, _mm_set_ps1( -1.0f ) ) );

		return res;
	}

	friend VecN	DAbs( const VecN &a )
	{
		static const U32 notSignBitMask = ~0x80000000;
		return _mm_and_ps( a.v, _mm_set_ps1( *(float *)&notSignBitMask ) );
	}

	friend VecN	DSin( const VecN &a ) { VecN tmp; for (size_t i=0; i<4; ++i) tmp.v.m128_f32[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a ) { VecN tmp; for (size_t i=0; i<4; ++i) tmp.v.m128_f32[i] = DCos( a[i] ); return tmp; }
	//friend VecN operator * (const _S &lval, const VecN &rval) { return rval * lval; }
};

#endif	// DMATH_USE_M128

//==================================================================
/// 512 bit 16-way float SIMD
//==================================================================
#if defined(DMATH_USE_M512)

//==================================================================
/// VecN <float,16>
//==================================================================
template <>
class VecN <float,16>
{
public:
	__m512	v;

	//==================================================================
	VecN()						{}
	VecN( const __m512 &v_ )	{ v = v_; }
	VecN( const VecN &v_ )		{ v = v_.v; }
	VecN( const float& a_ )		{ v = _mm512_set_1to16_ps( a_ );	}
	VecN( const float *p_ )		{ v = _mm512_expandd( (void *)p_, _MM_FULLUPC_NONE, _MM_HINT_NONE ); }	// unaligned

	void Set( const float *p_ )	{ v = _mm512_expandd( (void *)p_, _MM_FULLUPC_NONE, _MM_HINT_NONE ); }	// unaligned
	void SetZero()				{ v = _mm512_setzero_ps();		}

	VecN operator + (const float& rval) const	{ return _mm512_add_ps( v, _mm512_set_1to16_ps( rval )	); }
	VecN operator - (const float& rval) const	{ return _mm512_sub_ps( v, _mm512_set_1to16_ps( rval )	); }
	VecN operator * (const float& rval) const	{ return _mm512_mul_ps( v, _mm512_set_1to16_ps( rval )	); }
	VecN operator / (const float& rval) const	{ return _mm512_div_ps( v, _mm512_set_1to16_ps( rval )	); }
	VecN operator + (const VecN &rval) const	{ return _mm512_add_ps( v, rval.v	); }
	VecN operator - (const VecN &rval) const	{ return _mm512_sub_ps( v, rval.v	); }
	VecN operator * (const VecN &rval) const	{ return _mm512_mul_ps( v, rval.v	); }
	VecN operator / (const VecN &rval) const	{ return _mm512_div_ps( v, rval.v	); }

	VecN operator -() const						{ return _mm512_sub_ps( _mm512_setzero_ps(), v ); }

	VecN operator +=(const VecN &rval)			{ *this = *this + rval; return *this; }

	const float &operator [] (size_t i) const	{ return v.v[i]; }
		  float &operator [] (size_t i)			{ return v.v[i]; }


	friend VecN	DSqrt( const VecN &a )	{ return _mm512_sqrt_ps( a.v );	}
	friend VecN	DRSqrt( const VecN &a )	{ return _mm512_rsqrt_ps( a.v );	}

	//==================================================================
	friend VecN	DSign( const VecN &a )
	{
		const __m512	zero		= _mm512_setzero_ps();
		const __mmask	selectPos	= _mm512_cmpnle_ps( a.v, zero );	// >
		const __mmask	selectNeg	= _mm512_cmplt_ps( a.v, zero );		// <

		__m512	res = _mm512_mask_movd( zero, selectPos, _mm512_set_1to16_ps(  1.0f ) );
				res = _mm512_mask_movd( res , selectNeg, _mm512_set_1to16_ps( -1.0f ) );

		return res;
	}

	friend VecN	DAbs( const VecN &a )
	{
		return _mm512_maxabs_ps( a.v, a.v );
	}

	friend VecN	DSin( const VecN &a ) { VecN tmp; for (size_t i=0; i<16; ++i) tmp.v.v[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a ) { VecN tmp; for (size_t i=0; i<16; ++i) tmp.v.v[i] = DCos( a[i] ); return tmp; }
	//friend VecN operator * (const _S &lval, const VecN &rval) { return rval * lval; }
};

#endif	// DMATH_USE_M128

#endif