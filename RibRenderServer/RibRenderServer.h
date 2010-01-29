//==================================================================
/// RibRenderServer.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERSERVER_H
#define RIBRENDERSERVER_H

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

#define APPNAME		"RibRenderServer"
#define APPVERSION	"0.9"

#endif