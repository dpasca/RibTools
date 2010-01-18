//==================================================================
/// RI_Texture.cpp
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Texture.h"

//==================================================================
namespace RI
{

//==================================================================
/// Texture
//==================================================================
Texture::Texture( const char *pName, FileManagerBase &fileManager ) :
	ResourceBase(pName, ResourceBase::TYPE_TEXTURE)
{
	DUT::MemFile	file;

	fileManager.GrabFile( pName, file );
}

//==================================================================
}
