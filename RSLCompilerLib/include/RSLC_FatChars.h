//==================================================================
/// RSLC_FatChars.h
///
/// Created by Davide Pasca - 2010/2/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_FATCHARS_H
#define RSLC_FATCHARS_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include "DSystem/include/DStr.h"

//==================================================================
namespace RSLC
{

//==================================================================
class Fat8
{
public:
    U8			Ch;
    size_t		SrcPos;
    size_t		FNameIdx;

    Fat8() :
        Ch(0),
        SrcPos(DNPOS),
        FNameIdx(DNPOS)
    {
    }
};

//==================================================================
typedef DVec<Fat8> Fat8Vec;

//==================================================================
class FatBase
{
public:
    DVec<DStr>	mFileNames;

    size_t InsertNewFile( DVec<Fat8> &desData, size_t inserStart, size_t insertEnd, const char *pSrcFName, const U8 *pSrcData, size_t srcSize );

    void AppendNewFile( DVec<Fat8> &desData, const char *pSrcFName, const DVec<U8> &srcData );
    void AppendNewFile( DVec<Fat8> &desData, const char *pSrcFName, const U8 *pSrcData, size_t srcSize );

    void Append( DVec<Fat8> &desData, const DVec<Fat8> &srcData )
    {
        desData.insert( desData.end(), srcData.begin(), srcData.end() );
    }
};

//==================================================================
}

#endif
