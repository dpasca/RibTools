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
struct TokenDef
{
	const char	*pStr;
	TokenIDType	idType;
	TokenID		id;
};

//==================================================================
#define OP_DEF(_STR_,_X_)	_STR_, T_TYPE_OPERATOR,	T_OP_##_X_
#define DT_DEF(_X_)			#_X_, T_TYPE_DATATYPE,	T_DT_##_X_
#define ST_DEF(_X_)			#_X_, T_TYPE_SHADERTYPE,T_ST_##_X_
#define DE_DEF(_X_)			#_X_, T_TYPE_DETAIL,	T_DE_##_X_
#define KW_DEF(_X_)			#_X_, T_TYPE_KEYWORD,	T_KW_##_X_
#define SF_DEF(_X_)			#_X_, T_TYPE_STDFUNC,	T_SF_##_X_
#define FO_DEF(_X_)			#_X_, T_TYPE_FUNCOP,	T_FO_##_X_

//==================================================================
static TokenDef _sTokenDefs[TOKEN_N] =
{
	NULL	,	T_TYPE_NONTERM	,	T_NONTERM	,

	NULL	,	T_TYPE_TEMPDEST	,	T_TD_TEMPDEST	,

	NULL	,	T_TYPE_VALUE	,	T_VL_NUMBER	,
	NULL	,	T_TYPE_VALUE	,	T_VL_STRING	,

	OP_DEF(	"<="	,	LSEQ		)	,
	OP_DEF(	">="	,	GEEQ		)	,
	OP_DEF(	"<"		,	LSTH		)	,
	OP_DEF(	">"		,	GRTH		)	,
	OP_DEF(	"&&"	,	LOGIC_AND	)	,
	OP_DEF(	"||"	,	LOGIC_OR	)	,
	OP_DEF(	"=="	,	EQ			)	,
	OP_DEF(	"!="	,	NEQ			)	,
	OP_DEF(	"+="	,	PLUSASS		)	,
	OP_DEF(	"-="	,	MINUSASS	)	,
	OP_DEF(	"*="	,	MULASS		)	,
	OP_DEF(	"/="	,	DIVASS		)	,
	OP_DEF(	"+"		,	PLUS		)	,
	OP_DEF(	"-"		,	MINUS		)	,
	OP_DEF(	"*"		,	MUL			)	,
	OP_DEF(	"/"		,	DIV			)	,
	OP_DEF(	"="		,	ASSIGN		)	,
	OP_DEF(	"^"		,	POW			)	,
	OP_DEF(	"."		,	DOT			)	,
	OP_DEF(	":"		,	COLON		)	,
	OP_DEF(	","		,	COMMA		)	,
	OP_DEF(	";"		,	SEMICOL		)	,
	OP_DEF(	"("		,	LFT_BRACKET		)	,
	OP_DEF(	")"		,	RGT_BRACKET		)	,
	OP_DEF(	"["		,	LFT_SQ_BRACKET		)	,
	OP_DEF(	"]"		,	RGT_SQ_BRACKET		)	,
	OP_DEF(	"{"		,	LFT_CRL_BRACKET		)	,
	OP_DEF(	"}"		,	RGT_CRL_BRACKET		)	,
	OP_DEF(	"#"		,	HASH		)	,

	DT_DEF( float			)	,
	DT_DEF( vector			)	,
	DT_DEF( point			)	,
	DT_DEF( normal			)	,
	DT_DEF( color			)	,
	DT_DEF( matrix			)	,
	DT_DEF( string			)	,
	DT_DEF( void			)	,

	DE_DEF( varying			)	,
	DE_DEF( uniform			)	,

	FO_DEF( if				)	,
	FO_DEF( for				)	,
	FO_DEF( while			)	,
	FO_DEF( solar			)	,
	FO_DEF( illuminate		)	,
	FO_DEF( illuminance		)	,

	KW_DEF( break			)	,
	KW_DEF( continue		)	,
	KW_DEF( return			)	,
	KW_DEF( output			)	,

	//KW_DEF( texture			)	,	// texture_type
	//KW_DEF( environment		)	,
	KW_DEF( bump			)	,
	//KW_DEF( shadow			)	,

	ST_DEF( light			)	,	// shader types
	ST_DEF( surface			)	,
	ST_DEF( volume			)	,
	ST_DEF( displacement	)	,
	ST_DEF( transformation	)	,
	ST_DEF( imager			)	,

	SF_DEF( radians				)	,
	SF_DEF( degrees				)	,
	SF_DEF( sin					)	,
	SF_DEF( asin				)	,
	SF_DEF( cos					)	,
	SF_DEF( acos				)	,
	SF_DEF( tan					)	,
	SF_DEF( atan				)	,
	SF_DEF( pow					)	,
	SF_DEF( exp					)	,
	SF_DEF( sqrt				)	,
	SF_DEF( inversesqrt			)	,
	SF_DEF( log					)	,
	SF_DEF( mod					)	,
	SF_DEF( abs					)	,
	SF_DEF( sign				)	,
	SF_DEF( min					)	,
	SF_DEF( max					)	,
	SF_DEF( clamp				)	,
	SF_DEF( mix					)	,
	SF_DEF( floor				)	,
	SF_DEF( ceil				)	,
	SF_DEF( round				)	,
	SF_DEF( step				)	,
	SF_DEF( smoothstep			)	,
	SF_DEF( filterstep			)	,
	SF_DEF( spline				)	,
	SF_DEF( Du					)	,
	SF_DEF( Dv					)	,
	SF_DEF( Deriv				)	,
	SF_DEF( random				)	,
	SF_DEF( noise				)	,
	SF_DEF( pnoise				)	,
	SF_DEF( cellnoise			)	,
	SF_DEF( xcomp				)	,
	SF_DEF( ycomp				)	,
	SF_DEF( zcomp				)	,
	SF_DEF( setxcomp			)	,
	SF_DEF( setycomp			)	,
	SF_DEF( setzcomp			)	,
	SF_DEF( length				)	,
	SF_DEF( normalize			)	,
	SF_DEF( distance			)	,
	SF_DEF( ptlined				)	,
	SF_DEF( area				)	,
	SF_DEF( faceforward			)	,
	SF_DEF( reflect				)	,
	SF_DEF( refract				)	,
	SF_DEF( fresnel				)	,
	SF_DEF( transform			)	,
	SF_DEF( vtransform			)	,
	SF_DEF( ntransform			)	,
	SF_DEF( calculatenormal		)	,
	SF_DEF( comp				)	,
	SF_DEF( setcomp				)	,
	SF_DEF( ctransform			)	,
	SF_DEF( determinant			)	,
	SF_DEF( translate			)	,
	SF_DEF( rotate				)	,
	SF_DEF( scale				)	,
	SF_DEF( concat				)	,
	SF_DEF( printf				)	,
	SF_DEF( format				)	,
	SF_DEF( match				)	,
	SF_DEF( ambient				)	,
	SF_DEF( diffuse				)	,
	SF_DEF( specular			)	,
	SF_DEF( specularbrdf		)	,
	SF_DEF( phong				)	,
	SF_DEF( trace				)	,
	SF_DEF( texture				)	,
	SF_DEF( environment			)	,
	SF_DEF( shadow				)	,
	SF_DEF( textureinfo			)	,
};

//==================================================================
static size_t _sTokenDefsIdxInvSortLen[TOKEN_N];

//==================================================================
#undef OP_DEF
#undef DT_DEF
#undef ST_DEF
#undef DE_DEF
#undef KW_DEF
#undef SF_DEF
#undef FO_DEF

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
	if ( tokens.back().str.size() )
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

//==================================================================
Token *TokenFromDefOrNTerm( const char *pTokenStr, int lineCnt )
{
	for (size_t j=T_VL_STRING+1; j < TOKEN_N; ++j)
	{
		const TokenDef &tokDef = _sTokenDefs[ _sTokenDefsIdxInvSortLen[ j ] ];

		if ( tokDef.pStr && 0 == strcmp( pTokenStr, tokDef.pStr ) )
			return DNEW Token( tokDef.pStr, tokDef.id, tokDef.idType );
	}

	return DNEW Token( pTokenStr, T_NONTERM, T_TYPE_NONTERM );
}


//==================================================================
static bool matchTokenDef(
					DVec<Token> &tokens,
					const char *pSource,
					size_t &i,
					size_t sourceSize,
					bool wasPrecededByWS,
					int lineCnt )
{
	for (size_t j=T_VL_STRING+1; j < TOKEN_N; ++j)
	{
		const TokenDef &tokDef = _sTokenDefs[ _sTokenDefsIdxInvSortLen[ j ] ];

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
	case T_TYPE_STDFUNC:	return "Std Func";

	default:
		return "SYMBOL NOT FOUND ?!!";
	}
}

//==================================================================
const char *GetTokenIDStr( TokenID tokid )
{
	for (size_t i=0; i < TOKEN_N; ++i)
	{
		if ( _sTokenDefs[i].id == tokid )
			return _sTokenDefs[i].pStr ? _sTokenDefs[i].pStr : "N/A";
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
		sortItems[i].strLen = _sTokenDefs[i].pStr ? strlen( _sTokenDefs[i].pStr ) : 0;
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
}

//==================================================================
bool Token::IsBiOp() const
{
	return
		IsAssignOp()			||
		id == T_OP_PLUS			||
		id == T_OP_MINUS		||
		id == T_OP_MUL			||
		id == T_OP_DIV			||
		id == T_OP_ASSIGN		||
		id == T_OP_POW			||
		id == T_OP_DOT			||
		id == T_OP_LSEQ			||
		id == T_OP_GEEQ			||
		id == T_OP_LSTH			||
		id == T_OP_GRTH			||
		id == T_OP_LOGIC_AND	||
		id == T_OP_LOGIC_OR		||
		id == T_OP_EQ			||
		id == T_OP_NEQ			;
}

//==================================================================
}
