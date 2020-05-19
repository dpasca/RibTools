//==================================================================
/// RI_Noise.h
///
/// Created by Davide Pasca - 2009/5/17
/// based on http://mrl.nyu.edu/~perlin/doc/oscar.html
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_NOISE_H
#define RI_NOISE_H

//==================================================================
namespace RI
{

//==================================================================
/// Noise
//==================================================================
class Noise
{
	static const int B	= 0x100;
	static const int BM	= 0xff;

	static const int N	= 0x1000;
	static const int NP	= 12;   /* 2^N */
	static const int NM = 0xfff;

	static int		mP[B + B + 2];
	static float	mG3[B + B + 2][3];
	static float	mG2[B + B + 2][2];
	static float	mG1[B + B + 2];
	static bool		mStart;

	#define	RI_NS_O0X  2.3f
	#define	RI_NS_O0Y  3.1f
	#define	RI_NS_O0Z  5.4f

	#define	RI_NS_O1X  7.1f
	#define	RI_NS_O1Y  11.5f
	#define	RI_NS_O1Z  13.9f

	#define	RI_NS_O2X  17.2f
	#define	RI_NS_O2Y  19.6f
	#define	RI_NS_O2Z  23.5f

	//==================================================================
	static void init();

public:
	static float noise1( const float arg );
	static float noise2( const float vec[2] );
	static float noise3( const float vec[3] );

	//==================================================================
	inline static Float_ unoise1( const Float_ &sca )
	{
		Float_	outSca;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			outSca[i] = noise1( sca[i] );
		}
		return outSca * 0.5f + 0.5f;
	}
	//==================================================================
	inline static Float_ unoise1( const Float2_ &vec )
	{
		Float_	outSca;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			float	sisdVec[3] = { vec[0][i], vec[1][i] };
			outSca[i] = noise2( sisdVec );
		}
		return outSca * 0.5f + 0.5f;
	}
	//==================================================================
	inline static Float_ unoise1( const Float3_ &vec )
	{
		Float_	outSca;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			float	sisdVec[3] = { vec[0][i], vec[1][i], vec[2][i] };
			outSca[i] = noise3( sisdVec );
		}
		return outSca * 0.5f + 0.5f;
	}

	//==================================================================
	inline static Float3_ unoise3( const Float_ &sca )
	{
		Float3_	outVec;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			outVec[0][i] = noise1( sca[i] + RI_NS_O0X );
			outVec[1][i] = noise1( sca[i] + RI_NS_O1X );
			outVec[2][i] = noise1( sca[i] + RI_NS_O2X );
		}
		return outVec * 0.5f + 0.5f;
	}
	//==================================================================
	inline static Float3_ unoise3( const Float2_ &vec )
	{
		Float3_	outVec;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			Float2	sisdVec0( vec[0][i], vec[1][i] );
			Float2	sisdVec1 = sisdVec0 + Float2( RI_NS_O0X, RI_NS_O0Y );
			Float2	sisdVec2 = sisdVec0 + Float2( RI_NS_O1X, RI_NS_O1Y );

			outVec[0][i] = noise2( &sisdVec0[0] );
			outVec[1][i] = noise2( &sisdVec1[0] );
			outVec[2][i] = noise2( &sisdVec2[0] );
		}
		return outVec * 0.5f + 0.5f;
	}
	//==================================================================
	inline static Float3_ unoise3( const Float3_ &vec )
	{
		Float3_	outVec;
		for (int i=0; i < DMT_SIMD_FLEN; ++i)
		{
			Float3	sisdVec0( vec[0][i], vec[1][i], vec[2][i] );
			Float3	sisdVec1 = sisdVec0 + Float3( RI_NS_O0X, RI_NS_O0Y, RI_NS_O0Z );
			Float3	sisdVec2 = sisdVec0 + Float3( RI_NS_O1X, RI_NS_O1Y, RI_NS_O1Z );

			outVec[0][i] = noise3( &sisdVec0[0] );
			outVec[1][i] = noise3( &sisdVec1[0] );
			outVec[2][i] = noise3( &sisdVec2[0] );
		}
		return outVec * 0.5f + 0.5f;
	}

};

//==================================================================
}

#endif
