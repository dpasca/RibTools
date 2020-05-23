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
    Float_	mS_to_X;
    Float_	mT_to_Y;

public:
    DIMG::Image	mImage;

    Texture( const char *pTexName, DUT::MemFile &file );

    void Sample_1_1x1( Float_ &dest, const Float_ &s00, const Float_ &t00 ) const;

    void Sample_1_filter( Float_ &dest, const Float_ &s00, const Float_ &t00 ) const;
/*
    void Sample_1_rect(
                Float_ &dest,
                const Float_ &s00, const Float_ &t00,
                const Float_ &s01, const Float_ &t01,
                const Float_ &s10, const Float_ &t10,
                const Float_ &s11, const Float_ &t11 ) const;
*/
};


//==================================================================
}

#endif
