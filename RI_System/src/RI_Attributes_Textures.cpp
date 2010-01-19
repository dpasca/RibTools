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
Texture *Attributes::loadTexture( const char *pBasePath, const char *pName, bool &out_fileExists ) const
{
	FileManagerBase	&fmanager = mpState->GetFileManager();

	char	buff[2048];

	sprintf( buff, "%s/%s", pBasePath, pName );
	if NOT( fmanager.FileExists( buff ) )
	{
		out_fileExists = false;
		return NULL;
	}

	out_fileExists = true;

	Texture *pTexture = NULL;

	try {
		// grab the file
		DUT::MemFile	file;
		fmanager.GrabFile( buff, file );

		// load the texture based on the file
		pTexture = DNEW Texture( pName, file );
	} catch ( ... )
	{
		mpState->EXCEPTPrintf( "Could not load '%s' !", pName );
		return NULL;
	}

	mpResManager->AddResource( pTexture );

	return pTexture;
}

//==================================================================
Texture *Attributes::GetTexture( const char *pTextureName ) const
{
	// try see if we have it loaded already
	Texture	*pTexture =
			(Texture *)mpResManager->FindResource( pTextureName,
													ResourceBase::TYPE_TEXTURE );

	if ( pTexture )
		return pTexture;

	// try look into the search path
	const DVec<DStr>	&spaths = mpState->GetCurOptions().mSearchPaths[ Options::SEARCHPATH_TEXTURE ];

	SearchPathScanner	pathScanner( mpState->GetBaseDir(), NULL, spaths );

	std::string	usePath;
	bool		usePathIsAbsolute;

	while ( pathScanner.GetNext( usePath, usePathIsAbsolute ) )
	{
		bool	found;
		if ( pTexture = loadTexture( usePath.c_str(), pTextureName, found ) )
			return pTexture;

		if ( !found && !usePathIsAbsolute )
		{
			// WARNING: tricky path discovery.. we also try ribfilepath/searchpath
			usePath = std::string( mpState->GetBaseDir() ) + "/" + usePath;

			if ( pTexture = loadTexture( usePath.c_str(), pTextureName, found ) )
				return pTexture;
		}

		// in case we found it, but there is an error of some sort..
		// ..just give up instead of trying different paths
		if ( found )
			break;
	}

	return NULL;
}

//==================================================================
}
