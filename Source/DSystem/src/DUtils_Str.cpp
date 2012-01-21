//==================================================================
/// DUtils_Str.cpp
///
/// Created by Davide Pasca - 2011/12/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <ctype.h>
#include "DUtils_Str.h"

//==================================================================
namespace DUT
{

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
		if ( IsWhite( ch ) )
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

		if ( foundNonWhite || !IsWhite( ch ) )
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
	// ANSI strstr behavior: it's a match if search is empty
	if NOT( pSearch[0] )
		return pStr;

	for (size_t i=0; pStr[i]; ++i)
	{
		if ( tolower(pSearch[0]) == tolower(pStr[i]) )
		{
			size_t	j = 0;
			for (; pSearch[j]; ++j)
				if ( tolower(pStr[i+j]) != tolower(pSearch[j]) )
					break;

			// reached the end of the search string ?
			if ( pSearch[j] == 0 )
				return pStr + i;	// it's a match
		}
	}

	return NULL;
}

//==================================================================
bool StrStartsWithI( const char *pStr, const char *pSearch )
{
	return pStr == StrStrI( pStr, pSearch );
}

//==================================================================
bool StrEndsWithI( const char *pStr, const char *pSearch )
{
	size_t	strLen = strlen( pStr );
	size_t	searchLen = strlen( pSearch );

	if ( searchLen > strLen )
		return false;

	return 0 == strcasecmp( pStr + strLen - searchLen, pSearch );
}

//==================================================================
void StrToUpper( DStr &str )
{
	for (size_t i=0; i < str.size(); ++i)
		str[i] = toupper( str[i] );
}

//==================================================================
void StrToUpper( char *pStr )
{
	for (size_t i=0; pStr[i]; ++i)
		pStr[i] = toupper( pStr[i] );
}

//==================================================================
void StrToLower( DStr &str )
{
	for (size_t i=0; i < str.size(); ++i)
		str[i] = tolower( str[i] );
}

//==================================================================
const char *StrFindLastOf( const char *pStr, char searchCh )
{
	size_t	len = strlen( pStr );

	for (size_t i=0; i < len; ++i)
	{
		if ( pStr[ len-1 - i ] == searchCh )
			return &pStr[ len-1 - i];
	}

	return NULL;
}

//==================================================================
const char *StrTok_StrQuot( char *pSrcStr, char **ppContext )
{
	if ( pSrcStr )
	{
		*ppContext = pSrcStr;
	}

	char *p = *ppContext;

	if NOT( p )
		return NULL;

	size_t i = 0;
	for (; ; ++i)
	{
		if NOT( p[i] )
			return NULL;

		if NOT( CharIsWhite( p[i] ) )
			break;
	}

	bool isInStr;

	if ( p[i] == '"' )
	{
		isInStr = true;
		++i;
	}
	else
		isInStr = false;

	size_t	start = i;

	for (; p[i]; ++i)
	{
		if ( (isInStr && p[i] == '"') ||
			 (!isInStr && CharIsWhite( p[i] )) )
		{
			p[i] = 0;
			*ppContext = p + i + 1;
			return p + start;
		}
	}

	*ppContext = NULL;
	return p + start;
}

//==================================================================
}
