//==================================================================
/// RibRenderFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdio.h>
#include <stdexcept>

#if defined(WIN32)
#include <direct.h>
#else
//#include <direct.h>
#endif

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DNetwork.h"
#include "RI_System/include/RI_Render.h"
#include "RI_System/include/RI_Machine.h"
#include "RI_System/include/RI_FrameworkREYES.h"
#include "RenderOutputFile.h"
#include "RibRenderFile.h"

//===============================================================
static bool renderFile(
	const char *pFileName,
	const char *pDefaultResourcesDir,
	RenderOutputFile *pRenderOutput,
	int forcedWd=-1,
	int forcedHe=-1 )
{
	DStr	baseDir = DUT::GetDirNameFromFPathName( pFileName );

	char	defaultShadersDir[4096];
	sprintf_s( defaultShadersDir, "%s/Shaders", pDefaultResourcesDir );
	//printf( "Base Dir: %s\n", baseDir.c_str() );
	//printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::HiderREYES::Params	hiderParams;
	RI::FrameworkREYES		framework( pRenderOutput, hiderParams );
	RI::Machine				machine( &framework, baseDir.c_str(), defaultShadersDir, forcedWd, forcedHe );

	try
	{
		RI::Render	render( pFileName, machine );

		return true;
	}
	catch ( ... )
	{
		return false;
	}

}

//===============================================================
static void printUsage( int argc, char **argv )
{
	printf( "Usage:\n" );
	printf( "\t%s <rib file> <output JPEG file>\n", argv[0] );
	printf( "\t%s -server [-port <port number>]\n", argv[0] );
}

//===============================================================
static void serverTask( SOCKET clientSock )
{
	printf( "Rendering for %i... yeah, right !!\n", clientSock );
}

//===============================================================
static int serverMain( int argc, char **argv )
{
	int	port = 32323;

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-port", argv[i] ) )
		{
			if ( (i+1) >= argc )
			{
				printf( "Missing port value.\n" );
				return -1;
			}

			port = atoi( argv[i+1] );
			if ( port <= 0 && port >= 65536 )
			{
				printf( "Invalid port range.\n" );
				return -1;
			}

			i += 1;
		}
	}
	
	DNET::Listener	listener( port );

	if NOT( listener.Start() )
	{
		printf( "Failed to open port %i\n", port );
		return -1;
	}
	printf( "Listening on port %i for connections..\n", port );


	while ( true )
	{
		SOCKET	acceptedSock;

		switch ( listener.Idle( acceptedSock, 500 ) )
		{
		case DNET::Listener::IRT_CONNECTED:
			listener.Stop();

			printf( "Accepted socket %i\n", acceptedSock );
			serverTask( acceptedSock );

			closesocket( acceptedSock );

			if NOT( listener.Start() )
			{
				printf( "Failed to open port %i\n", port );
				return -1;
			}
			printf( "Listening on port %i for connections..\n", port );
			break;

		case DNET::Listener::IRT_ERROR:
			printf( "Error while listening !\n" );
			break;
		}
	}

    return 0;
}

//===============================================================
static int clientMain( int argc, char **argv )
{
	if ( argc != 3 )
	{
		printUsage( argc, argv );
		return -1;
	}

	const char *pOutExt = DUT::GetFileNameExt( argv[2] );

	if ( 0 != strcasecmp( pOutExt, "jpg" ) &&
		 0 != strcasecmp( pOutExt, "jpeg" ) )
	{
		printf( "Error: output file name must have a 'jpg' or 'jpeg' extension\n" );
		return -1;
	}

	char	defaultResDir[2048];

	DStr	exePath = DUT::GetDirNameFromFPathName( argv[0] );

	if ( exePath.length() )
		sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
	else
		strcpy_s( defaultResDir, "Resources" );

	RenderOutputFile	rendOut( argv[2] );

	renderFile( argv[1], defaultResDir, &rendOut );

    return 0;
}

//===============================================================
int main( int argc, char **argv )
{
	DNET::InitializeSocket();	// bha !

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-server", argv[i] ) )
			return serverMain( argc, argv );
	}

	return clientMain( argc, argv );
}
