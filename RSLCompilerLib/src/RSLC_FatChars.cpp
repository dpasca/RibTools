//==================================================================
/// RSLC_FatChars.cpp
///
/// Created by Davide Pasca - 2010/2/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{

//==================================================================	
size_t FatBase::InsertNewFile( DVec<Fat8> &desData, size_t inserStart, size_t insertEnd, const char *pSrcFName, const U8 *pSrcData, size_t srcSize )
{
    mFileNames.push_back( pSrcFName );
    size_t idx = mFileNames.size() - 1;

    DVec<Fat8> tmpData( desData );

    desData.resize( inserStart );

    size_t curLine = 1;

    for (size_t i=0; i < srcSize; ++i)
    {
        char srcCh = pSrcData[i];

        // a CR ?
        if ( srcCh == 0x0d )
        {
            // followed by LF ?
            if ( (i+1) < srcSize && pSrcData[i+1] == 0x0a )
            {
                // skip this
                continue;
            }
            else
            {
                // set as LF
                srcCh = 0x0a;
            }
        }

        Fat8 &fat = Dgrow( desData );
        fat.Ch = srcCh;
        fat.FNameIdx = idx;
        fat.SrcPos = curLine;

        // a LF ?
        if ( srcCh == 0x0a )
            ++curLine;
    }

    size_t	newInsertEnd = desData.size();

    for (size_t i=insertEnd; i < tmpData.size(); ++i)
    {
        desData.push_back( tmpData[ i ] );
    }

    return newInsertEnd;
}

//==================================================================	
void FatBase::AppendNewFile( DVec<Fat8> &desData, const char *pSrcFName, const U8 *pSrcData, size_t srcSize )
{
    mFileNames.push_back( pSrcFName );
    size_t idx = mFileNames.size() - 1;

    desData.reserve( desData.size() + srcSize );

    size_t curLine = 1;

    for (size_t i=0; i < srcSize; ++i)
    {
        char srcCh = pSrcData[i];

        // a CR ?
        if ( srcCh == 0x0d )
        {
            // followed by LF ?
            if ( (i+1) < srcSize && pSrcData[i+1] == 0x0a )
            {
                // skip this
                continue;
            }
            else
            {
                // set as LF
                srcCh = 0x0a;
            }
        }

        Fat8 &fat = Dgrow( desData );
        fat.Ch = srcCh;
        fat.FNameIdx = idx;
        fat.SrcPos = curLine;

        // a LF ?
        if ( srcCh == 0x0a )
            ++curLine;
    }
}

//==================================================================
void FatBase::AppendNewFile( DVec<Fat8> &desData, const char *pSrcFName, const DVec<U8> &srcData )
{
    AppendNewFile( desData, pSrcFName, &srcData[0], srcData.size() );
}

//==================================================================
}
