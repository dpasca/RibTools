//==================================================================
/// RSLCompiler.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DContainers.h"
#include "RSLCompiler.h"

//==================================================================
enum TokenType
{
	T_UNKNOWN	,
	T_LSEQ		,
	T_GEEQ		,
	T_LSTH		,
	T_GRTH		,
	T_PLUSEQ	,
	T_MINUSEQ	,
	T_MULEQ		,
	T_DIVEQ		,
	T_PLUS		,
	T_MINUS		,
	T_MUL		,
	T_DIV		,
	T_COLON		,
	T_COMMA		,
	T_SEMICOL	,
	T_LBRARND	,
	T_RBRARND	,
	T_LBRASQA	,
	T_RBRASQA	,
	T_LBRACRL	,
	T_RBRACRL	,
	T_N
};

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
struct Token
{
	std::string	str;
	TokenType	id;

	Token() : id(T_UNKNOWN) {}
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
static bool matchTokenDef( DVec<Token> &tokens, const char *pSource, size_t &i, size_t sourceSize )
{
	for (size_t j=1; j < T_N; ++j)
	{
		if ( matches( pSource, i, sourceSize, _sTokenDefs[j].pStr ) )
		{
			newToken( tokens );
			tokens.back().str	= _sTokenDefs[j].pStr;
			tokens.back().id	= _sTokenDefs[j].id;
			i += strlen( _sTokenDefs[j].pStr );
			newToken( tokens );
			return true;
		}
	}

	return false;
}

//==================================================================
static void tokenize( DVec<Token> &tokens, const char *pSource, size_t sourceSize )
{
	bool	isEscape	= false;
	bool	isInString	= false;

	tokens.grow();

	TokenDef	*pCurTDef = NULL;

	for (size_t i=0; i < sourceSize; )
	{
		char	ch = pSource[i];

		if ( ch == '"' )
		{
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
				// create a new token if the current one has any
				// content at all
				newToken( tokens );
			}
			else
			if NOT( matchTokenDef( tokens, pSource, i, sourceSize ) )
			{
				tokens.back().str += ch;
				++i;
			}
		}
	}
}

//==================================================================
RSLCompiler::RSLCompiler( const char *pSource, size_t sourceSize )
{
	DVec<Token>	tokens;

	tokenize( tokens, pSource, sourceSize );

	for (size_t i=0; i < tokens.size(); ++i)
	{
		printf( "%3i) %s\n", i, tokens[i].str.c_str() );
	}
}

//==================================================================
RSLCompiler::~RSLCompiler()
{

}
