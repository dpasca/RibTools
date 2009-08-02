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
	printf( "\t%s <rib file> <output JPEG file>\n", argv[0] );
	printf( "\t%s <rib file> <output JPEG file> [ -server <address>:<port> ]\n", argv[0] );
	printf( "\t%s -servermode [-port <port>]\n", argv[0] );
}

//==================================================================
int main( int argc, char **argv )
{
	DNET::InitializeSocket();	// bha !

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-servermode", argv[i] ) )
			return ServerMain( argc, argv );
	}

	return ClientMain( argc, argv );
}
