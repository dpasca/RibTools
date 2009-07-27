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
#include "RI_System/include/RI_Parser.h"
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
	DVec<U8>	fileData;

	if NOT( DUT::GrabFile( pFileName, fileData ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return false;
	}

	DStr	baseDir = DUT::GetDirNameFromFPathName( pFileName );

	char	defaultShadersDir[4096];
	sprintf_s( defaultShadersDir, "%s/Shaders", pDefaultResourcesDir );
	//printf( "Base Dir: %s\n", baseDir.c_str() );
	//printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::HiderREYES::Params	hiderParams;
	RI::FrameworkREYES		framework( pRenderOutput, hiderParams );
	RI::Machine				machine( &framework, baseDir.c_str(), defaultShadersDir, forcedWd, forcedHe );

	RI::Parser				parser;
	for (size_t i=0; i <= fileData.size(); ++i)
	{
		if ( i == fileData.size() )
			parser.AddChar( 0 );
		else
			parser.AddChar( (char)fileData[i] );

		while ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;
			int				cmdLine;

			parser.FlushNewCommand( &cmdName, &cmdParams, &cmdLine );

			printf( "CMD %s ", cmdName.c_str() );

			if ( cmdParams.size() )
				printf( "(%i params)", cmdParams.size() );

			puts( "" );


			try {
				machine.AddCommand( cmdName, cmdParams );
			} catch ( std::runtime_error ex )
			{
				printf( "ERROR at line: %i\n", cmdLine );
				break;
			}
		}
	}

	return true;
}

//===============================================================
int main(int argc, char** argv)
{
	if ( argc != 3 )
	{
		printf( "Usage: %s <rib file> <output JPEG file>\n", argv[0] );
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

    return EXIT_SUCCESS;
}
