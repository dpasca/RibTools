//==================================================================
/// RSLC_Prepro_ClearChunk.h
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_CLEARCHUNK_H
#define RSLC_PREPRO_CLEARCHUNK_H

#include "DSystem/include/DContainers.h"
#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
size_t ClearChunk( DVec<Fat8> &text, size_t start=0, size_t end=NPOS );


//==================================================================
}
//==================================================================
}

#endif
