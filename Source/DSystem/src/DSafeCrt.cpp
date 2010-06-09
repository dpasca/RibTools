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
	if ( *out_ppFile = fopen( pFName, pMode ) )
		return 0;
	else
		return -1;
}

//==================================================================
void strtime( char *pDest, size_t maxLen )
{
	//struct tm	tmm;
	time_t		t;

	t = time( NULL );

	strcpy_s( pDest, ctime( &t ) );
}

//==================================================================
int vsnprintf_s( char *str, size_t strMaxLen, size_t size, const char *format, va_list ap )
{
	int len = vsnprintf( str, size, format, ap );

	// verify only later.. better than nothing !
	DASSERT( (len+1) <= strMaxLen );

	return len;
}

#endif

//==================================================================
void numstrdate( char *pDest, size_t maxLen )
{
	time_t 		t;
	struct tm	*tm;

	t 	= time( NULL );
	tm	= localtime( &t );

	sprintf( pDest, "%i/%02i/%02i",
		tm->tm_year+1900,
		tm->tm_mon+1,
		tm->tm_mday );
}
