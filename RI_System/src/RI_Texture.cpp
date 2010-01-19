//==================================================================
/// RI_Texture.cpp
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "DImage/include/DImage_BMP.h"
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
	
	if ( 0 == strcasecmp( pDotExt, ".bmp" ) )
	{
		DIMG::LoadBMP( mImage, file );
	}
}

//==================================================================
}
