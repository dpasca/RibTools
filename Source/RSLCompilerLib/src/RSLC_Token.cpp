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
#include "RSLC_TextUtils.h"
#include "RSLC_Tokenize.h"
#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
Token::Token() :
	id(TOKEN_N),	// init with a non-plausible value
	idType(T_TYPE_NONTERM),
	isPrecededByWS(false),
	isBadNumber(false),
	pSourceFileName(NULL),
	sourceLine(0),
	ConstNum(0)
{
}

//==================================================================
Token::Token(
		const char	*pStr_,
		TokenID		id_,
		TokenIDType	idType_,
		const Token	*pInheritTokPos_ ) :
	str(pStr_),
	id(id_),
	idType(idType_),
	isPrecededByWS(false),
	isBadNumber(false),
	pSourceFileName( pInheritTokPos_ ? pInheritTokPos_->pSourceFileName : NULL ),
	sourceLine( pInheritTokPos_ ? pInheritTokPos_->sourceLine : 0 ),
	ConstNum(0)
{
}

//==================================================================
void Token::SetAsStringValue( const DStr &srcStr )
{
	id = T_VL_STRING;
	idType = T_TYPE_VALUE;
	str = srcStr;
}
//==================================================================
void Token::SetAsStringValue( const char *srcStr )
{
	id = T_VL_STRING;
	idType = T_TYPE_VALUE;
	str = srcStr;
}

//==================================================================
void Token::SetAsBoolValue( bool onoff )
{
	id = onoff ? T_VL_BOOL_TRUE : T_VL_BOOL_FALSE;
	idType = T_TYPE_VALUE;
	str = onoff ? "true" : "false";
}

//==================================================================
void Token::SetAsNumValue( double num )
{
	id = T_VL_NUMBER;
	idType = T_TYPE_VALUE;

	ConstNum = num;

	char buff[512];
	sprintf_s( buff, "%Lf", ConstNum );
	str = buff;
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
		id == T_OP_DOT			||
		id == T_OP_LFT_SQ_BRACKET;	// array indicization !
}

//==================================================================
void Token::UpdateConstNum()
{
	DASSERT( id == T_VL_NUMBER );
	sscanf_s( str.c_str(), "%Lf", &ConstNum );
}

//==================================================================
void Token::UpdateStrFromValue()
{
	DASSERT( idType == T_TYPE_VALUE );

	if ( id == T_VL_NUMBER )
	{
		char buff[512];
		sprintf_s( buff, "%Lf", ConstNum );
		str = buff;
	}
	else
	if ( id == T_VL_BOOL_TRUE )
	{
		str = "true";
	}
	else
	if ( id == T_VL_BOOL_FALSE )
	{
		str = "false";
	}
	else
	{
		DASSERT( 0 );
	}
}

//==================================================================
static size_t _sTokenDefsIdxInvSortLen[TOKEN_N];

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
void Tokenizer( DVec<Token> &tokens, FatBase &fatBase, const Fat8Vec &str )
{
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
}
