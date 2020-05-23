//==================================================================
/// DispDriverFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>
#include "DispDrivers/include/DispDriverFile.h"
#include "DImage/include/DImage_JPEG.h"
#include "DImage/include/DImage_TIFF.h"
#include "DSystem/include/DUtils_Files.h"

//==================================================================
// DispDriverFile
//==================================================================
DispDriverFile::DispDriverFile( const char *pFileName, const DIMG::Image &srcImg )
{
    const char *pExt = DUT::GetFileNameExt( pFileName );

    if ( 0 == strcasecmp( pExt, "jpg" ) ||
         0 == strcasecmp( pExt, "jpeg" ) )
    {
        DIMG::SaveJPEG( srcImg, pFileName );
    }
    else
    if ( 0 == strcasecmp( pExt, "tif" ) ||
         0 == strcasecmp( pExt, "tiff" ) )
    {
        DIMG::SaveTIFF( srcImg, pFileName );
    }
    else
    {
        DASSTHROW( 0, ("Unsupported file type '%s'", pExt) );
    }
}

//==================================================================
DispDriverFile::~DispDriverFile()
{
}
