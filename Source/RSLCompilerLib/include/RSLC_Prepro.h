//==================================================================
/// RSLC_Prepro.h
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_H
#define RSLC_PREPRO_H

#include "DSystem/include/DContainers.h"
#include "DSystem/include/DIO_FileManager.h"

#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
class Prepro
{
public:
	Prepro(
		DIO::FileManagerBase	&fmanager,
		FatBase					&fatBase,
		const DVec<Fat8>		&inText,
		const char				*pBaseInclude,
		const char				*pCurShaderDir,
		DVec<Fat8>				&out_text );
};

//==================================================================
}
//==================================================================
}

#endif
