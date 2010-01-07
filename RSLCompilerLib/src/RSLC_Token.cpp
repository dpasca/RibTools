//==================================================================
/// RSLC_Token.cpp
///
/// Created by Davide Pasca - 2009/5/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdlib.h>
#include "RSLC_Exceptions.h"
#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{
//==================================================================
static size_t _sTokenDefsIdxInvSortLen[TOKEN_N];

//==================================================================
static bool matches( const char *pStr, size_t i, size_t strSize, const char *pFindStr )
{
	for (; i < strSize && *pFindStr; ++i)
		if ( pStr[i] != *pFindStr++ )
			return false;

	return true;
}

//==================================================================
static bool findSkipWhites( const char *pStr, size_t &i, size_t strSize, int &io_lineCnt )
{
	char	ch = pStr[i];

	if ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' )
	{
		if ( ch == '\n' )
			++io_lineCnt;

		do {
			ch = pStr[ ++i ];

			if ( ch == '\n' )
				++io_lineCnt;

		} while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' );

		return true;
	}

	return false;
}

//==================================================================
static void newToken( DVec<Token> &tokens, int lineCnt )
{
	if ( tokens.back().IsSet() )
	{
		tokens.grow();
		tokens.back().sourceLine = lineCnt;
	}
}

//==================================================================
static bool isAlphaNum( char ch )
{
	return
		(ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z') ||
		(ch >= '0' && ch <= '9') ||
		ch == '_';
}

//==================================================================
static bool isAlphaNumStrFirstChar( char ch )
{
	return
		(ch >= 'a' && ch <= 'z') ||
		(ch >= 'A' && ch <= 'Z')||
		ch == '_';
}

/*
//==================================================================
Token *TokenFromDefOrNTerm( const char *pTokenStr, int lineCnt )
{
	for (size_t j=T_VL_STRING+1; j < TOKEN_N; ++j)
	{
		const TokenDef &tokDef = _TokenDefs[ _sTokenDefsIdxInvSortLen[ j ] ];

		if ( tokDef.pStr && 0 == strcmp( pTokenStr, tokDef.pStr ) )
			return DNEW Token( tokDef.pStr, tokDef.id, tokDef.idType );
	}

	return DNEW Token( pTokenStr, T_NONTERM, T_TYPE_NONTERM );
}
*/


//==================================================================
static bool matchTokenDef(
					DVec<Token> &tokens,
					const char *pSource,
					size_t &i,
					size_t sourceSize,
					bool wasPrecededByWS,
					int lineCnt )
{
	for (size_t j=0; j < TOKEN_N; ++j)
	{
		const TokenDef &tokDef = _TokenDefs[ _sTokenDefsIdxInvSortLen[ j ] ];

		if NOT( tokDef.pStr )
			continue;

		if ( matches( pSource, i, sourceSize, tokDef.pStr ) )
		{
			size_t	findStrLen = strlen( tokDef.pStr );

			// did we match an alphanumerical string ?
			if ( isAlphaNumStrFirstChar( tokDef.pStr[0] ) )
			{
				// have some more on the right ?
				if ( (i+findStrLen) < sourceSize )
				{
					// is that alphanumeric ?
					if ( isAlphaNum( pSource[ i+findStrLen ] ) )
					{
						// ..if yes, then it's a bad match
						continue;
					}
				}
			}

			newToken( tokens, lineCnt );
			tokens.back().str		= tokDef.pStr;
			tokens.back().idType	= tokDef.idType;
			tokens.back().id		= tokDef.id;
			i += findStrLen;
			if ( wasPrecededByWS )
			{
				tokens.back().isPrecededByWS = wasPrecededByWS;
			}
			return true;
		}
	}

	return false;
}

//==================================================================
static bool handleComment(
					const char	*pSource,
					size_t		&i,
					size_t		sourceSize,
					bool		&isInComment,
					int			&io_lineCnt )
{
	if ( isInComment )
	{
		if ( pSource[i] == '\n' )
			++io_lineCnt;

		if ( matches( pSource, i, sourceSize, "*/" ) )
		{
			isInComment = false;
			i += 2;
		}
		else
		{
			i += 1;
		}

		return true;
	}
	else
	if ( matches( pSource, i, sourceSize, "/*" ) )
	{
		isInComment = true;
		i += 2;
		return true;
	}

	return false;
}

//==================================================================
static bool isDigit( char ch )
{
	return ch >= '0' && ch <= '9';
}

//==================================================================
static bool handleAlphanum(
					const char	*pSource,
					size_t		&i,
					size_t		sourceSize,
					DVec<Token> &tokens,
					int			lineCnt )
{
	if NOT( isAlphaNumStrFirstChar( pSource[i] ) )
		return false;

	newToken( tokens, lineCnt );
	tokens.back().idType = T_TYPE_NONTERM;
	tokens.back().id	 = T_NONTERM;
	tokens.back().str = pSource[i++];

	for (; i < sourceSize; ++i)
	{
		if ( !isAlphaNum( pSource[i] ) )
			break;

		tokens.back().str += pSource[i];
	}

	return true;
}

//==================================================================
static bool handleNumber(
					const char	*pSource,
					size_t		&i,
					size_t		sourceSize,
					DVec<Token> &tokens,
					int			lineCnt )
{
	char ch		= pSource[i];

	// not the beginning of a number.. ?
	if ( (ch < '0' || ch > '9') && ch != '.' )
		return false;

	char nextCh = (i+1) < sourceSize ? pSource[i+1] : 0;

	// starts with a dot ?
	if ( ch == '.' )	// next must be a digit
		if NOT( isDigit( nextCh ) )
			return false;

	newToken( tokens, lineCnt );

	tokens.back().idType = T_TYPE_VALUE;
	tokens.back().id	 = T_VL_NUMBER;

	char	prevCh = 0;

	bool	gotDot = false;

	for (; i < sourceSize;)
	{
		ch		= pSource[i];
		nextCh	= (i+1) < sourceSize ? pSource[i+1] : 0;

		if ( isDigit( ch ) )
		{
			tokens.back().str += ch;
			++i;
		}
		else
		if ( ch == '.' )
		{
			if ( gotDot )
			{
				// 2+ decimal dots means bad number !
				tokens.back().isBadNumber = true;
				newToken( tokens, lineCnt );
				return true;
			}

			gotDot = true;
			tokens.back().str += ch;
			++i;
		}
		else
		if ( ch == 'f' || ch == 'F' || ch == 'd' || ch == 'D' )	// end of a float or double ?
		{
			tokens.back().str += ch;
			++i;
			newToken( tokens, lineCnt );
			return true;
		}
		else
		if ( ch == 'e' || ch == 'E' )
		{
			tokens.back().str += ch;
			++i;	// take it

			if ( nextCh != 0 )
			{
				tokens.back().str += nextCh;
				++i;	// take the next character if there is any
			}

			// must be +, - or a digit
			if NOT( nextCh == '+' || nextCh == '-' || isDigit( nextCh ) )
			{
				// if not, then it's a bad number !
				tokens.back().isBadNumber = true;
				newToken( tokens, lineCnt );
				return true;
			}
		}
		else
		{
			break;
		}

		prevCh = ch;
	}

	newToken( tokens, lineCnt );
	return true;
}

//==================================================================
static bool handleString(
					const char	*pSource,
					size_t		&i,
					size_t		sourceSize,
					DVec<Token> &tokens,
					int			lineCnt )
{
	if ( pSource[i] == '"' )
	{
		newToken( tokens, lineCnt );
		tokens.back().idType = T_TYPE_VALUE;
		tokens.back().id	 = T_VL_STRING;

		//tokens.back().str = pSource[i++];
		++i;
		for (; i < sourceSize; ++i)
		{
			if ( pSource[i] == '"' )
			{
				++i;
				return true;
			}
			else
				tokens.back().str += pSource[i];
		}

		throw Exception( "String not closing !", &tokens.back() );
	}

	return false;
}

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
void Tokenizer( DVec<Token> &tokens, const char *pSource, size_t sourceSize )
{
	initSortedTable();

	bool	isInComment	= false;

	tokens.grow();

	TokenDef	*pCurTDef = NULL;

	bool wasPrecededByWS = false;

	int	lineCnt = 1;

	for (size_t i=0; i < sourceSize; )
	{
		if ( handleComment( pSource, i, sourceSize, isInComment, lineCnt ) )
			continue;

		if ( findSkipWhites( pSource, i, sourceSize, lineCnt ) )
		{
			wasPrecededByWS = true;
			continue;
		}
		else
			wasPrecededByWS = false;

		if ( handleNumber( pSource, i, sourceSize, tokens, lineCnt ) )
			continue;

		if ( matchTokenDef( tokens, pSource, i, sourceSize, wasPrecededByWS, lineCnt ) )
			continue;

		if ( handleAlphanum( pSource, i, sourceSize, tokens, lineCnt ) )
			continue;

		if ( handleString( pSource, i, sourceSize, tokens, lineCnt ) )
			continue;

		throw Exception( DUT::SSPrintFS( "Invalid character '%c' at line %i !", pSource[i], lineCnt+1 ) );
	}

/*
	// solve ambiguities about variables named like standard functions !
	size_t	n = tokens.size();
	if ( n-- )
	{
		for (size_t i=0; i < n; ++i)
		{
			if ( tokens[i].idType == T_TYPE_STDFUNC &&
				 tokens[i+1].id != T_OP_LFT_BRACKET )
			{
				tokens[i].idType = T_TYPE_NONTERM;
				tokens[i].id	 = T_NONTERM;
			}	
		}
	}
*/
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
