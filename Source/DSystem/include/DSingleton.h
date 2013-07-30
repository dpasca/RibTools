//==================================================================
/// DSingleton.h
///
/// Created by Davide Pasca - 2013/6/19
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DSINGLETON_H
#define DSINGLETON_H

#include <type_traits>
#include <utility>
#include "DUtils_Base.h"

//==================================================================
template <class _T>
class DSingleton
{
    static _T    *mspSingleton;

public:
    static _T &S() { return *mspSingleton; }

    template <typename... Args>
    static void Create(Args&&... args)
    {
        DASSERT( mspSingleton == nullptr );
        mspSingleton = new _T( std::forward<Args>(args)... );
    }

    static void Destroy()
    {
        if ( mspSingleton )
        {
            delete mspSingleton;
            mspSingleton = nullptr;
        }
    }
};

template <class _T> _T* DSingleton<_T>::mspSingleton = nullptr;

#endif
