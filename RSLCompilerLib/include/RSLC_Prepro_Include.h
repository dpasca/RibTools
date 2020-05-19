//==================================================================
/// RSLC_Prepro_Include.h
///
/// Created by Davide Pasca - 2010/2/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_INCLUDE_H
#define RSLC_PREPRO_INCLUDE_H

#include "DSystem/include/DIO_FileManager.h"
#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
void HandleInclude(
				DVec<Fat8>				&text,
				size_t					i,
				size_t					lineEnd,
				size_t					includePoint,
				DIO::FileManagerBase	&fmanager,
				FatBase					&fatBase,
				const char				*pCurShaderDir );

//==================================================================
}
//==================================================================
}

#endif
