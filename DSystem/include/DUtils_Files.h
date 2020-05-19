//==================================================================
/// DUtils_Files.h
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DUTILS_FILES_H
#define DUTILS_FILES_H

#include "DTypes.h"
#include "DStr.h"
#include "DContainers.h"
#include "DUtils_MemFile.h"
#include "DFileManager.h"

//==================================================================
namespace DUT
{

//==================================================================
/// install any external file manager
void InstallFileManager(DFileManagerBase* pFileManager);
/// install the FILE-based file manager
void InstallFileManagerStd();
/// install the Android file manager
void InstallFileManagerAndroid();

/// FileExists(), GrabFile() and SaveFile() respond to the currently
/// installed file manager (see above).

/// prefs indicates whether the system preferences storage should be
/// used.  On some platforms (android) this is the only system by
/// which persistent data can be loaded and saved.  On other
/// platforms, this may be completely ignored.
bool FileExists( const char *pFileName, bool prefs = false );

bool GrabFile( const char *pFileName, DVec<U8> &out_data, bool prefs=false );

bool SaveFile( const char *pFileName, const U8 *pInData, size_t dataSize,
               bool prefs=false );

bool GrabFile( const char *pFileName, MemReader &reader, bool prefs=false );
bool SaveFile( const char *pFileName, const MemWriterDynamic &mw, bool prefs=false );

inline DStr JoinPath( const DStr &path1, const DStr &path2 )
{
	if NOT( path2.size() ) return path1;
	if NOT( path1.size() ) return path2;

	bool slash1 = (path1[path1.size()-1] == '\\' || path1[path1.size()-1] == '/');
	bool slash2 = (path2[0] == '\\' || path2[0] == '/');

	if ( slash1 && slash2 )
		return path1 + (path2.c_str()+1);
	else
	if ( slash1 || slash2 )
		return path1 + path2;
	else
		return path1 + '/' + path2;
}

inline DStr JoinPath( const char *pPath1, const char *pPath2 )
{
	return JoinPath( DStr( pPath1 ), DStr( pPath2 ) );
}

//==================================================================
DStr GetDirNameFromFPathName( const char *pInFPathname );
DStr GetFullDirNameFromFPathName( const char *pInFPathname );

const char *GetFileNameOnly( const char *pPathFileName );

const char *GetFileNameExt( const char *pPathFileName );
char *GetFileNameExt( char *pPathFileName );

#if !defined(ANDROID)
FILE *BeginGrabFile( const char *pFileName, size_t &out_fileSize ); // OBSOLETE
bool EndGrabFile( FILE *pFile, void *pDest, size_t readSize ); // OBSOLETE
#endif

//==================================================================
}

#endif
