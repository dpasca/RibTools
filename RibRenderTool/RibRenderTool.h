//==================================================================
/// RibRenderTool.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef RIBRENDERTOOL_H
#define RIBRENDERTOOL_H

#include "RI_System/include/RI_HiderREYES.h"

//==================================================================
class RibRendTool
{
	static RibRendTool	*mspThis;
	int					mMainMenuID;

public:
	char				mFileToRender[2048];
	char				mExePath[2048];
	char				mExeResPath[2048];
	DVec<DStr>			mTestRibFiles;
	DVec<DStr>			mTestRibFilesPaths;

	RI::Hider::Params	mREYESParams;

	std::string			mLastFileName;
	int					mLastUsedWd;
	int					mLastUsedHe;

	RenderOutputOpenGL	*mpRenderOutput;

	enum
	{
		MENUID_DBG_PICK_BUCKET,
		MENUID_DBG_COLOR_CODE_GRIDS,
		MENUID_FILES = 200
	};

	//===============================================================
	RibRendTool( const char *p );
	~RibRendTool();

	void SetFileToRender( const char *pFName );

	void RebuildMenu();

	void MenuFunc( int id );

	void MouseFunc( int button, int butState, int mx, int my );

	bool RenderFile(
					bool renderLastUsed,
					int forcedWd=-1,
					int forcedHe=-1 );

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
