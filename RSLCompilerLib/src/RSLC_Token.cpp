//==================================================================
/// RSLC_Token.cpp
///
/// Created by Davide Pasca - 2009/5/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
struct TokenDef
{
	const char	*pStr;
	TokenType	id;
};

//==================================================================
static TokenDef _sTokenDefs[T_N] =
{
	NULL	,	T_UNKNOWN	,
	"<="	,	T_LSEQ		,
	">="	,	T_GEEQ		,
	"<"		,	T_LSTH		,
	">"		,	T_GRTH		,
	"+="	,	T_PLUSEQ	,
	"-="	,	T_MINUSEQ	,
	"*="	,	T_MULEQ		,
	"/="	,	T_DIVEQ		,
	"+"		,	T_PLUS		,
	"-"		,	T_MINUS		,
	"*"		,	T_MUL		,
	"/"		,	T_DIV		,
	"^"		,	T_POW		,
	"."		,	T_DOT		,
	":"		,	T_COLON		,
	","		,	T_COMMA		,
	";"		,	T_SEMICOL	,
	"("		,	T_LBRARND	,
	")"		,	T_RBRARND	,
	"["		,	T_LBRASQA	,
	"]"		,	T_RBRASQA	,
	"{"		,	T_LBRACRL	,
	"}"		,	T_RBRACRL	,
};

//==================================================================
static bool matches( const char *pStr, size_t i, size_t strSize, const char *pFindStr )
{
	for (; i < strSize && *pFindStr; ++i)
		if ( pStr[i] != *pFindStr++ )
			return false;

	return true;
}

//==================================================================
static bool findSkipWhites( const char *pStr, size_t &i, size_t strSize )
{
	char	ch = pStr[i];

	if ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' )
	{
		do {
			ch = pStr[ ++i ];
		} while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f' );

		return true;
	}

	return false;
}

//==================================================================
static void newToken( DVec<Token> &tokens )
{
	if ( tokens.back().str.size() )
		tokens.grow();
}

//==================================================================
static bool matchTokenDef( DVec<Token> &tokens, const char *pSource, size_t &i, size_t sourceSize, bool wasPrecededByWS )
{
	for (size_t j=1; j < T_N; ++j)
	{
		if ( matches( pSource, i, sourceSize, _sTokenDefs[j].pStr ) )
		{
			newToken( tokens );
			tokens.back().str	= _sTokenDefs[j].pStr;
			tokens.back().id	= _sTokenDefs[j].id;
			i += strlen( _sTokenDefs[j].pStr );
			if ( wasPrecededByWS )
			{
				tokens.back().isPrecededByWS = wasPrecededByWS;
			}
			newToken( tokens );
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
					bool		&isInComment )
{
	if ( isInComment )
	{
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
void Tokenizer( DVec<Token> &tokens, const char *pSource, size_t sourceSize )
{
	bool	isEscape	= false;
	bool	isInString	= false;
	bool	isInComment	= false;

	tokens.grow();

	TokenDef	*pCurTDef = NULL;

	bool wasPrecededByWS = false;

	for (size_t i=0; i < sourceSize; )
	{
		if ( handleComment( pSource, i, sourceSize, isInComment ) )
			continue;

		char	ch = pSource[i];

		if ( ch == '"' )
		{
			wasPrecededByWS = false;

			if ( isInString )
			{
				if ( isEscape )
				{
					tokens.back().str += ch;
					++i;
				}
				else
				{
					isInString = false;
					tokens.back().str += ch;
					++i;
					newToken( tokens );
				}
			}
			else
			{
				isInString = true;
				newToken( tokens );
				tokens.back().str += ch;
				++i;
			}
		}
		else
		{
			if ( findSkipWhites( pSource, i, sourceSize ) )
			{
				wasPrecededByWS = true;
				newToken( tokens );
			}
			else
			{
				if NOT( matchTokenDef( tokens, pSource, i, sourceSize, wasPrecededByWS ) )
				{
					tokens.back().str += ch;
					++i;
				}

				wasPrecededByWS = false;
			}
		}
	}
}


//==================================================================
}
