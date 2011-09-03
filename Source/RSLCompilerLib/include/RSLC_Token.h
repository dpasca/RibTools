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
#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{

//==================================================================
enum BlockType
{
	BLKT_UNKNOWN,
	BLKT_ROOT,
	BLKT_DECL_PARAMS_SH,
	BLKT_DECL_PARAMS_FN,
	BLKT_CALL_PARAMS_FN,
	BLKT_CALL_OR_DELC_PARAMS_FN,
	BLKT_CODEBLOCK,
	BLKT_EXPRESSION,
	BLKT_N
};

//==================================================================
class Token
{
public:
	DStr		str;
	TokenID		id;
	TokenIDType	idType;
	bool		isPrecededByWS;
	bool		isBadNumber;
	const char	*pSourceFileName;
	int			sourceLine;

	double		ConstNum;	// valid only if id == T_VL_NUMBER

	Token();

	Token(
			const char	*pStr_,
			TokenID		id_,
			TokenIDType	idType_,
			const Token	*pInheritTokPos_=NULL );

	void SetAsStringValue( const char *srcStr );
	void SetAsStringValue( const DStr &srcStr );
	void SetAsBoolValue( bool onoff );
	void SetAsNumValue( double num );

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

	void UpdateConstNum();
	void UpdateStrFromValue();
};

//==================================================================
const char *GetTokenTypeStr( TokenIDType tokidtype );
const char *GetTokenIDStr( TokenID tokid );
void Tokenizer( DVec<Token> &tokens, FatBase &fatBase, const Fat8Vec &str );

Token *TokenFromDefOrNTerm( const char *pTokenStr, int lineCnt=0 );

//==================================================================
}

#endif
