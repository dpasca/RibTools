//==================================================================
/// RSLC_Tokenize.cpp
///
/// Created by Davide Pasca - 2010/3/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_TextUtils.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Tokenize.h"

//==================================================================
namespace RSLC
{

//==================================================================
static bool matches( const Fat8Vec &str, size_t i, const char *pFindStr )
{
	for (; i < str.size() && *pFindStr; ++i)
		if ( str[i].Ch != *pFindStr++ )
			return false;

	return true;
}

//==================================================================
static bool isDigit( char ch )
{
	return ch >= '0' && ch <= '9';
}

//==================================================================
struct SortItem
{
	size_t	strLen;
	size_t	idx;
};

//==================================================================
static int tokNamesCmpFunc( const void *a, const void *b )
{
	const SortItem	*pA = (const SortItem *)a;
	const SortItem	*pB = (const SortItem *)b;

	return (int)pB->strLen - (int)pA->strLen;
}

//==================================================================
static size_t _sTokenDefsIdxInvSortLen[TOKEN_N];

//==================================================================
static void initSortedTable()
{
	if ( _sTokenDefsIdxInvSortLen[0] != 0 )
		return;

	SortItem	sortItems[ TOKEN_N ];

	for (size_t i=0; i < TOKEN_N; ++i)
	{
		sortItems[i].strLen = _TokenDefs[i].pStr ? strlen( _TokenDefs[i].pStr ) : 0;
		sortItems[i].idx	= i;
	}

	qsort( sortItems, TOKEN_N, sizeof(SortItem), tokNamesCmpFunc );

	for (size_t i=0; i < TOKEN_N; ++i)
	{
		_sTokenDefsIdxInvSortLen[i] = sortItems[i].idx;
	}
}

//==================================================================
/// Tokenize
//==================================================================
Tokenize::Tokenize( const Fat8Vec &str, DVec<Token> &tokens, FatBase &fatBase ) :
	mStr(str),
	mTokens(tokens),
	mFatBase(fatBase)
{
	// do it here.. static stuff ! ..non MT-safe 8)
	initSortedTable();
}

//==================================================================
void Tokenize::newToken( const Fat8 &fatCh )
{
	if ( mTokens.back().IsSet() )
	{
		mTokens.grow();
		mTokens.back().sourceLine = fatCh.SrcPos;
		mTokens.back().pSourceFileName = mFatBase.mFileNames[ fatCh.FNameIdx ].c_str();
	}
}

//==================================================================
bool Tokenize::matchTokenDef( size_t &i, bool wasPrecededByWS )
{
	for (size_t j=0; j < TOKEN_N; ++j)
	{
		const TokenDef &tokDef = _TokenDefs[ _sTokenDefsIdxInvSortLen[ j ] ];

		if NOT( tokDef.pStr )
			continue;

		if ( matches( mStr, i, tokDef.pStr ) )
		{
			size_t	findStrLen = strlen( tokDef.pStr );

			// did we match an alphanumerical string ?
			if ( IsAlphaNumStart( tokDef.pStr[0] ) )
			{
				// have some more on the right ?
				if ( (i+findStrLen) < mStr.size() )
				{
					// is that alphanumeric ?
					if ( IsAlphaNumBody( mStr[ i+findStrLen ].Ch ) )
					{
						// ..if yes, then it's a bad match
						continue;
					}
				}
			}

			newToken( mStr[i] );
			mTokens.back().str		= tokDef.pStr;
			mTokens.back().idType	= tokDef.idType;
			mTokens.back().id		= tokDef.id;
			i += findStrLen;
			if ( wasPrecededByWS )
			{
				mTokens.back().isPrecededByWS = wasPrecededByWS;
			}
			return true;
		}
	}

	return false;
}

//==================================================================
bool Tokenize::handleAlphanum( size_t &i )
{
	if NOT( IsAlphaNumStart( mStr[i].Ch ) )
		return false;

	newToken( mStr[i] );
	mTokens.back().idType = T_TYPE_NONTERM;
	mTokens.back().id	 = T_NONTERM;
	mTokens.back().str	= mStr[i++].Ch;

	for (; i < mStr.size(); ++i)
	{
		if ( !IsAlphaNumBody( mStr[i].Ch ) )
			break;

		mTokens.back().str += mStr[i].Ch;
	}

	return true;
}

//==================================================================
bool Tokenize::handleNumber( size_t &i )
{
	char ch		= (char)mStr[i].Ch;

	// not the beginning of a number.. ?
	if ( (ch < '0' || ch > '9') && ch != '.' )
		return false;

	char nextCh = (i+1) < mStr.size() ? (char)mStr[i+1].Ch : 0;

	// starts with a dot ?
	if ( ch == '.' )	// next must be a digit
		if NOT( isDigit( nextCh ) )
			return false;

	newToken( mStr[i] );

	mTokens.back().idType = T_TYPE_VALUE;
	mTokens.back().id	 = T_VL_NUMBER;

	char	prevCh = 0;

	bool	gotDot = false;

	for (; i < mStr.size();)
	{
		ch		= mStr[i].Ch;
		nextCh	= (i+1) < mStr.size() ? (char)mStr[i+1].Ch : 0;

		if ( isDigit( ch ) )
		{
			mTokens.back().str += ch;
			++i;
		}
		else
		if ( ch == '.' )
		{
			if ( gotDot )
			{
				// 2+ decimal dots means bad number !
				mTokens.back().isBadNumber = true;
				newToken( mStr[i] );
				return true;
			}

			gotDot = true;
			mTokens.back().str += ch;
			++i;
		}
		else
		if ( ch == 'f' || ch == 'F' || ch == 'd' || ch == 'D' )	// end of a float or double ?
		{
			mTokens.back().str += ch;
			mTokens.back().UpdateConstNum();
			++i;
			newToken( mStr[i] );
			return true;
		}
		else
		if ( ch == 'e' || ch == 'E' )
		{
			mTokens.back().str += ch;
			++i;	// take it

			if ( nextCh != 0 )
			{
				mTokens.back().str += nextCh;
				++i;	// take the next character if there is any
			}

			// must be +, - or a digit
			if NOT( nextCh == '+' || nextCh == '-' || isDigit( nextCh ) )
			{
				// if not, then it's a bad number !
				mTokens.back().isBadNumber = true;
				newToken( mStr[i] );
				return true;
			}
		}
		else
		{
			break;
		}

		prevCh = ch;
	}

	mTokens.back().UpdateConstNum();

	newToken( mStr[ i < mStr.size() ? i : (int)mStr.size()-1 ] );
	return true;
}

//==================================================================
bool Tokenize::handleString( size_t &i )
{
	if ( mStr[i].Ch == '"' )
	{
		newToken( mStr[i] );
		mTokens.back().idType = T_TYPE_VALUE;
		mTokens.back().id	 = T_VL_STRING;

		//mTokens.back().str = mStr[i++];
		++i;
		for (; i < mStr.size(); ++i)
		{
			if ( mStr[i].Ch == '"' )
			{
				++i;
				return true;
			}
			else
				mTokens.back().str += mStr[i].Ch;
		}

		throw Exception( "String not closing !", &mTokens.back() );
	}

	return false;
}

//==================================================================
}
