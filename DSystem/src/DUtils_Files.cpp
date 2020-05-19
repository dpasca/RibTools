//==================================================================
/// DUtils_Files.cpp
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "dlog.h"
#include "DUtils_Files.h"
#include "DFileManager.h"

#if defined(ANDROID)
# include "native.h"
# include "DUtils_FileManagerAndroid.h"
#else
# include "DUtils_FileManagerStd.h"
#endif

//==================================================================
static DFileManagerBase	*_spFManager;

//==================================================================
namespace DUT
{

//==================================================================
//== File system stuff
//==================================================================
void InstallFileManager(DFileManagerBase* pFileManager)
{
	_spFManager = pFileManager;
}

//==================================================================
void InstallFileManagerStd()
{
#if defined(DUTILS_FILEMANAGERSTD_H)
	static FileManagerStd	manager;

	InstallFileManager( &manager );
#else
	DASSERT( 0 );
#endif
}

//==================================================================
void InstallFileManagerAndroid()
{
#if defined(DUTILS_FILEMANAGERANDROID_H)
	static FileManagerAndroid	manager;

	InstallFileManager( &manager );
#else
	DASSERT( 0 );
#endif
}

//==================================================================
bool FileExists( const char *pFileName, bool prefs )
{
	return _spFManager->FileExists( pFileName, prefs ? "p" : NULL );
}

//==================================================================
bool GrabFile( const char *pFileName, DVec<U8> &out_data, bool prefs )
{
	return _spFManager->GrabFile( pFileName, out_data, prefs ? "p" : NULL );
}

//==================================================================
bool SaveFile( const char *pFileName, const U8 *pInData, size_t dataSize, bool prefs )
{
	return _spFManager->SaveFile( pFileName, pInData, dataSize, prefs ? "p" : NULL );
}

//==================================================================
bool GrabFile( const char *pFileName, MemReader &reader, bool prefs )
{
	DVec<U8> data;
	bool ret = GrabFile( pFileName, data, prefs );
	reader.InitOwnVec( data );
	return ret;
}

//==================================================================
bool SaveFile( const char *pFileName, const MemWriterDynamic &mw, bool prefs )
{
	return SaveFile( pFileName, mw.GetDataBegin(), mw.GetCurSize(), prefs );
}

//==================================================================
//== Path name handling, etc.
//==================================================================
const char *GetFileNameOnly( const char *pPathFileName )
{
	int	len = strlen( pPathFileName );

	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '/' || pPathFileName[i] == '\\' )
			return pPathFileName + i + 1;

	return pPathFileName + len;
}

//==================================================================
const char *GetFileNameExt( const char *pPathFileName )
{
	int	len = strlen( pPathFileName );

	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '.' )
			return pPathFileName + i + 1;

	return pPathFileName + len;
}

//==================================================================
char *GetFileNameExt( char *pPathFileName )
{
	return (char *)GetFileNameExt( (const char *)pPathFileName );
}

//==================================================================
DStr GetDirNameFromFPathName( const char *pInFPathname )
{
	const char *pInFPathnameEnd = pInFPathname + strlen( pInFPathname );

	const char *pFNamePtr = GetFileNameOnly( pInFPathname );

	if ( pFNamePtr >= pInFPathnameEnd )
		return DStr();	// and empty string after all..
	else
	{
		size_t	len = pFNamePtr - pInFPathname - 1;

		DStr	tmp( pInFPathname );
		tmp.resize( len );

		return tmp;
	}
}

//==================================================================
//== OBSOLETE stuff
//==================================================================
#if !defined(ANDROID)
FILE *BeginGrabFile( const char *pFileName, size_t &out_fileSize )
{
	out_fileSize = 0;

	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
		return NULL;

	fseek( pFile, 0, SEEK_END );
	out_fileSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	return pFile;
}

//==================================================================
bool EndGrabFile( FILE *pFile, void *pDest, size_t readSize )
{
	if ( readSize != fread( pDest, 1, readSize, pFile ) )
	{
		fclose( pFile );
		return false;
	}

	fclose( pFile );
	return true;
}
#endif

//==================================================================
// Not really needed for now by RibTools
#if 0
DStr GetFullDirNameFromFPathName( const char *pInFPathname )
{
#if 0
	return GetDirNameFromFPathName( pInFPathname );

#else
	DStr	tmp = GetDirNameFromFPathName( pInFPathname );

	char	buff[2048];

#if defined(WIN32)
	GetFullPathNameA( tmp.c_str(), _countof(buff), buff, NULL );

#elif defined(__linux__) || defined(__APPLE__)
	buff[0] = 0;
	DASSERT( 0 );	// TODO

#endif

	return buff;
#endif
}
#endif

//==================================================================
}
