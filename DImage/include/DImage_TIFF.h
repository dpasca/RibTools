//==================================================================
/// DImage_TIFF.h
///
/// Created by Davide Pasca - 2010/1/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DIMAGE_TIFF_H
#define DIMAGE_TIFF_H

#include "DImage.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace DIMG
{

//==================================================================
void LoadTIFF( Image &img, DUT::MemFile &readFile, const char *pFName );

//==================================================================
}

#endif
