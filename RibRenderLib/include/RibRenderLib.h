//==================================================================
/// RibRenderLib.h
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_H
#define RIBRENDERLIB_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"

#include "RI_System/include/RI_Render.h"
#include "RI_System/include/RI_Net_Protocol.h"
#include "RI_System/include/RI_Machine.h"
#include "RI_System/include/RI_Framework.h"

#include "RibRenderLib_Net.h"

//==================================================================
namespace RRL
{

//==================================================================
/// Render
//==================================================================
class Render
{

public:
	Render(
		const char			*pFileName,
		RI::Machine			&machine,
		RI::FileManagerBase	&fileManager,
		bool				verbose=false );
};

//==================================================================
}

#endif
