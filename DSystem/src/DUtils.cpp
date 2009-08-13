//============f======================================================
/// DUtils.cpp
///
/// Created by Davide Pasca - 2009/5/1
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#if defined(_MSC_VER)
    #include <Windows.h>
#else
    #include <sys/time.h>
#endif

#include <stdio.h>
#include <stdexcept>
#include <stdarg.h>
#include "DTypes.h"
#include "DUtils.h"
#include "DMemory.h"

//==================================================================
namespace DUT
{

//==================================================================
char *SSPrintF( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	char *p = DNEW char [ strlen(buff)+1 ];
	strcpy( p, buff );

	return p;
}

//==================================================================
std::string SSPrintFS( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	return std::string( buff );
}

//===============================================================
void DAssert( bool ok, const char *pFile, int line )
{
	if ( ok )
		return;

	char	buff[1024];

	sprintf( buff, "ASSERT: %s %i\n", pFile, line );

	puts( buff );

#if defined(_DEBUG)

#if defined(_MSC_VER)
	DebugBreak();
#else
	__asm__ ( "int3" );
#endif

#endif
}

//===============================================================
void DAssThrow( bool ok, const char *pFile, int line, char *pNewCharMsg )
{
	if ( ok )
		return;

	char	buff[1024];

	if ( pNewCharMsg )
	{
		sprintf( buff, "ASSERT EXCEPT: %s - %s %i\n", pNewCharMsg, pFile, line );

		DSAFE_DELETE_ARRAY( pNewCharMsg );
	}
	else
	{
		sprintf( buff, "ASSERT EXCEPT: %s %i\n", pFile, line );
	}

	puts( buff );

#if !defined(NDEBUG)
#endif
	throw std::runtime_error( buff );
}

//==================================================================
static bool isWhite( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}

//==================================================================
void StrStripBeginEndWhite( char *pStr )
{
	size_t	len = strlen( pStr );

	if NOT( len )
		return;

	int	newLen = len;
	for (int i=(int)len-1; i >= 0; --i)
	{
		char ch = pStr[i];
		if ( isWhite( ch ) )
			pStr[i] = 0;
		else
		{
			newLen = i + 1;
			break;
		}
	}

	size_t	di = 0;
	bool	foundNonWhite = false;
	for (int si=0; si < newLen; ++si)
	{
		char ch = pStr[si];

		if ( foundNonWhite || !isWhite( ch ) )
		{
			pStr[di++] = pStr[si];
			foundNonWhite = true;
		}
	}
	pStr[di] = 0;
}

//==================================================================
const char *StrStrI( const char *pStr, const char *pSearch )
{
	for (size_t i=0; pStr[i]; ++i)
	{
		if ( tolower(pSearch[0]) == tolower(pStr[i]) )
		{
			for (size_t j=0; pSearch[j]; ++j)
			{
				if ( tolower(pStr[i+j]) != tolower(pSearch[j]) )
					return NULL;
			}

			return pStr + i;
		}
	}

	return NULL;
}

//==================================================================
I64 GetTimeTicks()
{
	I64	val;

#if defined(_MSC_VER)
	QueryPerformanceCounter( (LARGE_INTEGER *)&val );

#else
	timeval		timeVal;

	gettimeofday( &timeVal, NULL );

	val = (I64)timeVal.tv_sec * (1000*1000) + (I64)timeVal.tv_usec;
#endif

	return val;
}

//==================================================================
double TimeTicksToMS( I64 ticks )
{
	static double	coe;

#if defined(_MSC_VER)
	static I64  	freq;

	if ( freq == 0 )
	{
		QueryPerformanceFrequency( (LARGE_INTEGER *)&freq );
		coe = 1000.0 / freq;
	}

#else
	coe = 1.0 / 1000.0;

#endif

	return ticks * coe;
}

//==================================================================
void SleepMS( U32 sleepMS )
{
#if defined(WIN32)
	Sleep( sleepMS );
#endif
}

//==================================================================
}
