/*
 *  RibRender.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdexcept>
#include "DUtils.h"
#include "RI_Parser.h"
#include "RI_Machine.h"
#include "RI_FrameworkREYES.h"
#include "RenderOutputOpenGL.h"
#include "RibRender.h"

#include <GLUT/glut.h>

//#define ECHO_INPUT

//==================================================================
static RenderOutputOpenGL	*gpsRenderOutput;
static char					gpFileToRender[2048];
static char					gDefaultResDir[2048];
static char					gStartDir[2048];

//===============================================================
static bool renderFile(
				const char *pFileName,
				const char *pDefaultResourcesDir,
				int forcedWd=-1,
				int forcedHe=-1 )
{
	static std::string	sLastFileName;
	static int			sLastUsedWd;
	static int			sLastUsedHe;
	
	// remember the last used file name
	if ( pFileName )
		sLastFileName = pFileName;
	else
	{
		if ( sLastFileName.length() )
		{
			pFileName = sLastFileName.c_str();
			if ( forcedWd == sLastUsedWd &&
				 forcedHe == sLastUsedHe )
			{
				return false;
			}
		}
		else
			return false;
	}

	void	*pData;
	size_t	dataSize;

	if NOT( DUT::GrabFile( pFileName, pData, dataSize ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return false;
	}
	
	gpsRenderOutput = new RenderOutputOpenGL();

	char	defaultShadersDir[4096];
	sprintf( defaultShadersDir, "%s/Shaders", pDefaultResourcesDir );
	printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::Parser			parser;
	RI::FrameworkREYES	frameworkREYES( gpsRenderOutput );
	RI::Machine			machine( &frameworkREYES, defaultShadersDir, forcedWd, forcedHe );
	
	for (size_t i=0; i <= dataSize; ++i)
	{
		if ( i == dataSize )
			parser.AddChar( 0 );
		else
			parser.AddChar( ((char *)pData)[i] );

#if defined(ECHO_INPUT)
		printf( "%c", ((char *)pData)[i] );
#endif

		while ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;
			int				cmdLine;

			parser.FlushNewCommand( &cmdName, &cmdParams, &cmdLine );
			
//			printf( "%3i - %s\n", cmdLine, cmdName.c_str() );

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

	sLastUsedWd = (int)gpsRenderOutput->GetCurWd();
	sLastUsedHe = (int)gpsRenderOutput->GetCurHe();

	return true;
}

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
	renderFile( NULL, gDefaultResDir, width, height );
}

//===============================================================
static void passiveMotion( int x, int y )
{
	//gApp.SetCursorPos( x, y );
	glutPostRedisplay();
}

//===============================================================
void idle(void)
{
	if ( gpFileToRender[0] )
	{
		renderFile( gpFileToRender, gDefaultResDir );
		gpFileToRender[0] = 0;
	}

    glutPostRedisplay();
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
	"SimpleMug.rib",
	"uteapot.rib",
	"WarBird.rib",
	NULL
};

//===============================================================
static void menuFunc( int id )
{
	strcpy( gpFileToRender, gStartDir );
	strcat( gpFileToRender, "/../../Tests/" );
	strcat( gpFileToRender, gsTestRibFiles[id] );
	
	printf( "Render File: %s\n", gpFileToRender );
}

//===============================================================
int main(int argc, char** argv)
{
	if ( argc != 2 )
	{
		printf( "Invalid param count. Quitting !\n" );
		return -1;
	}

	getcwd( gStartDir, sizeof(gStartDir) );
	printf( "gStartDir: %s\n", gStartDir );

	sprintf( gDefaultResDir, "%s/../../RibRender/Resources", gStartDir );
	printf( "gDefaultResDir: %s\n", gStartDir );

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

    return EXIT_SUCCESS;
}
