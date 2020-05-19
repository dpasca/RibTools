//==================================================================
/// DUtils_FileManagerAndroid.h
///
/// Created by Davide Pasca - 2012/2/13
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DUTILS_FILEMANAGERANDROID_H
#define DUTILS_FILEMANAGERANDROID_H

#include "DFileManager.h"

//==================================================================
namespace DUT
{

//==================================================================
class FileManagerAndroid : public DFileManagerBase
{
public:
	virtual bool FileExists(const char* pFileName, const char* pMode=NULL) const;
	virtual bool GrabFile(const char* pFileName, DVec<U8> &out_data, const char* pMode=NULL);
	virtual bool SaveFile(const char* pFileName, const U8 *pInData, size_t dataSize, const char* pMode=NULL);
};

//==================================================================
}

#endif
