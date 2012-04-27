//==================================================================
/// DUtils_Random.h
///
/// Created by Davide Pasca - 2010/6/16
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DUTILS_RANDOM_H
#define DUTILS_RANDOM_H

#include "DTypes.h"

//#define DUT_USE_CHEAP_RANDOM

//==================================================================
namespace DUT
{

//==================================================================
// Taken from http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/VERSIONS/C-LANG/RandMT.cc
class RandMT
{
	static const int N =	624;                // length of state vector
	static const int M =	397;                // a period parameter
	static const U32 K =	0x9908B0DFU;        // a magic constant

	// If you want a single generator, consider using a singleton class 
	// instead of trying to make these static.
	U32		state[N+1];  // state vector + 1 extra to not violate ANSI C
	U32		*next;       // next random value is computed from here
	U32		initseed;    //
	int		left;        // can *next++ this many times before reloading

	inline U32 hiBit(U32 u) { 
		return u & 0x80000000U;    // mask all but highest   bit of u
	}

	inline U32 loBit(U32 u) { 
		return u & 0x00000001U;    // mask all but lowest    bit of u
	}

	inline U32 loBits(U32 u) { 
		return u & 0x7FFFFFFFU;   // mask     the highest   bit of u
	}

	inline U32 mixBits(U32 u, U32 v) {
		return hiBit(u)|loBits(v);  // move hi bit of u to hi bit of v
	}

	U32 reloadMT() ;

public:
	RandMT() ;
	RandMT(U32 seed) ;
	void seedMT(U32 seed) ;

	U32 randomMT()
	{
		U32 y;

		if(--left < 0)
			return(reloadMT());

		y  = *next++;
		y ^= (y >> 11);
		y ^= (y <<  7) & 0x9D2C5680U;
		y ^= (y << 15) & 0xEFC60000U;
		return(y ^ (y >> 18));
	}

	template <U32 precisionLog2>
	float RandomFloat()
	{
		U32		precisionN1 = (1 << precisionLog2) - 1;
		float	ooPrecision = 1.f / (float)precisionN1;

		return (float)(randomMT() & precisionN1) * ooPrecision;
	}

	template <U32 precisionLog2>
	float RandomFloatRange( float x0, float x1 )
	{
		U32		precisionN1 = (1 << precisionLog2) - 1;
		float	ooPrecision = 1.f / (float)precisionN1;

		float t = (float)(randomMT() & precisionN1) * ooPrecision;

		return t * (x1 - x0) + x0;
	}
};

//==================================================================
#if !defined(DUT_USE_CHEAP_RANDOM)

//==================================================================
inline void RandomSeed( u_int seed )	{ extern RandMT _gsRandMT; _gsRandMT.seedMT( seed ); }
inline u_int RandomU32()				{ extern RandMT _gsRandMT; return _gsRandMT.randomMT(); }

//==================================================================
inline float Random()
{
	extern RandMT _gsRandMT;
	return _gsRandMT.RandomFloat<14>();
}

#else
	
	//==================================================================
	extern u_int _g_m_w;
	extern u_int _g_m_z;
	
	//==================================================================
	inline void RandomSeed( u_int seed )
	{
		_g_m_w = seed & 0xffff;
		_g_m_z = seed >> 16;
	}
	
	//==================================================================
	inline u_int RandomU32()
	{
		_g_m_z = 36969 * (_g_m_z & 65535) + (_g_m_z >> 16);
		_g_m_w = 18000 * (_g_m_w & 65535) + (_g_m_w >> 16);
		return (_g_m_z << 16) + _g_m_w;
	}
	
	//==================================================================
	inline float Random()
	{
		U32 val = RandomU32();
		
		return (float)(val & 0xffffff) / 0xffffff;
	}

#endif

inline float RandomRange( float a, float b )
{
	return D::Lerp( a, b, Random() );
}

inline float RandomError( float baseValue, float errorScale )
{
	return baseValue + Random() * baseValue * errorScale;
}

//==================================================================
}

#endif
