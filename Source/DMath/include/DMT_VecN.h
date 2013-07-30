//==================================================================
/// DMT_VecN.h
///
/// Created by Davide Pasca - 2009/5/9
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DMT_VECN_H
#define DMT_VECN_H

#include "DTypes.h"
#include "DMathBase.h"

//==================================================================
/// VecN
//==================================================================

//==================================================================
#define FOR_I_N	for (size_t i=0; i<_TN; ++i)

template<class _TS, size_t _TN>
class VecN
{
public:
	_TS	v[_TN];

	//==================================================================
	VecN()						{}
	VecN( const VecN &v_ )		{ FOR_I_N v[i] = v_.v[i]; }
	VecN( const _TS& a_ )		{ FOR_I_N v[i] = a_;		}
	VecN( const _TS *p_ )		{ FOR_I_N v[i] = p_[i];	}

	void Set( const _TS *p_ )	{ FOR_I_N v[i] = p_[i];	}
	void SetZero()				{ FOR_I_N v[i] = 0;		}

	_TS	AddReduce() const		{ _TS acc=_TS(0); FOR_I_N acc += v[i]; return acc; }

	VecN operator + (const _TS& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] + rval; return tmp; }
	VecN operator - (const _TS& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] - rval; return tmp; }
	VecN operator * (const _TS& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] * rval; return tmp; }
	VecN operator / (const _TS& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] / rval; return tmp; }
	VecN operator + (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] + rval.v[i]; return tmp; }
	VecN operator - (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] - rval.v[i]; return tmp; }
	VecN operator * (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] * rval.v[i]; return tmp; }
	VecN operator / (const VecN &rval) const{ VecN tmp; FOR_I_N tmp.v[i] = v[i] / rval.v[i]; return tmp; }

	VecN operator -() const	{ VecN tmp; FOR_I_N tmp.v[i] = -v[i]; return tmp; }

	VecN operator +=(const VecN &rval)	{ *this = *this + rval; return *this; }

	friend bool operator ==( const VecN &lval, const VecN &rval ) { FOR_I_N if (NOT( lval[i] == rval[i] )) return false; return true; }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { FOR_I_N if (NOT( lval[i] == rval[i] )) return true; return false; }

	const _TS &operator [] (size_t i) const	{ return v[i]; }
		  _TS &operator [] (size_t i)		{ return v[i]; }

};

#define _DTPL template<class _TS,size_t _TN> DFORCEINLINE
#define _DTYP VecN<_TS,_TN>

_DTPL _DTYP DSqrt( const _DTYP &a )					{ _DTYP tmp; FOR_I_N tmp[i] = DSqrt( a[i] ); return tmp; }
_DTPL _DTYP DRSqrt(const _DTYP &a )					{ _DTYP tmp; FOR_I_N tmp[i] = DRSqrt( a[i] ); return tmp; }
_DTPL _DTYP DPow(  const _DTYP &a,  const _DTYP &b)	{ _DTYP tmp; FOR_I_N tmp[i] = DPow( a[i], b[i] ); return tmp; }
_DTPL _DTYP DSign( const _DTYP &a )					{ _DTYP tmp; FOR_I_N tmp[i] = DSign( a[i] ); return tmp; }
_DTPL _DTYP DAbs(  const _DTYP &a	)				{ _DTYP tmp; FOR_I_N tmp[i] = DAbs( a[i] ); return tmp; }
_DTPL _DTYP DMin(  const _DTYP &a, const _DTYP &b )	{ _DTYP tmp; FOR_I_N tmp[i] = DMin( a[i], b[i] ); return tmp; }
_DTPL _DTYP DMax(  const _DTYP &a, const _DTYP &b )	{ _DTYP tmp; FOR_I_N tmp[i] = DMax( a[i], b[i] ); return tmp; }
_DTPL _DTYP DSin(  const _DTYP &a )				 	{ _DTYP tmp; FOR_I_N tmp[i] = DSin( a[i] ); return tmp; }
_DTPL _DTYP DCos(  const _DTYP &a )				 	{ _DTYP tmp; FOR_I_N tmp[i] = DCos( a[i] ); return tmp; }
_DTPL _DTYP operator + (const _TS &lval, const _DTYP &rval) { return rval + lval; }
_DTPL _DTYP operator - (const _TS &lval, const _DTYP &rval) { return rval - lval; }
_DTPL _DTYP operator * (const _TS &lval, const _DTYP &rval) { return rval * lval; }
_DTPL _DTYP operator / (const _TS &lval, const _DTYP &rval) { return rval / lval; }

#undef _DTPL
#undef _DTYP

/*
template <class _TS, size_t _TN> VecNMask CmpMaskLT( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _TS, size_t _TN> VecNMask CmpMaskGT( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _TS, size_t _TN> VecNMask CmpMaskEQ( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _TS, size_t _TN> VecNMask CmpMaskNE( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _TS, size_t _TN> VecNMask CmpMaskLE( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
template <class _TS, size_t _TN> VecNMask CmpMaskGE( const VecN<_TS,_TN> &lval, const VecN<_TS,_TN> &rval ) { DASSERT( 0 ); return VecNMaskEmpty; }
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
	friend VecNMask CmpMaskGT( const VecN &lval, const VecN &rval ) { return ~_mm512_cmple_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskEQ( const VecN &lval, const VecN &rval ) { return _mm512_cmpeq_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskNE( const VecN &lval, const VecN &rval ) { return _mm512_cmpneq_ps( lval.v, rval.v ); }
	friend VecNMask CmpMaskLE( const VecN &lval, const VecN &rval ) { return _mm512_cmple_ps( lval.v, rval.v );  }
	friend VecNMask CmpMaskGE( const VecN &lval, const VecN &rval ) { return ~_mm512_cmplt_ps( lval.v, rval.v );  }

	friend bool operator ==( const VecN &lval, const VecN &rval ) { return VecNMaskEmpty == CmpMaskNE( lval, rval ); }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { return VecNMaskFull  != CmpMaskEQ( lval, rval ); }

	const float &operator [] (size_t i) const	{ return v.v[i]; }
		  float &operator [] (size_t i)			{ return v.v[i]; }

#else
//==================================================================
/// No hardware SIMD
//==================================================================
public:
	float	v[DMT_SIMD_FLEN];

	//==================================================================
	VecN()						{}
	VecN( const VecN &v_ )		{ FOR_I_N v[i] = v_.v[i]; }
	VecN( const float& a_ )		{ FOR_I_N v[i] = a_;		}
	VecN( const float *p_ )		{ FOR_I_N v[i] = p_[i];	}

	//void Set( const float *p_ )	{ FOR_I_N v[i] = p_[i];	}
	void SetZero()				{ FOR_I_N v[i] = 0;		}

	float	AddReduce() const		{ float acc=float(0); FOR_I_N acc += v[i]; return acc; }

	VecN operator + (const float& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] + rval; return tmp; }
	VecN operator - (const float& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] - rval; return tmp; }
	VecN operator * (const float& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] * rval; return tmp; }
	VecN operator / (const float& rval) const	{ VecN tmp; FOR_I_N tmp.v[i] = v[i] / rval; return tmp; }
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

	friend bool operator ==( const VecN &lval, const VecN &rval ) { return VecNMaskEmpty == CmpMaskNE( lval, rval ); }
	friend bool operator !=( const VecN &lval, const VecN &rval ) { return VecNMaskFull  != CmpMaskEQ( lval, rval ); }

	const float &operator [] (size_t i) const	{ return v[i]; }
		  float &operator [] (size_t i)		{ return v[i]; }
#endif
};

// specialization of functions
#define _DTPL template<> DFORCEINLINE
#define _DTYP VecN<float,DMT_SIMD_FLEN>

#if defined(DMATH_USE_M128)
//==================================================================
//==================================================================
//==================================================================
_DTPL _DTYP	DSqrt( const _DTYP &a )	{ return _mm_sqrt_ps( a.v );	}
_DTPL _DTYP	DRSqrt( const _DTYP &a )	{ return _mm_rsqrtnr_ps( a.v );	}
//_DTPL _DTYP	DRSqrt( const _DTYP &a )	{ return _mm_rsqrt_ps( a.v );	}

// TODO: get a proper _mm_pow_ps !!!
//_DTPL _DTYP	DPow( const _DTYP &a, const _DTYP &b ){ return _mm_pow_ps( a.v, b.v );	}
_DTPL _DTYP	DPow( const _DTYP &a, const _DTYP &b )
{
	_DTYP tmp;
	for (size_t i=0; i < DMT_SIMD_FLEN; ++i)
		tmp[i] = powf( a[i], b[i] );
	
	return tmp;
}

//==================================================================
_DTPL _DTYP	DSign( const _DTYP &a )
{
	const __m128	zero		= _mm_setzero_ps();
	const __m128	selectPos	= _mm_cmpgt_ps( a.v, zero );	// > 0
	const __m128	selectNeg	= _mm_cmplt_ps( a.v, zero );	// < 0

	__m128	res =		  _mm_and_ps( selectPos, _mm_set_ps1(  1.0f ) );
	res = _mm_or_ps( res, _mm_and_ps( selectNeg, _mm_set_ps1( -1.0f ) ) );

	return res;
}

_DTPL _DTYP	DAbs( const _DTYP &a )
{
	static const U32 notSignBitMask = ~0x80000000;
	return _mm_and_ps( a.v, _mm_set_ps1( *(float *)&notSignBitMask ) );
}

_DTPL _DTYP	DMin( const _DTYP &a, const _DTYP &b )	{	return _mm_min_ps( a.v, b.v );	}
_DTPL _DTYP	DMax( const _DTYP &a, const _DTYP &b )	{	return _mm_max_ps( a.v, b.v );	}

#elif defined(DMATH_USE_M512)
//==================================================================
//==================================================================
//==================================================================
_DTPL _DTYP	DSqrt( const _DTYP &a )		{ return _mm512_sqrt_ps( a.v );	}
_DTPL _DTYP	DRSqrt( const _DTYP &a )	{ return _mm512_rsqrt_ps( a.v );	}

_DTPL _DTYP	DPow( const _DTYP &a, const _DTYP &b ){ return _mm512_pow_ps( a.v, b.v );	}

//==================================================================
_DTPL _DTYP	DSign( const _DTYP &a )
{
	const __m512	zero		= _mm512_setzero_ps();
	const __mmask	selectPos	= _mm512_cmpnle_ps( a.v, zero );	// >
	const __mmask	selectNeg	= _mm512_cmplt_ps( a.v, zero );		// <

	__m512	res = _mm512_mask_movd( zero, selectPos, _mm512_set_1to16_ps(  1.0f ) );
			res = _mm512_mask_movd( res , selectNeg, _mm512_set_1to16_ps( -1.0f ) );

	return res;
}

_DTPL _DTYP	DAbs( const _DTYP &a )
{
	return _mm512_maxabs_ps( a.v, a.v );
}

_DTPL _DTYP	DMin( const _DTYP &a, const _DTYP &b )	{	return _mm512_min_ps( a.v, b.v );	}
_DTPL _DTYP	DMax( const _DTYP &a, const _DTYP &b )	{	return _mm512_max_ps( a.v, b.v );	}
#else
//==================================================================
//==================================================================
//==================================================================
#endif

#undef _DTPL
#undef _DTYP

#undef FOR_I_N

#endif
