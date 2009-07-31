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
#include "DContainers.h"

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, DVec<U8> &out_data );

bool FileExists( const char *pFileName );

DStr GetDirNameFromFPathName( const char *pInFPathname );

const char *GetFileNameOnly( const char *pPathFileName );

const char *GetFileNameExt( const char *pPathFileName );
char *GetFileNameExt( char *pPathFileName );

//==================================================================
/// MemFile
//==================================================================
class MemFile
{
	DVec<U8>	mOwnData;
	const U8	*mpData;
	size_t		mDataSize;
	size_t		mReadPos;
	bool		mIsReadOnly;

public:
	MemFile( const void *pDataSrc, size_t dataSize );
	MemFile( const char *pFileName );
	~MemFile();

	const U8 *GetData() const	{	return mpData;		}
	size_t GetDataSize() const	{	return mDataSize;	}

	bool ReadTextLine( char *pDestStr, size_t destStrMaxSize );
};

//==================================================================
}

#endif
