//==================================================================
/// RibRender.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDER_H
#define RIBRENDER_H

#include <stdio.h>
#include <stdexcept>

#if defined(WIN32)
#include <direct.h>
#else
//#include <direct.h>
#endif

#include "RibRenderLib/include/RibRenderLib.h"
#include "DSystem/include/DUtils.h"
#include "DSystem/include/DUtils_Files.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DCriticalSection.h"

#define APPNAME		"RibRender"
#define APPVERSION	"0.9"

//==================================================================
struct CmdParams
{
	const char				*pInFileName;
	int						forcedlongdim;
	DVec<RRL::NET::Server>	servList;

	DStr					baseDir;

	CmdParams() :
		pInFileName		(NULL),
		forcedlongdim	(-1)
	{
	}
};

//==================================================================
bool GetServersList( int argc, char **argv, DVec<RRL::NET::Server> &list );
void InitServers( CmdParams &cmdPars, const char *defaultShadersDir );

#endif
