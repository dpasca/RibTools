//==================================================================
/// RibRenderTool.cpp
///
/// Created by Davide Pasca - 2009/4/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdio.h>
#include <stdexcept>
#include "RibRenderLib/include/RibRenderLib.h"

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DUtils_Files.h"
#include "RenderOutputOpenGL.h"
#include "RibRenderTool.h"

#ifdef _MSC_VER
	#include <GL/glut.h>
	#include <direct.h>
	#include <io.h>

#elif defined(__linux__)
	#include <GL/glut.h>
	#include <sys/types.h>
	#include <dirent.h>

#else
	#include <GLUT/glut.h>

#endif

//#define ECHO_INPUT

//==================================================================
RibRendTool	*RibRendTool::mspThis;

/*
//==================================================================
char *RibRendTool::msTestRibFiles[] =
{
	"Airplane.rib",
	"Bag.rib",
	"Chair.rib",
	"dragonhead.rib",
	"Elephant.rib",
	"Enterprise_ncc1701d.rib",
	"Pixar.rib",
	//"sebtest.rib",
	"SimpleMug.rib",
	"uteapot.rib",
	//"WarBird.rib",
	NULL
};
*/

//==================================================================
RibRendTool::RibRendTool( const char *pExePath ) :
	mpRenderOutput(NULL),
	mLastUsedWd(0),
	mLastUsedHe(0),
	mMainMenuID(-1)
{
	DASSERT( mspThis == NULL );

	mspThis = this;

	mFileToRender[0] = 0;

	strcpy_s( mExePath, pExePath );

	sprintf( mExeResPath, "%s/Resources", mExePath );

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize( 640, 480 );

    glutCreateWindow( "RibRender" );

    glutDisplayFunc( sDisplayFunc );
    glutReshapeFunc( sReshapeFunc );
	glutMouseFunc( sMouseFunc );
	glutPassiveMotionFunc( sPassiveMotion );
    glutIdleFunc( sIdleFunc );

	RebuildMenu();
}

//==================================================================
RibRendTool::~RibRendTool()
{
	DSAFE_DELETE( mpRenderOutput );
}

//==================================================================
void RibRendTool::SetFileToRender( const char *pFName )
{
	strcpy( mFileToRender, pFName );
}

//===============================================================
static void addBoolMenuItem( const char *pName, bool onoff, int id )
{
	char	buff[1024];
	sprintf_s( buff, "[%c] %s", onoff ? '*' : ' ', pName );
	glutAddMenuEntry( buff, id );
}

//==================================================================
void RibRendTool::addDirToMenu( const char *pDirName, const char *pFilesExt )
{
#if defined(WIN32)

	DStr	buff = DUT::SSPrintFS( "%s/*.%s", pDirName, pFilesExt );

	_finddatai64_t	findData;
	intptr_t	handle = _findfirst64( buff.c_str(), &findData );
	if ( handle != -1 )
	{
		int	ret = 0;
		do
		{
			mTestRibFiles.push_back( findData.name );
			mTestRibFilesPaths.push_back( pDirName );

			glutAddMenuEntry(
				DUT::SSPrintFS( "%s / %s", pDirName, findData.name ).c_str(),
				MENUID_FILES + mTestRibFiles.size()-1 );

			ret = _findnext64( handle, &findData );
		} while ( ret == 0 );

		_findclose( handle );
	}

#elif defined(__linux__)

	DIR	*pDir = opendir( pDirName );
	if ( pDir )
	{
		struct dirent *pDirent;

		while ( pDirent = readdir( pDir ) )
		{
			//pDirent->
		}

		closedir( pDir );
	}

#endif

}

//==================================================================
void RibRendTool::RebuildMenu()
{
	if ( mMainMenuID != -1 )
		glutDestroyMenu( mMainMenuID );

	mMainMenuID = glutCreateMenu( sMenuFunc );
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutAddMenuEntry( "-- Debug --", -1 );
	addBoolMenuItem( "Pick Bucket Mode", mREYESParams.mDbgShowBuckets, MENUID_DBG_PICK_BUCKET );
	addBoolMenuItem( "Color Coded Grids", mREYESParams.mDbgColorCodedGrids, MENUID_DBG_COLOR_CODE_GRIDS );
	glutAddMenuEntry( "", -1 );

	glutAddMenuEntry( "-- Test Files --", -1 );

	mTestRibFiles.clear();
	mTestRibFilesPaths.clear();

	addDirToMenu( "TestScenes", "rib" );
	addDirToMenu( "TestScenes/Sponza", "rib" );
	addDirToMenu( "TestScenes/LightsTests", "rib" );
	addDirToMenu( "TestScenes/ValuesTests", "rib" );
}

//==================================================================
void RibRendTool::MenuFunc( int id )
{
	switch ( id )
	{
	case MENUID_DBG_PICK_BUCKET:
		mREYESParams.mDbgShowBuckets = !mREYESParams.mDbgShowBuckets;
		RenderFile( true );
		glutPostRedisplay();
		RebuildMenu();
		return;

	case MENUID_DBG_COLOR_CODE_GRIDS:
		mREYESParams.mDbgColorCodedGrids = !mREYESParams.mDbgColorCodedGrids;
		RenderFile( true );
		glutPostRedisplay();
		RebuildMenu();
		return;
	}

	if ( id >= MENUID_FILES )
	{
		strcpy_s( mFileToRender, mExePath );
		strcat_s( mFileToRender, "/" );
		strcat_s( mFileToRender, mTestRibFilesPaths[id - MENUID_FILES].c_str() );
		strcat_s( mFileToRender, "/" );
		strcat_s( mFileToRender, mTestRibFiles[id - MENUID_FILES].c_str() );

		printf( "Render File: %s\n", mFileToRender );
	}
}

//==================================================================
bool RibRendTool::RenderFile( bool renderLastUsed, int forcedWd/*=-1*/, int forcedHe/*=-1 */ )
{
	const char *pFileName;

	if ( renderLastUsed )
	{
		if ( mLastFileName.length() )
		{
			pFileName = mLastFileName.c_str();
			if ( forcedWd == mLastUsedWd &&
				forcedHe == mLastUsedHe )
			{
				return false;
			}

			if ( forcedWd == -1 )
			{
				forcedWd = mLastUsedWd;
				forcedHe = mLastUsedHe;
			}
		}
		else
			return false;
	}
	else
	{
		// remember the last used file name
		mLastFileName = mFileToRender;
		pFileName = mFileToRender;
	}

	if NOT( pFileName[0] )
		return false;

	DStr	baseDir = DUT::GetDirNameFromFPathName( pFileName );

	DSAFE_DELETE( mpRenderOutput );
	mpRenderOutput = DNEW RenderOutputOpenGL();

	char	defaultShadersDir[4096];
	sprintf( defaultShadersDir, "%s/Shaders", mExeResPath );
	printf( "Base Dir: %s\n", baseDir.c_str() );
	printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::Framework	framework( mpRenderOutput, NULL, mREYESParams );
	RI::FileManagerDisk	fileManager;

	RI::Machine::Params	params;
	params.mState.mpFramework			= &framework;
	params.mState.mpFileManager			= &fileManager;
	params.mState.mBaseDir				= baseDir;
	params.mState.mDefaultShadersDir	= defaultShadersDir;

	if ( mREYESParams.mDbgColorCodedGrids )
		params.mState.mForcedSurfaceShader = "constant";

	params.mForcedWd					= forcedWd;
	params.mForcedHe					= forcedHe;

	RI::Machine			machine( params );

	try
	{
		RRL::Render	render( pFileName, machine, fileManager );
	}
	catch ( std::bad_alloc )
	{
		printf( "Out of Memory !!!\n" );
		return false;
	}
	catch ( ... )
	{
		return false;
	}

	mLastUsedWd = (int)mpRenderOutput->GetCurWd();
	mLastUsedHe = (int)mpRenderOutput->GetCurHe();

	return true;
}

//===============================================================
void RibRendTool::sDisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( mspThis->mpRenderOutput )
		mspThis->mpRenderOutput->Blit();

	glutSwapBuffers();
}

//===============================================================
void RibRendTool::sReshapeFunc( int width, int height )
{
	glViewport(0, 0, width, height);

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D(0, width, height, 0);

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//glTranslatef( -0.375f, -0.375f, 0 );

	//mspThis->SetSize( width, height );

	glEnable( GL_DEPTH_TEST );

	// render the last loaded file
	mspThis->RenderFile( true, width, height );
}

//===============================================================
void RibRendTool::sMouseFunc( int button, int butState, int mx, int my )
{
	mspThis->MouseFunc( button, butState, mx, my );
}

//==================================================================
void RibRendTool::MouseFunc( int button, int butState, int mx, int my )
{
	if ( button == 0 )
	{
		if ( butState == GLUT_DOWN )
		{
			if ( mREYESParams.mDbgShowBuckets )
			{
				mREYESParams.mDbgOnlyBucketAtX = mx;
				mREYESParams.mDbgOnlyBucketAtY = my;
				RenderFile( true );
				glutPostRedisplay();
				mREYESParams.mDbgOnlyBucketAtX = -1;
				mREYESParams.mDbgOnlyBucketAtY = -1;
			}
		}
	}
}

//===============================================================
void RibRendTool::sPassiveMotion( int x, int y )
{
	//mspThis->SetCursorPos( x, y );
	//glutPostRedisplay();
}

//===============================================================
void RibRendTool::sIdleFunc()
{
	if ( mspThis->mFileToRender[0] )
	{
		mspThis->RenderFile( false );

		// it has been rendered...
		mspThis->mFileToRender[0] = 0;

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

//==================================================================
void RibRendTool::sMenuFunc( int id )
{
	mspThis->MenuFunc( id );
}

//==================================================================
/// main
//==================================================================
int main(int argc, char** argv)
{

#if 0
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |
					_CRTDBG_DELAY_FREE_MEM_DF |
					_CRTDBG_CHECK_ALWAYS_DF |
					_CRTDBG_LEAK_CHECK_DF );
#endif

    glutInit(&argc, argv);


	DStr	exePath = DUT::GetDirNameFromFPathName( argv[0] );

	RibRendTool	tool( exePath.c_str() );

	// set optional initial file name
	if ( argc >= 2 )
		tool.SetFileToRender( argv[1] );

	glutMainLoop();

    return EXIT_SUCCESS;
}
