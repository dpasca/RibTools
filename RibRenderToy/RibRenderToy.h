//==================================================================
/// RibRenderToy.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef RIBRENDERTOY_H
#define RIBRENDERTOY_H

#include "RI_System/include/RI_HiderST.h"

#define APPNAME		"RibRenderToy"
#define APPVERSION	"0.9"

//==================================================================
class RibRendToy
{
	static RibRendToy	*mspThis;
	int					mMainMenuID;

public:
	char				mFileToRender[2048];
	char				mExePath[2048];
	char				mExeResPath[2048];
	DVec<DStr>			mTestRibFiles;
	DVec<DStr>			mTestRibFilesPaths;

	RI::Hider::Params	mHiderParams;

	std::string			mLastFileName;
	int					mLastUsedWd;
	int					mLastUsedHe;

	DispDriverFBuffOGL	*mpDispDriverFBuff;

	// used for tricky menu/invalidated zone problem
	int					mPostRedisplayCnt;

	enum
	{
		MENUID_DBG_PICK_BUCKET,
		MENUID_DBG_COLOR_CODE_GRIDS,
		MENUID_DBG_RASTERIZE_VERTS,
		MENUID_FILES = 200
	};

	//===============================================================
	RibRendToy( const char *p );
	~RibRendToy();

	void SetFileToRender( const char *pFName );

	void RebuildMenu();

	void MenuFunc( int id );

	void MouseFunc( int button, int butState, int mx, int my );

	bool RenderFile(
					bool renderLastUsed,
					int forcedWd=-1,
					int forcedHe=-1 );

private:
	static void renderFile_HandleDisplays_s( void *pThis, const RRL::DisplayList &pDisplays )
	{
		((RibRendToy *)pThis)->renderFile_HandleDisplays( pDisplays );
	}
	void renderFile_HandleDisplays( const RRL::DisplayList &pDisplays );

private:
	//===============================================================
	static void sDisplayFunc();
	static void sReshapeFunc(int width, int height);
	static void sMouseFunc( int button, int butState, int mx, int my );
	static void sPassiveMotion( int x, int y );
	static void sIdleFunc();
	static void sMenuFunc( int id );

	void addDirToMenu( const char *pDirName, const char *pFilesExt );

};

#endif
