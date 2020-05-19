//==================================================================
/// DUtils_FileStdManager.cpp
///
/// Created by Davide Pasca - 2012/2/13
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "dlog.h"
#include "DUtils_FileManagerStd.h"

//==================================================================
namespace DUT {

//==================================================================
bool FileManagerStd::FileExists(const char* pFileName, const char* pMode) const
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
bool FileManagerStd::GrabFile(const char* pFileName, DVec<U8> &out_data, const char* pMode)
{
    DFUNCTION();
    // DLOG("File: %s", pFileName);

    //(void )pMode;
	out_data.clear();

	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}

	fseek( pFile, 0, SEEK_END );
	out_data.resize( ftell( pFile ) );
	fseek( pFile, 0, SEEK_SET );

	if ( out_data.size() )
		fread( &out_data[0], 1, out_data.size(), pFile );

	fclose( pFile );

	return true;
}

//==================================================================
bool FileManagerStd::SaveFile(const char* pFileName, const U8 *pInData, size_t dataSize, const char* pMode)
{
    DFUNCTION();
    DLOG("Saving file: %s", pFileName);

	FILE	*pFile = fopen( pFileName, "wb" );
	if NOT( pFile )
	{
		return false;
	}

	if ( dataSize != fwrite( pInData, 1, dataSize, pFile ) )
	{
		fclose( pFile );
		return false;
	}

	fclose( pFile );

	return true;
}

//==================================================================
}
