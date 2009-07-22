//==================================================================
/// DUtils_Files.cpp
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

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
bool FileExists( const char *pFileName )
{
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
}

//==================================================================
const char *GetFileNameOnly( const char *pPathFileName )
{
	int	idx = 0;
	int	len = strlen( pPathFileName );
	
	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '/' || pPathFileName[i] == '\\' )
			return pPathFileName + i + 1;
			
	return pPathFileName + len;
}

//==================================================================
const char *GetFileNameExt( const char *pPathFileName )
{
	int	idx = 0;
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
	const char *pFNamePtr = GetFileNameOnly( pInFPathname );

	if ( pFNamePtr <= pInFPathname )
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
}
