//==================================================================
/// RibRenderFile.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERFILE_H
#define RIBRENDERFILE_H

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
#include "RenderOutputFile.h"
#include "RibRenderFile.h"

void PrintUsage( int argc, char **argv );

int ServerMain( int argc, char **argv );
int ClientMain( int argc, char **argv );


#endif