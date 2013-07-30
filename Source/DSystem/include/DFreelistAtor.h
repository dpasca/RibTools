//==================================================================
/// DFreelistAtor.h
///
/// Created by Davide Pasca - 2012/12/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DFREELISTATOR_H
#define DFREELISTATOR_H

#include "DMemory.h"

//==================================================================
template <class _T>
class DFreelistAtor
{
    DVec<U8 *>  mpFreeList;
public:

    DFreelistAtor( size_t warmUpSize=16 )
    {
        // warm up.. with an arbitrary quantity
        for (size_t i=0; i != warmUpSize; ++i)
            mpFreeList.push_back( DNEW U8 [sizeof(_T)] );
    }

    ~DFreelistAtor()
    {
        for (size_t i=0; i != mpFreeList.size(); ++i)
        {
            DASSERT( mpFreeList[i] != NULL );
            DDELETE_ARRAY( mpFreeList[i] );
        }
    }

    _T *Alloc()
    {
        if ( mpFreeList.size() ) {
            _T *pRet = (_T *)mpFreeList.back();
            mpFreeList.pop_back();
            return pRet;
        }
        else {
            return (_T *)DNEW U8 [sizeof(_T)];
        }
    }

    void Free( void *p )
    {
        mpFreeList.push_back( (U8 *)p );
    }
};


#endif
