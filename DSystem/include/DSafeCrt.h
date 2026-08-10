//==================================================================
/// DSafeCrt.h
///
/// Created by Davide Pasca - 2009/07/25
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DSAFECRT_H
#define DSAFECRT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>

//===============================================================
#if !defined(_MSC_VER)

#define fprintf_s	fprintf
#define _strdate_s	strdate
#define _strtime_s	strtime
#define sscanf_s	sscanf

//#define vsnprintf_s(_DST_,_DSTSIZ_,_MAXCNT_,_FMT_,_ARGS_)	vsnprintf(_DST_,_MAXCNT_,_FMT_,_ARGS_)

int vsprintf_s( char *str, size_t destMaxSize, const char *pFmt, va_list vl );
int sprintf_s( char *str, size_t destMaxSize, const char *format, ...);

template <size_t N>
int sprintf_s( char (&str)[N], const char *format, ... )
{
    va_list vl;
    va_start( vl, format );
    const int ret = vsprintf_s( str, N, format, vl );
    va_end( vl );
    return ret;
}

#define	_getcwd		getcwd

typedef int	errno_t;

errno_t fopen_s( FILE **out_ppFile, const char *pFName, const char *pMode );

int vsnprintf_s( char *str, size_t strMaxLen, size_t size, const char *format, va_list ap );

void strtime( char *pDest, size_t maxLen );

char *strcpy_s( char *pDest, size_t destSize, const char *pSrc );
char *strcat_s( char *pDest, size_t destSize, const char *pSrc );

template <size_t N>
char *strcpy_s( char (&dest)[N], const char *pSrc )
{
    return strcpy_s( dest, N, pSrc );
}

template <size_t N>
char *strcat_s( char (&dest)[N], const char *pSrc )
{
    return strcat_s( dest, N, pSrc );
}

template <size_t N>
void strtime( char (&dest)[N] )
{
    strtime( dest, N );
}

#else

#define snprintf	_snprintf

#endif

#if defined(_DEBUG)
# define ASSTHROW(_X_,_MSG_, ...)   DASSTHROW(_X_,_MSG_, ##__VA_ARGS__)
#else
# define ASSTHROW(_X_,_MSG_, ...)
#endif

void numstrdate( char *pDest, size_t maxLen );

template <size_t N>
void numstrdate( char (&dest)[N] )
{
    numstrdate( dest, N );
}

#endif
