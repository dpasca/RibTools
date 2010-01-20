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

//==================================================================
namespace RI
{

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
		int xi = ((int)floorf( x[0] )) % (int)mImage.mWd;
		int yi = ((int)floorf( y[0] )) % (int)mImage.mHe;

		if ( xi < 0 ) xi += mImage.mWd;
		if ( yi < 0 ) yi += mImage.mHe;

		dest[i] = (float)*mImage.GetPixelPtrR( xi, yi );
	}

	// assume byte values for now !

	dest = dest * Float_( 1.0f / 255.0f );
}

//==================================================================
}
