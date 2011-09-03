//==================================================================
/// DispWindows.cpp
///
/// Created by Davide Pasca - 2010/1/28
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DispWindows.h"
#include <GL/glut.h>

//==================================================================
DVec<DispWindows::Window>	DispWindows::msWindows;

//==================================================================
/// DispWindows
//==================================================================
DispWindows::DispWindows()
{

}

//==================================================================
DispWindows::~DispWindows()
{
	for (size_t i=0; i < msWindows.size(); ++i)
	{
		DSAFE_DELETE( msWindows[i].mpDispDriver );
	}
}

//==================================================================
void DispWindows::AddWindow( const RI::Options::Display &disp )
{
	// first window ?
	if ( msWindows.size() == 0 )
	{
		glutInit( &mArgc, mppArgv );
	    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
	}

	msWindows.grow();

	glutInitWindowSize( disp.mImage.mWd, disp.mImage.mHe );

	DStr	winName = mWindowBaseName + " -- " + disp.mName;
	msWindows.back().mWinId = glutCreateWindow( winName.c_str() );

	glutDisplayFunc( sDisplayFunc );
	glutReshapeFunc( sReshapeFunc );
	//glutMouseFunc( sMouseFunc );
	//glutPassiveMotionFunc( sPassiveMotion );
	//glutIdleFunc( sIdleFunc );

	msWindows.back().mpDispDriver =
		DNEW DispDriverFBuffOGL( disp.mName.c_str(), disp.mImage );

	msWindows.back().mpDispDriver->Blit();
}

//==================================================================
void DispWindows::sDisplayFunc()
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	int	curWinId = glutGetWindow();

	for (size_t i=0; i < msWindows.size(); ++i)
	{
		if ( msWindows[i].mWinId == curWinId )
		{
			if ( msWindows[i].mpDispDriver )
				msWindows[i].mpDispDriver->Blit();

			break;
		}
	}

	glutSwapBuffers();
}

//==================================================================
void DispWindows::sReshapeFunc( int width, int height )
{
	glViewport(0, 0, width, height);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glEnable( GL_DEPTH_TEST );

	glutPostRedisplay();
}

//==================================================================
void DispWindows::MainLoop()
{
	glutMainLoop();
}