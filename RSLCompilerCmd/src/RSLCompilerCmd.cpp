//==================================================================
/// RSLCompilerCmd.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>
#include "DSystem/include/DTypes.h"
#include "RSLCompilerLib/include/RSLCompiler.h"

//==================================================================
static void printUsage( const char *pCmdName )
{
	printf( "USAGE: %s <Input .sl File> <Output .rrasm File>\n", pCmdName );
}

//==================================================================
int main( int argc, char *argv[] )
{
	if ( argc < 3 )
	{
		printf( "ERROR: Missing parameters !\n" );
		printUsage( argv[0] );
		exit( -1 );
	}

	const char	*pSLFName = argv[1];
	const char	*pRRFName = argv[2];

	printf( "Opening %s in input...\n", pSLFName );

	printf( "Generating %s...\n", pRRFName );

	printf( "Done !\n", pRRFName );

	return 0;
}