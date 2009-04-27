//==================================================================
/// RibRenderTool.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERTOOL_H
#define RIBRENDERTOOL_H

//==================================================================
class RibRendTool
{
	static RibRendTool	*mspThis;
	int					mMainMenuID;

public:
	char				mFileToRender[2048];
	char				mDefaultResDir[2048];
	char				mStartDir[2048];
	DVec<DStr>			mTestRibFiles;

	bool				mDbgPickBucket;
	int					mDbgDrawOnlyBucketAtX;
	int					mDbgDrawOnlyBucketAtY;

	std::string			mLastFileName;
	int					mLastUsedWd;
	int					mLastUsedHe;

	RenderOutputOpenGL	*mpRenderOutput;

	enum
	{
		MENUID_DBGMARKBUCKETS,
		MENUID_FILES = 200
	};

	//===============================================================
	RibRendTool();
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

};

#endif
