//==================================================================
/// RI_Render.h
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_RENDER_H
#define RI_RENDER_H

#include "DSystem/include/DNetwork.h"

//==================================================================
namespace RI
{

//==================================================================
class Machine;

//==================================================================
/// Render
//==================================================================
class Render
{

public:
	Render(
		const char *pFileName,
		Machine		&machine,
		SOCKET		ioSocket=INVALID_SOCKET,
		bool		verbose=false );
};

//==================================================================
}

#endif
