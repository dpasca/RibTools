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

//===============================================================
#if !defined(_MSC_VER)

#define fprintf_s	fprintf
#define strcpy_s	strcpy
#define strcat_s	strcat
#define _strdate_s	strdate
#define _strtime_s	strtime
#define sprintf_s	sprintf

#define	_getcwd		getcwd

typedef int	errno_t;

errno_t fopen_s( FILE **out_ppFile, const char *pFName, const char *pMode );

void strtime( char *pDest, size_t maxLen=0 );

#endif

void numstrdate( char *pDest, size_t maxLen=0 );

#endif
