//==================================================================
/// RI_Texture.h
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_TEXTURE_H
#define RI_TEXTURE_H

//==================================================================
namespace RI
{

//==================================================================
/// Texture
//==================================================================
class Texture : public ResourceBase
{
public:

	Texture( const char *pTexName, FileManagerBase &fileManager );
};


//==================================================================
}

#endif
