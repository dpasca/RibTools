//==================================================================
/// RibRenderToy.cpp
///
/// Created by Davide Pasca - 2009/4/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdio.h>
#include <stdexcept>
#include "RibToolsBase/include/RibToolsBase.h"
#include "RibRenderLib/include/RibRenderLib.h"
#include "DSystem/include/DUtils.h"
#include "DSystem/include/DUtils_Files.h"
#include "RibRenderToy.h"

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

//==================================================================
RibRendToy	*RibRendToy::mspThis;

//==================================================================
RibRendToy::RibRendToy( const char *pExePath ) :
	mpDispDriverFBuff(NULL),
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

    glutCreateWindow( APPNAME " v" APPVERSION );

    glutDisplayFunc( sDisplayFunc );
    glutReshapeFunc( sReshapeFunc );
	glutMouseFunc( sMouseFunc );
	glutPassiveMotionFunc( sPassiveMotion );
    glutIdleFunc( sIdleFunc );

	RebuildMenu();
}

//==================================================================
RibRendToy::~RibRendToy()
{
	DSAFE_DELETE( mpDispDriverFBuff );
}

//==================================================================
void RibRendToy::SetFileToRender( const char *pFName )
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
void RibRendToy::addDirToMenu( const char *pDirName, const char *pFilesExt )
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
				MENUID_FILES + (int)mTestRibFiles.size()-1 );

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
void RibRendToy::RebuildMenu()
{
	if ( mMainMenuID != -1 )
		glutDestroyMenu( mMainMenuID );

	mMainMenuID = glutCreateMenu( sMenuFunc );
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glutAddMenuEntry( "-- Debug --", -1 );
	addBoolMenuItem( "Pick Bucket Mode", mHiderParams.mDbgShowBuckets, MENUID_DBG_PICK_BUCKET );
	addBoolMenuItem( "Color Coded Grids", mHiderParams.mDbgColorCodedGrids, MENUID_DBG_COLOR_CODE_GRIDS );
	addBoolMenuItem( "Rasterize Vertices", mHiderParams.mDbgRasterizeVerts, MENUID_DBG_RASTERIZE_VERTS );
	glutAddMenuEntry( "", -1 );

	glutAddMenuEntry( "-- Test Files --", -1 );

	mTestRibFiles.clear();
	mTestRibFilesPaths.clear();

	addDirToMenu( "TestScenes", "rib" );
	addDirToMenu( "TestScenes/KillerooNURBS", "rib" );
	addDirToMenu( "TestScenes/Sponza", "rib" );
	addDirToMenu( "TestScenes/LightsTests", "rib" );
	addDirToMenu( "TestScenes/MiscTests", "rib" );
	addDirToMenu( "TestScenes/ValuesTests", "rib" );
}

//==================================================================
void RibRendToy::MenuFunc( int id )
{
	switch ( id )
	{
	case MENUID_DBG_PICK_BUCKET:
		mHiderParams.mDbgShowBuckets = !mHiderParams.mDbgShowBuckets;
		RenderFile( true );
		RebuildMenu();
		glutPostRedisplay();
		return;

	case MENUID_DBG_COLOR_CODE_GRIDS:
		mHiderParams.mDbgColorCodedGrids = !mHiderParams.mDbgColorCodedGrids;
		RenderFile( true );
		RebuildMenu();
		glutPostRedisplay();
		return;

	case MENUID_DBG_RASTERIZE_VERTS:
		mHiderParams.mDbgRasterizeVerts = !mHiderParams.mDbgRasterizeVerts;
		RenderFile( true );
		RebuildMenu();
		glutPostRedisplay();
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
bool RibRendToy::RenderFile( bool renderLastUsed, int forcedWd/*=-1*/, int forcedHe/*=-1 */ )
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

	char	defaultShadersDir[4096];
	sprintf( defaultShadersDir, "%s/Shaders", mExeResPath );
	printf( "Base Dir: %s\n", baseDir.c_str() );
	printf( "Default Shaders Dir: %s\n", defaultShadersDir );

	RI::Framework::Params	fwParams;
	fwParams.mFallBackFBuffDisplay		= true;
	fwParams.mpHiderParams				= &mHiderParams;
	RI::Framework	framework( fwParams );

	DIO::FileManagerDisk	fileManager;

	RRL::Render::Params	params;
	
	params.mTrans.mState.mpFramework			= &framework;
	params.mTrans.mState.mpFileManager			= &fileManager;
	params.mTrans.mState.mBaseDir				= baseDir;
	params.mTrans.mState.mDefaultShadersDir		= defaultShadersDir;

	if ( mHiderParams.mDbgColorCodedGrids )
		params.mTrans.mState.mForcedSurfaceShader = "constant";

	params.mTrans.mForcedWd		= forcedWd;
	params.mTrans.mForcedHe		= forcedHe;

	params.mpFileName			= pFileName;

	params.mpOnFrameEndCB		= renderFile_HandleDisplays_s;
	params.mpOnFrameEndCBData	= this;

	try
	{
		RRL::Render	render( params );
	}
	catch ( std::bad_alloc )
	{
		printf( "Out of Memory !!!\n" );
		return false;
	}
	catch ( RI::Exception &e )
	{
		printf( "%s\nAborting.\n", e.GetMessage_().c_str() );
		return false;
	}
	catch ( ... )
	{
		printf( "Unknown exception. Aborting.\n" );
		return false;
	}

	return true;
}

//==================================================================
void RibRendToy::renderFile_HandleDisplays( const RRL::DisplayList &pDisplays )
{
	// be happy with the first display found
	for (size_t i=0; i < pDisplays.size(); ++i)
	{
		DSAFE_DELETE( mpDispDriverFBuff );

		mpDispDriverFBuff =
			DNEW DispDriverFBuffOGL( "Default Disp Driver", pDisplays[i]->mImage );

		mLastUsedWd = (int)pDisplays[i]->mImage.mWd;
		mLastUsedHe = (int)pDisplays[i]->mImage.mHe;

		// actually resize the window !
		glutReshapeWindow( mLastUsedWd, mLastUsedHe );
		break;
	}
}

//===============================================================
void RibRendToy::sDisplayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if ( mspThis->mpDispDriverFBuff )
		mspThis->mpDispDriverFBuff->Blit();

	glutSwapBuffers();
}

//===============================================================
void RibRendToy::sReshapeFunc( int width, int height )
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
void RibRendToy::sMouseFunc( int button, int butState, int mx, int my )
{
	mspThis->MouseFunc( button, butState, mx, my );
}

//==================================================================
void RibRendToy::MouseFunc( int button, int butState, int mx, int my )
{
	if ( button == 0 )
	{
		if ( butState == GLUT_DOWN )
		{
			if ( mHiderParams.mDbgShowBuckets )
			{
				mHiderParams.mDbgOnlyBucketAtX = mx;
				mHiderParams.mDbgOnlyBucketAtY = my;
				RenderFile( true );
				glutPostRedisplay();
				mHiderParams.mDbgOnlyBucketAtX = -1;
				mHiderParams.mDbgOnlyBucketAtY = -1;
			}
		}
	}
}

//===============================================================
void RibRendToy::sPassiveMotion( int x, int y )
{
	//mspThis->SetCursorPos( x, y );
	//glutPostRedisplay();
}

//===============================================================
void RibRendToy::sIdleFunc()
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
void RibRendToy::sMenuFunc( int id )
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

    DUT::InstallFileManagerStd();

    glutInit( &argc, argv );

	DStr	exePath = RTB::FindRibToolsDir( argv );

	RibRendToy	toy( exePath.c_str() );

	// set optional initial file name
	if ( argc >= 2 )
		toy.SetFileToRender( argv[1] );

	glutMainLoop();

    return EXIT_SUCCESS;
}
