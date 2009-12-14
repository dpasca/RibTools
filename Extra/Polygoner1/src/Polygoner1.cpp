/*
 *  Polygoner1.cpp
 *  Polygoner1
 *
 *  Created by Davide Pasca on 09/02/22.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdexcept>
#include <GLUT/glut.h>
#include "Engine1.h"
#include "DU_RenderOutput.h"
#include "Polygoner1.h"

//===============================================================
class App
{
public:
	RendBuff			mRendBuff;
	Container			mContainer;
	DU::RenderOutput	mRendOutput;

	//===============================================================
	void OnDisplay()
	{
		mRendBuff.Clear();

		Matrix44	mtxProjCam =
			Matrix44::PerspectiveRH0(
							M_PI/180.0f * 70.0f,
							(float)mRendBuff.mWd / mRendBuff.mHe,
							0.01f,
							1000.0f );

		Matrix44	mtxCamWorld = Matrix44::Translate( 0, 0, -50.0f );
		Matrix44	mtxProjWorld = mtxCamWorld * mtxProjCam;

		const ObjList	&nodesList = mContainer.mpObjects[ObjBase::OT_NODE];
		for (size_t ni=0; ni < nodesList.size(); ++ni)
		{
			const Node	&node = *(const Node *)nodesList[ni];
			
			for (size_t mi=0; mi < node.mpMeshes.size(); ++mi)
			{
				const Mesh	&mesh = *node.mpMeshes[mi];
				
				RendMesh( mRendBuff, mesh, mtxProjWorld );
			}
		}

		mRendOutput.Update( mRendBuff.mWd, mRendBuff.mHe, mRendBuff.mpData );
		mRendOutput.Blit();
	}
};

//===============================================================
static App	*gpApp;

//===============================================================
void display(void)
{
	//glClearColor( 0.2f, 0.2f, 0.2f, 0 );
    //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	gpApp->OnDisplay();

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
	
	glDisable( GL_DEPTH_TEST );
	
	gpApp->mRendBuff.Resize( width, height, 3 );
}

//===============================================================
static void passiveMotion( int x, int y )
{
}

//===============================================================
void idle(void)
{
    glutPostRedisplay();
}

//===============================================================
static void loadObj( const char *pFileName, Container &container  )
{
	char curPath[1024];
	getcwd( curPath, 1024 );
	
	FILE	*pFile = fopen( pFileName, "r" );
	if NOT( pFile )
		throw std::runtime_error( "Could not open file" );

	Node	*pNode = new Node( pFileName );
	Mesh	*pMesh = new Mesh( pFileName );

	pNode->mpMeshes.push_back( pMesh );
	
	container.AddObject( pNode );
	container.AddObject( pMesh );

	while ( true )
	{
		char	line[2048];
		if NOT( fgets( line, 2048, pFile ) )
			break;

		char *pCommentStart = strstr( line, "#" );
		if ( pCommentStart )
			*pCommentStart = 0;
			
		char *pSave;
		char *pTok = strtok_r( line, " \t", &pSave );
		if NOT( pTok )
			continue;
			
		if ( 0 == strcasecmp( pTok, "v" ) )
		{
			// vertex pos
			float	*p = pMesh->mVerts[Mesh::VT_POS].grow(3);
			for (size_t i=0; i < 3; ++i)
				p[i] = atof( strtok_r( NULL, " \t", &pSave ) );
		}
		else
		if ( 0 == strcasecmp( pTok, "vt" ) )
		{
			// vertex texture
			float	*p = pMesh->mVerts[Mesh::VT_TX0].grow(2);
			for (size_t i=0; i < 2; ++i)
				p[i] = atof( strtok_r( NULL, " \t", &pSave ) );
		}
		else
		if ( 0 == strcasecmp( pTok, "vn" ) )
		{
			// vertex normal
			float	*p = pMesh->mVerts[Mesh::VT_NOR].grow(3);
			for (size_t i=0; i < 3; ++i)
				p[i] = atof( strtok_r( NULL, " \t", &pSave ) );
		}
		else
		if ( 0 == strcasecmp( pTok, "f" ) )
		{
			// face
		}
		
	}
	
	fclose( pFile );
}

//===============================================================
int main(int argc, char** argv)
{
    glutInit(&argc, argv);

    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE );
    glutInitWindowSize( 640, 480 );
    
    glutCreateWindow( "Polygoner1" );

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
	glutPassiveMotionFunc( passiveMotion );
    glutIdleFunc( idle );
/*	
	glutCreateMenu( menuFunc );

	for (int i=0; gpApp->mTestRibFiles[i]; ++i)
		 glutAddMenuEntry( gpApp->mTestRibFiles[i], i );
	
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	strcpy( gpFileToRender, argv[1] );
*/

	App		app;
	gpApp = &app;


	loadObj( "cessna.obj", gpApp->mContainer );

	glutMainLoop();

    return 0;
}
