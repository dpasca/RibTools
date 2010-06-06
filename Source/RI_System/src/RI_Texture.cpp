//==================================================================
/// RI_Texture.cpp
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "DImage/include/DImage_BMP.h"
#include "DImage/include/DImage_TIFF.h"
#include "RI_Texture.h"

#define GAUSS_HDIM	2
#define GAUSS_DIM	(GAUSS_HDIM*2 + 1)

//==================================================================
namespace RI
{

//==================================================================
static float _gGaussKernel[GAUSS_DIM][GAUSS_DIM];

//==================================================================
static void makeGaussKernel()
{
	static const float sigma = 1.f;

	for (int i=-GAUSS_HDIM; i <= GAUSS_HDIM; ++i)
	{
		for (int j=-GAUSS_HDIM; j <= GAUSS_HDIM; ++j)
		{
			float x = (float)j;
			float y = (float)i;

			float	sigmaSqr = sigma * sigma;

			_gGaussKernel[j][i] =
				float(
					(1 / (2 * (float)M_PI * sigmaSqr)) * exp( -(x*x + y*y) / (2 * sigmaSqr) )
				);
		}
	}
}

//==================================================================
static const char *StrFindLastChar( const char *pStr, char ch )
{
	size_t	len = strlen( pStr );

	for (size_t i=len; i > 0; --i)
	{
		if ( pStr[i-1] == ch )
			return &pStr[i-1];
	}

	return NULL;
}

//==================================================================
/// Texture
//==================================================================
Texture::Texture( const char *pName, DUT::MemFile &file ) :
	ResourceBase(pName, ResourceBase::TYPE_TEXTURE)
{
	const char	*pDotExt = StrFindLastChar( pName, '.' );

	if NOT( pDotExt )
	{
		DASSTHROW( 0, ("Missing file extension ! %s", pName) );
	}
	
	if ( 0 == strcasecmp( pDotExt, ".tif" ) || 0 == strcasecmp( pDotExt, ".mip" ) )
	{
		DIMG::LoadTIFF( mImage, file, pName );
	}
	else
	if ( 0 == strcasecmp( pDotExt, ".bmp" ) )
	{
		DIMG::LoadBMP( mImage, file, pName );
	}

	mS_to_X = Float_( (float)mImage.mWd-1 );
	mT_to_Y = Float_( (float)mImage.mHe-1 );

	static bool kernelDone;
	if NOT( kernelDone )
	{
		makeGaussKernel();
		kernelDone = true;
	}
}

//==================================================================
void Texture::Sample_1_1x1( Float_ &dest, const Float_ &s00, const Float_ &t00 ) const
{
	// assume wrapping for now

	Float_	x = s00 * mS_to_X;
	Float_	y = t00 * mT_to_Y;

	// TODO: use SIMD to do the conversion !
	//Int_	xi, yi;
	for (size_t i=0; i < DMT_SIMD_FLEN; ++i)
	{
		int xi = ((int)floorf( x[i] )) % (int)mImage.mWd;
		int yi = ((int)floorf( y[i] )) % (int)mImage.mHe;

		if ( xi < 0 ) xi += mImage.mWd;
		if ( yi < 0 ) yi += mImage.mHe;

		//xi = D::Clamp( xi, 0, (int)mImage.mWd-1 );
		//yi = D::Clamp( yi, 0, (int)mImage.mHe-1 );

		dest[i] = (float)*mImage.GetPixelPtrR( xi, yi );
	}

	// assume byte values for now !

	dest = dest * Float_( 1.0f / 255.0f );
}

//==================================================================
void Texture::Sample_1_filter( Float_ &dest, const Float_ &s00, const Float_ &t00 ) const
{
	// assume wrapping for now
	Float_	spix = s00 * mS_to_X;
	Float_	tpix = t00 * mT_to_Y;

	int	imgw = mImage.mWd;
	int	imgh = mImage.mHe;

	Float_	tmp( 0.f );

	for (size_t smdi=0; smdi < DMT_SIMD_FLEN; ++smdi)
	{
		float	s = spix[smdi];
		float	t = tpix[smdi];

		float	s_flr = floorf( s );
		float	t_flr = floorf( t );

		float	u = s - s_flr;
		float	v = t - t_flr;

		int		x00 = (int)s_flr;
		int		y00 = (int)t_flr;

		U8	*pPixels[4];

		mImage.GetPixelQuadWrapR( x00, y00, pPixels );

		float	c00 = (float)*pPixels[0];
		float	c01 = (float)*pPixels[1];
		float	c10 = (float)*pPixels[2];
		float	c11 = (float)*pPixels[3];

		float	top = (1 - u) * c00 + (u - 0) * c01;
		float	bot = (1 - u) * c10 + (u - 0) * c11;

		tmp[smdi] = (1 - v) * top + (v - 0) * bot;
	}

	dest = tmp * Float_( 1.0f / 255.0f );
}

//==================================================================
}
