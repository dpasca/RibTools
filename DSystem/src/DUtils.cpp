//============f======================================================
/// DUtils.cpp
///
/// Created by Davide Pasca - 2009/5/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <Windows.h>
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
	DebugBreak();
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
/// MemFile
//==================================================================
MemFile::MemFile( const void *pDataSrc, size_t dataSize ) :
	mpData((const U8 *)pDataSrc),
	mDataSize(dataSize),
	mReadPos(0),
	mIsReadOnly(true)
{
}

//==================================================================
MemFile::MemFile( const char *pFileName ) :
	mpData(NULL),
	mDataSize(0),
	mReadPos(0),
	mIsReadOnly(true)
{

	if NOT( DUT::GrabFile( pFileName, mOwnData ) )
	{
		DASSTHROW( 0, ("Could not open the file %s in input.", pFileName) );
	}
	
	mpData = (const U8 *)&mOwnData[0];
	mDataSize = mOwnData.size();
}

//==================================================================
MemFile::~MemFile()
{
}

//==================================================================
bool MemFile::ReadTextLine( char *pDestStr, size_t destStrMaxSize )
{
	if ( mReadPos >= mDataSize )
		return false;

	size_t	destIdx = 0;
	while ( mReadPos < mDataSize )
	{
		char ch = mpData[ mReadPos++ ];

		if ( ch == '\n' )
		{
			break;
		}
		else
		{
			DASSTHROW( destIdx < destStrMaxSize, ("Writing out of bounds !") );
			pDestStr[ destIdx++ ] = ch;
		}
	}

	DASSTHROW( destIdx < destStrMaxSize, ("Writing out of bounds !") );
	pDestStr[ destIdx ] = 0;
	
	DASSERT( mReadPos <= mDataSize );

	return true;
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
I64 GetTimeTicks()
{
	__int64	val;
	QueryPerformanceCounter( (LARGE_INTEGER *)&val );

	return val;
}

//==================================================================
double TimeTicksToMS( I64 ticks )
{
	static __int64	freq;
	static double	coe;
	
	if ( freq == 0 )
	{
		QueryPerformanceFrequency( (LARGE_INTEGER *)&freq );
		coe = 1.0 / freq;
	}

	return ticks * coe; 
}

//==================================================================
}
