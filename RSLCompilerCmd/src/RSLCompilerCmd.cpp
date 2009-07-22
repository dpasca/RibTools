//==================================================================
/// RSLCompilerCmd.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>
#include "DSystem/include/DTypes.h"
#include "DSystem/include/DUtils.h"
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

	DVec<U8>	inData;
	if NOT( DUT::GrabFile( pSLFName, inData ) )
	{
		printf( "ERROR: Failed opening %s\n", pSLFName );
		exit( -1 );
	}

	try 
	{
		RSLCompiler::Params	params;
		params.mDbgOutputTree = true;

		RSLCompiler	compiler(
						(const char *)&inData[0],
						inData.size(),
						"Resources\\Shaders\\RSLC_Builtins.sl",
						params );

		printf( "Generating %s...\n", pRRFName );

		compiler.SaveASM( pRRFName, pSLFName );
	}
	catch ( RSLC::Exception &e )
	{
		printf( "ERROR: %s !\n", e.GetMessage().c_str() );
	}
	catch ( ... )
	{
		printf( "ERROR while compiling !\n" );
	}

	printf( "Done !\n", pRRFName );

	return 0;
}