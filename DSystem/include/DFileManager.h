//==================================================================
/// DFileManager.h
///
/// Created by Davide Pasca - 2012/02/13/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DFILEMANAGER_H
#define DFILEMANAGER_H

#include "DTypes.h"
#include "DContainers.h"

//==================================================================
class DFileManagerBase
{
public:
    DFileManagerBase(){}
    virtual ~DFileManagerBase(){}

    virtual bool FileExists(const char* pFileName, const char* pMode) const = 0;
    virtual bool GrabFile(const char* pFileName, DVec<U8> &out_data, const char* pMode) = 0;
    virtual bool SaveFile(const char* pFileName, const U8 *pInData, size_t dataSize, const char* pMode) = 0;
};

#endif
