//==================================================================
/// DImage_BMP.h
///
/// Created by Davide Pasca - 2010/1/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DIMAGE_BMP_H
#define DIMAGE_BMP_H

#include "DImage.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace DIMG
{

//==================================================================
void LoadBMP( Image &img, DUT::MemFile &readFile, const char *pFName );

//==================================================================
}

#endif
