//==================================================================
/// RSLC_Token.cpp
///
/// Created by Davide Pasca - 2009/5/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdlib.h>
#include "RSLC_Exceptions.h"
#include "RSLC_FatChars.h"
#include "RSLC_Token.h"
#include "RSLC_TextUtils.h"

//==================================================================
namespace RSLC
{

//==================================================================
static size_t _sTokenDefsIdxInvSortLen[TOKEN_N];

//==================================================================
static bool matches( const Fat8Vec &str, size_t i, const char *pFindStr )
{
	for (; i < str.size() && *pFindStr; ++i)
		if ( str[i].Ch != *pFindStr++ )
			return false;

	return true;
}

//==================================================================
static bool findSkipWhites( const Fat8Vec &str, size_t &i )
{
	char	ch = str[i].Ch;

	if ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' )
	{
		for (; i < str.size(); ++i)
		{
			ch = str[ i ].Ch;

			if NOT( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' )
				break;
		}

		return true;
	}
	else
		return false;
}

//==================================================================
static bool isDigit( char ch )
{
	return ch >= '0' && ch <= '9';
}

//==================================================================
/// 
//==================================================================
class Tokenize
{
public:
	const Fat8Vec	&mStr;
	DVec<Token>		&mTokens;
	FatBase			&mFatBase;

	//==================================================================
	Tokenize( const Fat8Vec &str, DVec<Token> &tokens, FatBase &fatBase ) :
		mStr(str),
		mTokens(tokens),
		mFatBase(fatBase)
	{
	}

//==================================================================
void newToken( const Fat8 &fatCh )
{
	if ( mTokens.back().IsSet() )
	{
		mTokens.grow();
		mTokens.back().sourceLine = fatCh.SrcPos;
		mTokens.back().pSourceFileName = mFatBase.mFileNames[ fatCh.FNameIdx ].c_str();
	}
}

//==================================================================
bool matchTokenDef( size_t &i, bool wasPrecededByWS )
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
bool handleAlphanum( size_t &i )
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
bool handleNumber( size_t &i )
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

	newToken( mStr[ i < mStr.size() ? i : (int)mStr.size()-1 ] );
	return true;
}

//==================================================================
bool handleString( size_t &i )
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
};

//==================================================================
const char *GetTokenTypeStr( TokenIDType tokidtype )
{
	switch ( tokidtype )
	{
	case T_TYPE_NONTERM:	return "Non Terminal";
	case T_TYPE_VALUE:		return "Value";
	case T_TYPE_OPERATOR:	return "Operator";
	case T_TYPE_DATATYPE:	return "Data Type";
	case T_TYPE_SHADERTYPE:	return "Shader Type";
	case T_TYPE_KEYWORD:	return "Keyword";

	default:
		return "SYMBOL NOT FOUND ?!!";
	}
}

//==================================================================
const char *GetTokenIDStr( TokenID tokid )
{
	for (size_t i=0; i < TOKEN_N; ++i)
	{
		if ( _TokenDefs[i].id == tokid )
			return _TokenDefs[i].pStr ? _TokenDefs[i].pStr : "N/A";
	}

	return "SYMBOL NOT FOUND ?!!";
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
void Tokenizer( DVec<Token> &tokens, FatBase &fatBase, const Fat8Vec &str )
{
	initSortedTable();

	bool	isInBlockComment	= false;
	bool	isInLineComment		= false;

	tokens.grow();

	TokenDef	*pCurTDef = NULL;

	bool wasPrecededByWS = false;

	Tokenize	tokenize( str, tokens, fatBase );

	for (size_t i=0; i < str.size(); )
	{
		if ( findSkipWhites( str, i ) )
		{
			wasPrecededByWS = true;
			continue;
		}
		else
			wasPrecededByWS = false;

		if ( tokenize.handleNumber( i ) )
			continue;

		if ( tokenize.matchTokenDef( i, wasPrecededByWS ) )
			continue;

		if ( tokenize.handleAlphanum( i ) )
			continue;

		if ( tokenize.handleString( i ) )
			continue;

		throw Exception( fatBase, str[i], "Invalid character '%c' !", str[i].Ch );
	}
}

//==================================================================
bool Token::IsBiOp() const
{
	return
		IsAssignOp()			||
		IsCmpOp()				||
		id == T_OP_LOGIC_AND	|| // general bi-ops
		id == T_OP_LOGIC_OR		||
		id == T_OP_PLUS			||
		id == T_OP_MINUS		||
		id == T_OP_MUL			||
		id == T_OP_DIV			||
		id == T_OP_POW			||
		id == T_OP_DOT			;
}

//==================================================================
}
