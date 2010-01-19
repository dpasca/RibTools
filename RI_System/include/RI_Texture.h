//==================================================================
/// RI_Texture.h
///
/// Created by Davide Pasca - 2010/1/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_TEXTURE_H
#define RI_TEXTURE_H

#include "RI_Resource.h"
#include "DImage/include/DImage.h"

//==================================================================
namespace DUT
{
	class MemFile;
};

//==================================================================
namespace RI
{

//==================================================================
/// Texture
//==================================================================
class Texture : public ResourceBase
{
public:
	DIMG::Image	mImage;	

	Texture( const char *pTexName, DUT::MemFile &file );
};


//==================================================================
}

#endif
