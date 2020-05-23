//==================================================================
/// RI_Exceptions.cpp
///
/// Created by Davide Pasca - 2010/2/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include <stdarg.h>
#include "DSystem/include/DUtils.h"
#include "RI_Exceptions.h"

//==================================================================
namespace RI
{

//==================================================================
Exception::Exception( const char *pFmt, ... )
{
    va_list	vl;
    va_start( vl, pFmt );

    char	buff[4096];

    vsnprintf_s( buff, sizeof(buff), _countof(buff)-1, pFmt, vl );
    va_end( vl );

    mMsg = buff;
}

//==================================================================
}
