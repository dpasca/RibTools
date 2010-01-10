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

	_S	AddReduce() const		{ _S acc=_S(0); FOR_I_N acc += v[i]; return acc; }

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

	friend bool operator ==( const VecN &lval, const VecN &rval ) { FOR_I_N if NOT( lval[i] == rval[i] ) return false; return true; }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { FOR_I_N if NOT( lval[i] == rval[i] ) return true; return false; }

	const _S &operator [] (size_t i) const	{ return v[i]; }
		  _S &operator [] (size_t i)		{ return v[i]; }

	friend VecN	DSqrt( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSqrt( a[i] ); return tmp; }
	friend VecN	DRSqrt( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DRSqrt( a[i] ); return tmp; }
	friend VecN	DPow( const VecN &a,  const VecN &b ){ VecN tmp; FOR_I_N tmp[i] = DPow( a[i], b[i] ); return tmp; }
	friend VecN	DSign( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSign( a[i] ); return tmp; }
	friend VecN	DAbs( const VecN &a	)				 { VecN tmp; FOR_I_N tmp[i] = DAbs( a[i] ); return tmp; }
	friend VecN	DMin( const VecN &a, const VecN &b ) { VecN tmp; FOR_I_N tmp[i] = DMin( a[i], b[i] ); return tmp; }
	friend VecN	DMax( const VecN &a, const VecN &b ) { VecN tmp; FOR_I_N tmp[i] = DMax( a[i], b[i] ); return tmp; }
	friend VecN	DSin( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DCos( a[i] ); return tmp; }
};

template <class _S, size_t _N> VecN<_S,_N> operator + (const _S &lval, const VecN<_S,_N> &rval) { return rval + lval; }
template <class _S, size_t _N> VecN<_S,_N> operator - (const _S &lval, const VecN<_S,_N> &rval) { return rval - lval; }
template <class _S, size_t _N> VecN<_S,_N> operator * (const _S &lval, const VecN<_S,_N> &rval) { return rval * lval; }
template <class _S, size_t _N> VecN<_S,_N> operator / (const _S &lval, const VecN<_S,_N> &rval) { return rval / lval; }

/*
template <class _S, size_t _N> VecNMask CmpMaskLT( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _S, size_t _N> VecNMask CmpMaskGT( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _S, size_t _N> VecNMask CmpMaskEQ( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _S, size_t _N> VecNMask CmpMaskNE( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _S, size_t _N> VecNMask CmpMaskLE( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _S, size_t _N> VecNMask CmpMaskGE( const VecN<_S,_N> &lval, const VecN<_S,_N> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
*/

#undef FOR_I_N

//==================================================================
#define FOR_I_N	for (int i=0; i<DMT_SIMD_FLEN; ++i)

template<>
class VecN<float,DMT_SIMD_FLEN>
{
//==================================================================
/// 128 bit 4-way float SIMD
//==================================================================
#if defined(DMATH_USE_M128)
public:
	__m128	v;

	//==================================================================
	VecN()						{}
	VecN( const __m128 &v_ )	{ v = v_; }
	VecN( const VecN &v_ )		{ v = v_.v; }
	VecN( const float& a_ )		{ v = _mm_set_ps1( a_ );	}
	//VecN( const float *p_ )		{ v = _mm_loadu_ps( p_ );	}	// unaligned

	//VecN( const VecN &from )	{ v.v = from.v; }

	//void Set( const float *p_ )	{ v = _mm_loadu_ps( p_ );	}	// unaligned
	void SetZero()				{ v = _mm_setzero_ps();		}

	// TODO: add specialized AddReduce()
	float AddReduce() const		{ return (*this)[0] + (*this)[1] + (*this)[2] + (*this)[3]; }

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

	friend VecNMask CmpMaskLT( const VecN &lval, const VecN &rval ) { return _mm_cmplt_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskGT( const VecN &lval, const VecN &rval ) { return _mm_cmpgt_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskEQ( const VecN &lval, const VecN &rval ) { return _mm_cmpeq_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskNE( const VecN &lval, const VecN &rval ) { return _mm_cmpneq_ps( lval.v, rval.v ); }
	friend VecNMask CmpMaskLE( const VecN &lval, const VecN &rval ) { return _mm_cmple_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskGE( const VecN &lval, const VecN &rval ) { return _mm_cmpge_ps( lval.v, rval.v );  }

	friend bool operator ==( const VecN &lval, const VecN &rval ) { return VecNMaskEmpty == CmpMaskNE( lval, rval ); }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { return VecNMaskFull  != CmpMaskEQ( lval, rval ); }

//#if defined(_MSC_VER)
//	const float &operator [] (size_t i) const	{ return v.m128_f32[i]; }
//		  float &operator [] (size_t i)			{ return v.m128_f32[i]; }
//#else
	const float &operator [] (size_t i) const	{ return ((const float *)&v)[i]; }
		  float &operator [] (size_t i)			{ return ((float *)&v)[i]; }
//#endif

	friend VecN	DSqrt( const VecN &a )	{ return _mm_sqrt_ps( a.v );	}
	friend VecN	DRSqrt( const VecN &a )	{ return _mm_rsqrtnr_ps( a.v );	}
	//friend VecN	DRSqrt( const VecN &a )	{ return _mm_rsqrt_ps( a.v );	}

	// TODO: get a proper _mm_pow_ps !!!
	//friend VecN	DPow( const VecN &a, const VecN &b ){ return _mm_pow_ps( a.v, b.v );	}
	friend VecN	DPow( const VecN &a, const VecN &b )
	{
		VecN tmp;
		for (size_t i=0; i < DMT_SIMD_FLEN; ++i)
			tmp[i] = powf( a[i], b[i] );
		
		return tmp;
	}

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

	friend VecN	DMin( const VecN &a, const VecN &b )	{	return _mm_min_ps( a.v, b.v );	}
	friend VecN	DMax( const VecN &a, const VecN &b )	{	return _mm_max_ps( a.v, b.v );	}

#elif defined(DMATH_USE_M512)
//==================================================================
/// 512 bit 16-way float SIMD
//==================================================================
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

	float AddReduce() const		{ return _mm512_reduce_add_ps( v ); }

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

	// TODO: verify that it works !
	friend VecNMask CmpMaskLT( const VecN &lval, const VecN &rval ) { return _mm512_cmplt_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskGT( const VecN &lval, const VecN &rval ) { return _mm512_cmpgt_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskEQ( const VecN &lval, const VecN &rval ) { return _mm512_cmpeq_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskNE( const VecN &lval, const VecN &rval ) { return _mm512_cmpneq_ps( lval.v, rval.v ); }
	friend VecNMask CmpMaskLE( const VecN &lval, const VecN &rval ) { return _mm512_cmple_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskGE( const VecN &lval, const VecN &rval ) { return _mm512_cmpge_ps( lval.v, rval.v );  }

	friend bool operator ==( const VecN &lval, const VecN &rval ) { return VecNMaskEmpty == CmpMaskNE( lval, rval ) }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { return VecNMaskFull  != CmpMaskEQ( lval, rval ) }

	const float &operator [] (size_t i) const	{ return v.v[i]; }
		  float &operator [] (size_t i)			{ return v.v[i]; }


	friend VecN	DSqrt( const VecN &a )	{ return _mm512_sqrt_ps( a.v );	}
	friend VecN	DRSqrt( const VecN &a )	{ return _mm512_rsqrt_ps( a.v );	}

	friend VecN	DPow( const VecN &a, const VecN &b ){ return _mm512_pow_ps( a.v, b.v );	}

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

	friend VecN	DMin( const VecN &a, const VecN &b )	{	return _mm512_min_ps( a.v, b.v );	}
	friend VecN	DMax( const VecN &a, const VecN &b )	{	return _mm512_max_ps( a.v, b.v );	}

#else
//==================================================================
/// No hardware SIMD
//==================================================================
public:
	_S	v[_N];

	//==================================================================
	VecN()						{}
	VecN( const VecN &v_ )		{ FOR_I_N v[i] = v_.v[i]; }
	VecN( const _S& a_ )		{ FOR_I_N v[i] = a_;		}
	VecN( const _S *p_ )		{ FOR_I_N v[i] = p_[i];	}

	void Set( const _S *p_ )	{ FOR_I_N v[i] = p_[i];	}
	void SetZero()				{ FOR_I_N v[i] = 0;		}

	_S	AddReduce() const		{ _S acc=_S(0); FOR_I_N acc += v[i]; return acc; }

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

	friend VecNMask CmpMaskLT( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] < rval[i] ? (1<<i) : 0); return mask; }
	friend VecNMask CmpMaskGT( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] > rval[i] ? (1<<i) : 0); return mask; }
	friend VecNMask CmpMaskEQ( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] ==rval[i] ? (1<<i) : 0); return mask; }
	friend VecNMask CmpMaskNE( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] !=rval[i] ? (1<<i) : 0); return mask; }
	friend VecNMask CmpMaskLE( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] <=rval[i] ? (1<<i) : 0); return mask; }
	friend VecNMask CmpMaskGE( const VecN &lval, const VecN &rval ) { VecNMask mask=0; FOR_I_N mask |= (lval[i] >=rval[i] ? (1<<i) : 0); return mask; }

	friend bool operator ==( const VecN &lval, const VecN &rval ) { return VecNMaskEmpty == CmpMaskNE( lval, rval ) }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { return VecNMaskFull  != CmpMaskEQ( lval, rval ) }

	const _S &operator [] (size_t i) const	{ return v[i]; }
		  _S &operator [] (size_t i)		{ return v[i]; }

	friend VecN	DSqrt( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSqrt( a[i] ); return tmp; }
	friend VecN	DRSqrt( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DRSqrt( a[i] ); return tmp; }
	friend VecN	DPow( const VecN &a,  const VecN &b ){ VecN tmp; FOR_I_N tmp[i] = DPow( a[i], b[i] ); return tmp; }
	friend VecN	DSign( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSign( a[i] ); return tmp; }
	friend VecN	DAbs( const VecN &a	)				 { VecN tmp; FOR_I_N tmp[i] = DAbs( a[i] ); return tmp; }
	friend VecN	DMin( const VecN &a, const VecN &b ) { VecN tmp; FOR_I_N tmp[i] = DMin( a[i], b[i] ); return tmp; }
	friend VecN	DMax( const VecN &a, const VecN &b ) { VecN tmp; FOR_I_N tmp[i] = DMax( a[i], b[i] ); return tmp; }
	friend VecN	DSin( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a )				 { VecN tmp; FOR_I_N tmp[i] = DCos( a[i] ); return tmp; }

#endif

	friend VecN	DSin( const VecN &a ) { VecN tmp; FOR_I_N tmp[i] = DSin( a[i] ); return tmp; }
	friend VecN	DCos( const VecN &a ) { VecN tmp; FOR_I_N tmp[i] = DCos( a[i] ); return tmp; }
	//friend VecN operator * (const _S &lval, const VecN &rval) { return rval * lval; }
};

#undef FOR_I_N

#endif
