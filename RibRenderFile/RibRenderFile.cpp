//==================================================================
/// RibRenderFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>
#include <stdexcept>
#include <direct.h>
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
	void	*pData;
	size_t	dataSize;

	if NOT( DUT::GrabFile( pFileName, pData, dataSize ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return false;
	}
	
	char	defaultShadersDir[4096];
	sprintf_s( defaultShadersDir, "%s/Shaders", pDefaultResourcesDir );
	printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::Parser			parser;
	RI::FrameworkREYES	frameworkREYES( pRenderOutput );
	RI::Machine			machine( &frameworkREYES, defaultShadersDir, forcedWd, forcedHe );
	
	for (size_t i=0; i <= dataSize; ++i)
	{
		if ( i == dataSize )
			parser.AddChar( 0 );
		else
			parser.AddChar( ((char *)pData)[i] );

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


#if 0
//==================================================================
static RenderOutputFile	*gpsRenderOutput;
static char				gpFileToRender[2048];

//===============================================================
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( gpsRenderOutput )
		gpsRenderOutput->Blit();

    glutSwapBuffers();
}

//===============================================================
void reshape(int width, int height)
{
    glViewport(0, 0, width, height);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);
	
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//glTranslatef( -0.375f, -0.375f, 0 );

	//gApp.SetSize( width, height );
	
	glEnable( GL_DEPTH_TEST );
	
	// render the last loaded file
	renderFile( NULL, defaultResDir, width, height );
}

//===============================================================
static void passiveMotion( int x, int y )
{
	//gApp.SetCursorPos( x, y );
	//glutPostRedisplay();
}

//===============================================================
void idle(void)
{
	if ( gpFileToRender[0] )
	{
		renderFile( gpFileToRender, defaultResDir );
		gpFileToRender[0] = 0;

		glutPostRedisplay();
	}
	else
	{
		// Sleep because freeglut goes nuts with mainloop
		#ifdef _MSC_VER
		Sleep( 1 );
		#endif
	}
	//glutPostRedisplay();
}

//===============================================================
static char *gsTestRibFiles[] =
{
	"Airplane.rib",
	"Bag.rib",
	"Chair.rib",
	"dragonhead.rib",
	"Elephant.rib",
	"Enterprise_ncc1701d.rib",
	"Pixar.rib",
	"sebtest.rib",
	"SimpleMug.rib",
	"uteapot.rib",
	"WarBird.rib",
	NULL
};

//===============================================================
static void menuFunc( int id )
{
	strcpy( gpFileToRender, startDir );
	strcat( gpFileToRender, "/../../Tests/" );
	strcat( gpFileToRender, gsTestRibFiles[id] );
	
	printf( "Render File: %s\n", gpFileToRender );
}

#endif

//===============================================================

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
