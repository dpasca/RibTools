//==================================================================
/// RibToolsBase.cpp
///
/// Created by Davide Pasca - 2010/2/16
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(WIN32)
#include <Windows.h>
#endif

#include "DSystem/include/DUtils_Files.h"
#include "RibToolsBase.h"

//==================================================================
namespace RTB
{

//==================================================================
static void tragicEnd()
{
    fprintf_s( stderr,
        "FATAL ERROR:\n"
        "  Could not find RibTools' directory.\n"
        "  Please set the RIBTOOLS_DIR environment variable to point to the appropriate path.\n"
        "  \"Appropriate path\" being where RibTools binaries are, along with the ribtools.ini file.\n" );

    exit( -1 );
}

//==================================================================
// we place this here for now
DStr FindRibToolsDir( char *argv[] )
{
    char buff[1024];

#if defined(WIN32)
    if NOT( GetEnvironmentVariableA( "RIBTOOLS_DIR", buff, sizeof(buff)-1 ) )
        buff[0] = 0;

#elif defined(__linux__)
    char *pEnvVar = getenv( "RIBTOOLS_DIR" );

    if ( pEnvVar )
        strcpy_s( buff, pEnvVar );
    else
        buff[0] = 0;

#endif

    char buff2[1024];

    if ( buff[0] )
    {
        sprintf_s( buff2, "%s/ribtools.ini", buff );
        if ( DUT::FileExists( buff2 ) )
            return buff;
    }
    else
    {
        DStr exePath = DUT::GetDirNameFromFPathName( argv[0] );

        if ( exePath.length() )
        {
            sprintf_s( buff2, "%s/ribtools.ini", exePath.c_str() );
            if ( DUT::FileExists( buff2 ) )
                return exePath.c_str() ;
        }
    }

    // basically exists the application here !
    tragicEnd();

    return "";
}

//==================================================================
}
