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
	printf( "Base Dir: %s\n", baseDir.c_str() );
	printf( "Default Shaders Dir: %s\n", defaultShadersDir );

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
	if ( argc != 2 )
	{
		printf( "Invalid param count. Quitting !\n" );
		return -1;
	}

	char	defaultResDir[2048];
	char	startDir[2048];

	_getcwd( startDir, sizeof(startDir) );
	printf( "startDir: %s\n", startDir );

	sprintf_s( defaultResDir, "%s/../../Resources", startDir );
	printf( "defaultResDir: %s\n", startDir );

	char	outName[4096];
	sprintf_s( outName, "%s.jpg", argv[1] );

	RenderOutputFile	rendOut( outName );

	renderFile( argv[1], defaultResDir, &rendOut );

/*
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(512, 256);

    glutCreateWindow( "RibRender" );

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
	glutPassiveMotionFunc( passiveMotion );
    glutIdleFunc(idle);

	glutCreateMenu( menuFunc );

	for (int i=0; gsTestRibFiles[i]; ++i)
		 glutAddMenuEntry( gsTestRibFiles[i], i );

	glutAttachMenu(GLUT_RIGHT_BUTTON);

	strcpy( gpFileToRender, argv[1] );

	glutMainLoop();
*/

    return EXIT_SUCCESS;
}
