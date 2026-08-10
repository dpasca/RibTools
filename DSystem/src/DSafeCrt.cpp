//==================================================================
/// DSafeCrt.h
///
/// Created by Davide Pasca - 2009/07/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <string.h>
#include "DUtils_Base.h"
#include "DSafeCrt.h"

//===============================================================
#if !defined(_MSC_VER)

//==================================================================
errno_t fopen_s( FILE **out_ppFile, const char *pFName, const char *pMode )
{
    if ( (*out_ppFile = fopen( pFName, pMode )) )
        return 0;
    else
        return -1;
}

//==================================================================
void strtime( char *pDest, size_t maxLen )
{
    DASSTHROW( pDest != NULL && maxLen > 0, "Invalid destination buffer" );

    const time_t t = time( NULL );
    const char *pTime = ctime( &t );
    DASSTHROW( pTime != NULL, "Could not format the current time" );

    snprintf( pDest, maxLen, "%s", pTime );
}

//==================================================================
int vsnprintf_s( char *str, size_t strMaxLen, size_t size, const char *format, va_list ap )
{
    int len = vsnprintf( str, size, format, ap );

    // verify only later.. better than nothing !
    DASSERT( (size_t )(len+1) <= strMaxLen );

    return len;
}

//==================================================================
char *strcpy_s( char *pDest, size_t destSize, const char *pSrc )
{
    DASSTHROW( pDest != NULL && destSize > 0 && pSrc != NULL, "Invalid string buffer" );
    const size_t srcLen = strlen( pSrc );
    DASSTHROW( (srcLen + 1) <= destSize, "Out of bounds !" );
    return strcpy( pDest, pSrc );
}

//==================================================================
char *strcat_s( char *pDest, size_t destSize, const char *pSrc )
{
    DASSTHROW( pDest != NULL && destSize > 0 && pSrc != NULL, "Invalid string buffer" );
    const size_t srcLen = strlen( pSrc );
    const size_t curDestLen = strnlen( pDest, destSize );
    DASSTHROW( curDestLen < destSize, "Destination string is not terminated" );
    DASSTHROW( (curDestLen + srcLen + 1) <= destSize, "Out of bounds !" );
    return strcat( pDest, pSrc );
}

//==================================================================
int vsprintf_s( char *str, size_t destMaxSize, const char *pFmt, va_list vl )
{
    if ( str == NULL || destMaxSize == 0 || pFmt == NULL )
        return -1;

    const int ret = vsnprintf( str, destMaxSize, pFmt, vl );
    if ( ret < 0 || static_cast<size_t>(ret) >= destMaxSize )
    {
        str[destMaxSize - 1] = 0;
        return -1;
    }

    return ret;
}

//==================================================================
int sprintf_s( char *str, size_t destMaxSize, const char *format, ... )
{
    va_list vl;
    va_start( vl, format );
    const int ret = vsprintf_s( str, destMaxSize, format, vl );
    va_end( vl );
    return ret;
}

#endif

//==================================================================
void numstrdate( char *pDest, size_t maxLen )
{
    DASSTHROW( pDest != NULL && maxLen > 0, "Invalid destination buffer" );

    const time_t t = time( NULL );
    const struct tm *pTime = localtime( &t );
    DASSTHROW( pTime != NULL, "Could not read the current date" );

    snprintf(
        pDest,
        maxLen,
        "%i/%02i/%02i",
        pTime->tm_year + 1900,
        pTime->tm_mon + 1,
        pTime->tm_mday );
}
