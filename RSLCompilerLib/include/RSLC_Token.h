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

//==================================================================
namespace RSLC
{

//==================================================================
enum TokenIDType
{
	T_TYPE_NONTERM		,

	T_TYPE_TEMPDEST		,

	T_TYPE_VALUE		,

	T_TYPE_OPERATOR		,
	T_TYPE_DATATYPE		,
	T_TYPE_SHADERTYPE	,
	T_TYPE_DETAIL		,
	T_TYPE_KEYWORD		,
	T_TYPE_STDFUNC		,

	T_TYPE_FUNCOP		,

	T_TYPE_N
};

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
#define OP_DEF(_DUMMY_,_X_)	T_OP_##_X_
#define DT_DEF(_X_)			T_DT_##_X_
#define ST_DEF(_X_)			T_ST_##_X_
#define DE_DEF(_X_)			T_DE_##_X_
#define KW_DEF(_X_)			T_KW_##_X_
//#define SF_DEF(_X_)			T_SF_##_X_
#define FO_DEF(_X_)			T_FO_##_X_

//==================================================================
enum TokenID
{
	T_NONTERM		,

	T_TD_TEMPDEST	,

	T_VL_NUMBER,
	T_VL_STRING,

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
	OP_DEF(	"("		,	LFT_BRACKET			)	,
	OP_DEF(	")"		,	RGT_BRACKET			)	,
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
	DT_DEF( __funcop		)	,

	DE_DEF( varying			)	,
	DE_DEF( uniform			)	,

	FO_DEF( if				)	,
	FO_DEF( for				)	,
	FO_DEF( while			)	,

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

	TOKEN_N
};

//==================================================================
#undef OP_DEF
#undef DT_DEF
#undef ST_DEF
#undef DE_DEF
#undef KW_DEF
//#undef SF_DEF
#undef FO_DEF

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
		id(T_NONTERM),
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
