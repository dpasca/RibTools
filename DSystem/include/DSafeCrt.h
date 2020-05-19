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
#define strcpy_s	strcpy
#define strcat_s	strcat
#define _strdate_s	strdate
#define _strtime_s	strtime
#define sscanf_s	sscanf

//#define vsnprintf_s(_DST_,_DSTSIZ_,_MAXCNT_,_FMT_,_ARGS_)	vsnprintf(_DST_,_MAXCNT_,_FMT_,_ARGS_)

int vsprintf_s( char *str, const char *pFmt, va_list vl );
int vsprintf_s( char *str, size_t destMaxSize, const char *pFmt, va_list vl );
int sprintf_s( char *str, const char *format, ...);
int sprintf_s( char *str, size_t destMaxSize, const char *format, ...);

#define	_getcwd		getcwd

typedef int	errno_t;

errno_t fopen_s( FILE **out_ppFile, const char *pFName, const char *pMode );

int vsnprintf_s( char *str, size_t strMaxLen, size_t size, const char *format, va_list ap );

void strtime( char *pDest, size_t maxLen=0 );

char *strcpy_s( char *pDest, size_t destSize, const char *pSrc );
char *strcat_s( char *pDest, size_t destSize, const char *pSrc );

#else

#define snprintf	_snprintf

#endif

#if defined(_DEBUG)
# define ASSTHROW(_X_,_MSG_, ...)   DASSTHROW(_X_,_MSG_, ##__VA_ARGS__)
#else
# define ASSTHROW(_X_,_MSG_, ...)
#endif

void numstrdate( char *pDest, size_t maxLen=0 );

#endif
