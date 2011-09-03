//==================================================================
/// DUtils_Files.cpp
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(WIN32)

#include <Windows.h>
#include <direct.h>

#elif defined(__linux__)

#endif

#include "DUtils_Files.h"

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, DVec<U8> &out_data )
{
	out_data.clear();

	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}
	
	fseek( pFile, 0, SEEK_END );
	out_data.resize( ftell( pFile ) );
	fseek( pFile, 0, SEEK_SET );
	
	fread( &out_data[0], 1, out_data.size(), pFile );
	
	fclose( pFile );
	
	return true;
}

//==================================================================
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

//==================================================================
bool FileExists( const char *pFileName, bool prefs )
{
#if defined(ANDROID)

    // TODO: This is terrible...  We end up loading the whole file,
    // but pref files should be small anyway.

    DASSTHROW(prefs, "FileExists() only supported for preference files");

    DVec<U8> data;
    return GrabFile(pFileName, data, prefs);

#else

    (void )prefs;
	FILE	*pFile = fopen( pFileName, "rb" );

	if ( pFile )
	{
		fclose( pFile );
		return true;
	}
	else
	{
		return false;
	}

#endif
}

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
// Not really needed for now by RibTools
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

//==================================================================
}
