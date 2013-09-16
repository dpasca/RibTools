//==================================================================
/// RSLCompilerCmd.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdio.h>
#include <stdlib.h>
#include "DSystem/include/DUtils_Files.h"
#include "RibToolsBase/include/RibToolsBase.h"
#include "RSLCompilerLib/include/RSLCompiler.h"
#include "RSLCompilerLib/include/RSLC_Prepro.h"

#define APPNAME		"RSLCompilerCmd"
#define APPVERSION	"0.5"

//==================================================================
struct CmdParams
{
	const char				*pInFileName;
	const char				*pOutFileName;
	bool					optPrepro;

	CmdParams() :
		pInFileName		(NULL),
		pOutFileName	(NULL),
		optPrepro		(false)
	{
	}
};

//==================================================================
static void printUsage( int argc, char **argv )
{
	printf( "\n==== "APPNAME" v"APPVERSION" -- (" __DATE__ " - " __TIME__ ") ====\n" );

	printf( "\n%s <Input .sl File> <Output .rrasm File>\n", argv[0] );
	printf( "\n%s -prepro <Input .sl File>\n", argv[0] );

	printf( "\nOptions:\n" );
	printf( "    -help | --help | -h     -- Show this help\n" );
	printf( "    -prepro                 -- Apply the C-preprocessor and give to stdout\n" );
}

//==================================================================
static bool getCmdParams( int argc, char **argv, CmdParams &out_cmdPars )
{
	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-prepro", argv[i] ) )
		{
			out_cmdPars.optPrepro = true;
		}
		else
		if (0 == strcasecmp( "-help", argv[i] ) ||
			0 == strcasecmp( "--help", argv[i] ) ||
			0 == strcasecmp( "-h", argv[i] ) )
		{
			printUsage( argc, argv );
			exit( 0 );
			return false;	// not needed really
		}
		else
		{
			if ( out_cmdPars.pInFileName == NULL )
				out_cmdPars.pInFileName = argv[i];
			else
			if ( out_cmdPars.pOutFileName == NULL )
				out_cmdPars.pOutFileName = argv[i];
			else
			{
				printf( "What is '%s' ?\n", argv[i] );
				printUsage( argc, argv );
				exit( 0 );
				return false;	// not needed really
			}
		}
	}

	if ( out_cmdPars.pInFileName == NULL ||
		 (out_cmdPars.pOutFileName == NULL && !out_cmdPars.optPrepro) )
		return false;

	return true;
}

//==================================================================
static void handlePreproOnly( const char *pSLFName, const char *pBuiltinPathFName )
{
	DIO::FileManagerDisk	fmanager;

	DVec<U8>	inData;
	if NOT( DUT::GrabFile( pSLFName, inData ) )
	{
		printf( "ERROR: Failed opening %s\n", pSLFName );
		exit( -1 );
	}

	try
	{
		RSLC::FatBase		fatBase;
		DVec<RSLC::Fat8>	source;
		fatBase.AppendNewFile( source, pSLFName, (const U8 *)&inData[0], inData.size() );
		DVec<RSLC::Fat8>	processedSource;

		DStr	curShaderDir = DUT::GetDirNameFromFPathName( pSLFName );

		RSLC::PREPRO::Prepro
						prepro(
							fmanager,
							fatBase,
							source,
							pBuiltinPathFName,
							curShaderDir.c_str(),
							processedSource );

		for (size_t i=0; i < processedSource.size(); ++i)
		{
			putchar( processedSource[i].Ch );
		}

		puts( "" );
	}
	catch (...)
	{
		exit(-1);
	}
}

//==================================================================
int main( int argc, char *argv[] )
{
#if 0
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |
					_CRTDBG_DELAY_FREE_MEM_DF |
					_CRTDBG_CHECK_ALWAYS_DF |
					_CRTDBG_LEAK_CHECK_DF|
					_CRTDBG_CHECK_CRT_DF );
#endif

    DUT::InstallFileManagerStd();

	CmdParams	params;

	if NOT( getCmdParams( argc, argv, params ) )
	{
		printUsage( argc, argv );
		return -1;
	}

	char	defaultResDir[2048];
	char	builtinPathFName[4096];
	DStr	exePath = RTB::FindRibToolsDir( argv );
	if ( exePath.length() )
		sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
	else
		strcpy_s( defaultResDir, "Resources" );
	sprintf_s( builtinPathFName, "%s/Shaders/RSLC_Builtins.sl", defaultResDir );


	if ( params.optPrepro )
	{
		handlePreproOnly( params.pInFileName, builtinPathFName );
		return 0;
	}

	const char	*pSLFName = params.pInFileName;
	const char	*pRRFName = params.pOutFileName;

	printf( "Opening %s in input...\n", pSLFName );

	DVec<U8>	inData;
	if NOT( DUT::GrabFile( pSLFName, inData ) )
	{
		printf( "ERROR: Failed opening %s\n", pSLFName );
		return -1;
	}

	try
	{
		DIO::FileManagerDisk	fmanager;

		RSLCompiler::Params	params;
		params.mDbgOutputTree = true;
		params.mpFileManager = &fmanager;

		RSLCompiler	compiler(
						pSLFName,
						(const char *)&inData[0],
						inData.size(),
						builtinPathFName,
						params );

		printf( "Generating %s...\n", pRRFName );

		compiler.SaveASM( pRRFName, pSLFName );
	}
	catch ( RSLC::Exception &e )
	{
		printf( "%s\n", e.GetMessage().c_str() );
	}
	catch ( ... )
	{
		printf( "ERROR while compiling !\n" );
		return -1;
	}

	printf( "Done !\n", pRRFName );

	return 0;
}
