//==================================================================
/// DUtils.cpp
///
/// Created by Davide Pasca - 2009/5/1
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#define D_LOGGINGx
#define D_TRACKINGx

#include "dlog.h"

#if defined(_MSC_VER)
# include <Windows.h>
#else
# include <sys/time.h>
#endif

#ifdef __APPLE__
# include "TargetConditionals.h"
# include <signal.h>
# include <unistd.h>

# if defined(MACOSX)
#  undef TARGET_OS_IPHONE
# endif
#endif

#if defined(TARGET_OS_IPHONE)
# include "IPH_Utils.h"
#endif

#include <stdio.h>
#if !defined(ANDROID)
# include <stdexcept>
#else
# include <ctype.h>
# include "native.h"
#endif
#include <stdarg.h>
#include "DTypes.h"
#include "DUtils.h"
#include "DMemory.h"

//==================================================================
namespace DUT
{

//===============================================================
DStr VSSPrintFS( const char *pFmt, va_list vl )
{
	char	buff[1024];
	vsnprintf( buff, sizeof(buff), pFmt, vl );

	return DStr( buff );
}

//==================================================================
DStr SSPrintFS( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	DStr ret = VSSPrintFS( pFmt, vl );

	va_end( vl );

	return ret;
}

//===============================================================
void DAssert( bool ok, const char *pFile, int line, const char *msg )
{
	if ( ok )
		return;

	char	buff[1024];

	sprintf(buff, "ASSERT: %s:%i '%s'\n", pFile, line, (msg==0)?(""):msg);

	printf( "%s", buff );

#if defined(_DEBUG) || defined(DEBUG)

# if defined(_MSC_VER)

    OutputDebugString( buff );
	DebugBreak();

# elif defined(TARGET_OS_IPHONE)
	kill( getpid(), SIGINT );

# elif defined(ANDROID)
    PlatformAssertRaise(buff);

# elif defined(MACOSX)
    //
    abort();

# elif defined(NACL)

    *(int *)0 = 0;

# else
#  error Assert not defined on this platform

# endif

#endif
}

//===============================================================
void DAssThrow( bool ok, const char *pFile, int line, const char *pFmt, ... )
{
	if ( ok )
		return;

	va_list	vl;
	va_start( vl, pFmt );

	DVAssThrow( ok, pFile, line, pFmt, vl );

	va_end( vl );
}

//===============================================================
void DVAssThrow( bool ok, const char *pFile, int line, const char *pFmt, va_list vl )
{
	if ( ok )
		return;

	char	buff[1024];
	vsnprintf( buff, sizeof(buff), pFmt, vl );

	DSAssThrow( ok, pFile, line, buff );
}

//===============================================================
void DSAssThrow( bool ok, const char *pFile, int line, const char *pNewCharMsg )
{
	if ( ok )
		return;

	char	buff[1024];

	if ( pNewCharMsg )
	{
		sprintf( buff, "ASSERT EXCEPT: %s - %s %i\n", pNewCharMsg, pFile, line );
	}
	else
	{
		sprintf( buff, "ASSERT EXCEPT: %s %i\n", pFile, line );
	}

	//puts( buff );

#if defined(ANDROID)
    PlatformAssertRaise(buff);
#else
	throw std::runtime_error( buff );
#endif
}

//==================================================================
void DVerbose(const char *fmt, ... )
{
	va_list	vl;
	va_start( vl, fmt );

    DVVerbose(fmt, vl);
}

//==================================================================
void DVVerbose(const char *fmt, va_list vl)
{
#if defined(ANDROID)
    __android_log_vprint(ANDROID_LOG_INFO, "oyatsukai", fmt, vl);
#else
    vprintf(fmt, vl);
#endif
}

//==================================================================
void DDebugOut(const char *fmt, ... )
{
	va_list	vl;
	va_start( vl, fmt );

    DVDebugOut(fmt, vl);
}
//==================================================================
void DVDebugOut(const char *fmt, va_list vl)
{
    DVVerbose( fmt, vl );

    // like DVVerbose, but with additional debugger output
#if defined(_MSC_VER)
	char	buff[2048];
    vsnprintf( buff, sizeof(buff), fmt, vl);
    OutputDebugString( buff );
#endif
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
void OpenURL( const char *pURL )
{
#if defined(TARGET_OS_IPHONE)

	IPHUT::OpenURL( pURL );

#elif defined(WIN32)

	ShellExecute( NULL, "open", pURL, NULL, NULL, SW_SHOWNORMAL );

#elif defined(ANDROID)

    GameWindowOpenURL(pURL);

#else

	// DAVIDE - DPRINT seems to be undefined (?)
    //DPRINT("(unimplemented) OpenURL: %s", pURL);
    printf("(unimplemented) OpenURL: %s\n", pURL);

#endif
}

//==================================================================
void ShowAlert( const char *pTitle, const char *pText )
{
#if defined(TARGET_OS_IPHONE)

    IPHUT::ShowAlert( pTitle, pText );

#elif defined(ANDROID)

    DPRINT("Alert: %s: %s", pTitle, pText);
    GameWindowNotifyUser(pTitle, pText);

#else

    printf("(unimplemented)\nTitle: %s\nText:\n%s\n", pTitle, pText );

#endif
}

//==================================================================
void CharScreen::clear()
{
	mLines.clear();
	mNX = 0;
}

//==================================================================
void CharScreen::AddLine( const char *pFmt, ... )
{
	char	buff[1024];

	va_list	vl;
	va_start( vl, pFmt );
	vsnprintf( buff, sizeof(buff), pFmt, vl );
	va_end( vl );

	mLines.push_back( buff );

	if ( (u_int)mLines.back().size() > mNX )
		mNX = (u_int)mLines.back().size();
}

//==================================================================
void CharScreen::AddLine()
{
	mLines.push_back( DStr() );
}

//==================================================================
}
