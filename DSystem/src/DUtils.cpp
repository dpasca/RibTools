//==================================================================
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
bool GrabFile( const char *pFileName, void * &out_pData, size_t &out_dataSize )
{
	out_pData		= NULL;
	out_dataSize	= 0;
	
	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}
	
	fseek( pFile, 0, SEEK_END );
	out_dataSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	
	out_pData = malloc( out_dataSize );
	
	fread( out_pData, 1, out_dataSize, pFile );
	
	fclose( pFile );
	
	return true;
}

//==================================================================
const char *GetFileNameOnly( const char *pPathFileName )
{
	int	idx = 0;
	int	len = strlen( pPathFileName );
	
	for (int i=len-1; i >= 0; --i)
		if ( pPathFileName[i] == '/' || pPathFileName[i] == '\\' )
			return pPathFileName + i + 1;
			
	return pPathFileName + len;
}

//==================================================================
DStr GetDirNameFromFPathName( const char *pInFPathname )
{
	const char *pFNamePtr = GetFileNameOnly( pInFPathname );

	if ( pFNamePtr <= pInFPathname )
		return DStr();	// and empty string after all..
	else
	{
		size_t	len = pFNamePtr - pInFPathname - 1;
		
		DStr	tmp( pInFPathname );
		tmp.resize( len );

		return tmp;
	}
}

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

//===============================================================
void DAssThrow( bool ok, const char *pFile, int line, char *pNewCharMsg )
{
	if ( ok )
		return;

	char	buff[1024];
	
	if ( pNewCharMsg )
	{
		sprintf( buff, "ASSERT: %s - %s %i\n", pNewCharMsg, pFile, line );

		DSAFE_DELETE_ARRAY( pNewCharMsg );
	}
	else
	{
		sprintf( buff, "ASSERT: %s %i\n", pFile, line );
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
	mOwnData(false),
	mIsReadOnly(true)
{
}

//==================================================================
MemFile::MemFile( const char *pFileName ) :
	mpData(NULL),
	mDataSize(0),
	mReadPos(0),
	mOwnData(true),
	mIsReadOnly(true)
{
	void	*pData;
	size_t	dataSize;
	if NOT( DUT::GrabFile( pFileName, pData, dataSize ) )
	{
		DASSTHROW( 0, ("Could not open the file %s in input.", pFileName) );
	}
	
	mpData = (const U8 *)pData;
	mDataSize = dataSize;
}

//==================================================================
MemFile::~MemFile()
{
	if ( mOwnData )
		free( (void *)mpData );
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
