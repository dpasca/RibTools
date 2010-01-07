//==================================================================
/// RSLC_Token.h
///
/// Created by Davide Pasca - 2009/5/23
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_TOKEN_H
#define RSLC_TOKEN_H

#include <string>
#include "DSystem/include/DUtils.h"
#include "DSystem/include/DContainers.h"
#include "RSLC_TokenDefs.h"

//==================================================================
namespace RSLC
{

//==================================================================
enum BlockType
{
	BLKT_UNKNOWN,
	BLKT_ROOT,
	BLKT_SHPARAMS,
	BLKT_FNPARAMS,
	BLKT_CODEBLOCK,
	BLKT_EXPRESSION,
	BLKT_FUNCCALL,
	BLKT_FUNCOPEXPR,
	BLKT_N
};

//==================================================================
class Token
{
public:
	std::string	str;
	TokenID		id;
	TokenIDType	idType;
	bool		isPrecededByWS;
	bool		isBadNumber;
	int			sourceLine;

	Token() :
		id(TOKEN_N),	// init with a non-plausible value
		idType(T_TYPE_NONTERM),
		isPrecededByWS(false),
		isBadNumber(false),
		sourceLine(0)
		{}

	Token( const char *pStr_, TokenID id_, TokenIDType idType_ ) :
		str(pStr_),
		id(id_),
		idType(idType_),
		isPrecededByWS(false),
		isBadNumber(false),
		sourceLine(0)
		{}

	// kind of lame function used during construction..
	bool IsSet() const
	{
		return id != TOKEN_N;
	}

	const char *GetStrChar() const
	{
		return str.c_str();
	}

	bool IsAssignOp() const
	{
		return
			id == T_OP_ASSIGN	||
			id == T_OP_PLUSASS	||
			id == T_OP_MINUSASS	||
			id == T_OP_MULASS	||
			id == T_OP_DIVASS	;
	}

	bool IsBiOp() const;

	bool IsCmpOp() const
	{
		return
			id == T_OP_LSEQ	||
			id == T_OP_GEEQ	||
			id == T_OP_LSTH	||
			id == T_OP_GRTH	||
			id == T_OP_EQ	||
			id == T_OP_NEQ	;
	}
};

//==================================================================
const char *GetTokenTypeStr( TokenIDType tokidtype );
const char *GetTokenIDStr( TokenID tokid );
void Tokenizer( DVec<Token> &tokens, const char *pSource, size_t sourceSize );

Token *TokenFromDefOrNTerm( const char *pTokenStr, int lineCnt=0 );

//==================================================================
}

#endif
