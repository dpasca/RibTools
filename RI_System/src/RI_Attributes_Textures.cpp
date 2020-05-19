//==================================================================
/// RI_Attributes_Textures.cpp
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Attributes.h"
#include "RI_State.h"
#include "RI_Texture.h"
#include "RI_Res_SearchPathScanner.h"

//==================================================================
namespace RI
{

//==================================================================
Texture *Attributes::GetTexture( const char *pTextureName ) const
{
	// try see if we have it loaded already
	Texture	*pTexture =
			(Texture *)mpResManager->FindResource( pTextureName,
													ResourceBase::TYPE_TEXTURE );

	if ( pTexture )
		return pTexture;

	DStr	shaderFullPathName;

	shaderFullPathName = mpState->FindResFile( pTextureName, Options::SEARCHPATH_TEXTURE );

	if ( shaderFullPathName.length() )
	{
/*
		try {
*/
			// grab the file
			DUT::MemFile	file;
			mpState->GetFileManager().GrabFile( shaderFullPathName.c_str(), file );

			// load the texture based on the file
			pTexture = DNEW Texture( pTextureName, file );
/*
		} catch ( ... )
		{
			mpState->EXCEPTPrintf( "Could not load '%s' !", pTextureName );
			return NULL;
		}
*/

		mpResManager->AddResource( pTexture );
	}

	return pTexture;
}

//==================================================================
}
