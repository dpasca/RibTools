/*
 *  RibRender.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "RibRender.h"

#include "RI_Base.h"
#include "RI_Machine.h"

#include <GLUT/glut.h>

//===============================================================
void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	//gApp.OnPaint();

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
    glutPostRedisplay();
}
/*
//===============================================================
static int stricmp( const char *a, const char *b )
{
	size_t	len = strlen(a);

	return strncasecmp( a, b, len );
}

//===============================================================
static size_t subStrings( char *pBuff, const char *pOutList[32] )
{
	size_t	cnt = 0;
	char	*pTrack = NULL;
	
	for (;;)
	{
		strtok_r( pBuff, " ¥t/", &pTrack );
		if ( pTrack == NULL )
			break;

		if ( cnt >= 32 )
			throw "Out of bounds !";

		pOutList[cnt++] = pTrack;

		++cnt;
	}
	
	return cnt;
}
*/

//===============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    
    glutCreateWindow( "RibRender" );
    
    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
	glutPassiveMotionFunc( passiveMotion );

    glutIdleFunc(idle);
    
    glutMainLoop();
    return EXIT_SUCCESS;
}
