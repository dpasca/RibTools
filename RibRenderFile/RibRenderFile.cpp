//==================================================================
/// RibRenderFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibRenderFile.h"

//==================================================================
void PrintUsage( int argc, char **argv )
{

	printf( "Usage:\n" );
	
	printf( "\t%s clientmode <rib file> <output JPEG file> [Client Options]\n", argv[0] );
	printf( "\t%s servermode [Server Options]\n", argv[0] );
	printf( "\t%s [-help | --help | -h]\n", argv[0] );

	printf( "\nClient Options:\n" );
	printf( "\t-server <address>:<port>\n" );
	printf( "\t-forcedlongdim <size in pixels>\n" );

	printf( "\nServer Options:\n" );
	printf( "\t-port <port>\n" );

	printf( "\nExamples:\n" );
	printf( "\t%s clientmode TestScenes/Airplane.rib airplane.jpg\n", argv[0] );
	printf( "\t%s clientmode TestScenes/Airplane.rib airplane.jpg -longdimsize 1024 -server 192.168.1.107 -server 192.168.1.108\n", argv[0] );
	printf( "\t%s servermode\n", argv[0] );
	printf( "\t%s servermode -port 31111\n", argv[0] );
}

//==================================================================
int main( int argc, char **argv )
{
#if 0
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |
					_CRTDBG_DELAY_FREE_MEM_DF |
					_CRTDBG_CHECK_ALWAYS_DF |
					_CRTDBG_LEAK_CHECK_DF|
					_CRTDBG_CHECK_CRT_DF );
#endif

	DNET::InitializeSocket();	// bha !

	// enough params ?
	if ( argc < 2 )
	{
		PrintUsage( argc, argv );
		return 0;
	}

	// looking for help ?
	for (int i=1; i < argc; ++i)
	{
		if (
			0 == strcasecmp( "-help", argv[i] ) ||
			0 == strcasecmp( "--help", argv[i] ) ||
			0 == strcasecmp( "-h", argv[i] )
			)
		{
			PrintUsage( argc, argv );
			return 0;
		}
	}

	// which mode ?
	if ( 0 == strcasecmp( "clientmode", argv[1] ) )
	{
		return ClientMain( argc, argv );
	}
	else
	if ( 0 == strcasecmp( "servermode", argv[1] ) )
	{
		return ServerMain( argc, argv );
	}
	else
	{
		PrintUsage( argc, argv );
		return 0;
	}
}
