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

FILE *BeginGrabFile( const char *pFileName, size_t &out_fileSize );
bool EndGrabFile( FILE *pFile, void *pDest, size_t readSize );

bool FileExists( const char *pFileName );

DStr GetDirNameFromFPathName( const char *pInFPathname );
DStr GetFullDirNameFromFPathName( const char *pInFPathname );

const char *GetFileNameOnly( const char *pPathFileName );

const char *GetFileNameExt( const char *pPathFileName );
char *GetFileNameExt( char *pPathFileName );

//==================================================================
}

#endif
