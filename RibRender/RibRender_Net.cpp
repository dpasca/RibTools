//==================================================================
/// RibRender_Net.cpp
///
/// Created by Davide Pasca - 2010/1/28
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRender.h"

//==================================================================
bool GetServersList( int argc, char **argv, DVec<RRL::NET::Server> &list )
{
    list.clear();

    for (int i=1; i < argc; ++i)
    {
        if ( 0 == strcasecmp( "-server", argv[i] ) )
        {
            if ( (i+1) >= argc )
            {
                printf( "Missing server definition.\n" );
                return false;
            }

            RRL::NET::Server &servEntry = Dgrow( list );

            char	*pContext = NULL;
            
            char *pToken = strtok_r( argv[i+1], ":", &pContext );
            if ( pToken )
            {
                servEntry.mAddressName = pToken;
                if ( pToken = strtok_r( NULL, ":", &pContext ) )
                {
                    servEntry.mPortToCall = atoi( pToken );

                    if ( servEntry.mPortToCall <= 0 && servEntry.mPortToCall >= 65536 )
                    {
                        printf( "Invalid port range.\n" );
                        return false;
                    }
                }
            }
            else
                servEntry.mAddressName = argv[i+1];

            i += 1;
        }
    }

    return true;
}

//==================================================================
void InitServers( CmdParams &cmdPars, const char *defaultShadersDir )
{
    // prepare the template job header for the servers
    RRL::NET::MsgRendJob	netRendJob;
    strcpy_s( netRendJob.FileName	, cmdPars.pInFileName );
    strcpy_s( netRendJob.BaseDir	, cmdPars.baseDir.c_str() );
    strcpy_s( netRendJob.DefaultResourcesDir, defaultShadersDir );
    netRendJob.ForcedLongDim = cmdPars.forcedlongdim;
    netRendJob.ForcedWd = -1;
    netRendJob.ForcedHe = -1;

    // connect to all servers
    printf( "Connecting to servers...\n" );
    RRL::NET::ConnectToServers( cmdPars.servList, 3 * 1000 );

    // for each server..
    for (size_t i=0; i < cmdPars.servList.size(); ++i)
    {
        // if the connection was successful
        if ( cmdPars.servList[i].IsConnected() )
        {
            // print a nice message and start by sending a job header..
            printf( "Successfully connected to %s:%i\n", cmdPars.servList[i].mAddressName.c_str(), cmdPars.servList[i].mPortToCall );
            cmdPars.servList[i].mpPakMan->Send( &netRendJob, sizeof(netRendJob) );
        }
        else
        {
            printf( "Failed to connect to %s:%i\n", cmdPars.servList[i].mAddressName.c_str(), cmdPars.servList[i].mPortToCall );
        }
    }
}

