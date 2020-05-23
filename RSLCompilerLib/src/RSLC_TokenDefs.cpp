//==================================================================
/// RSLC_TokenDefs.cpp
///
/// Created by Davide Pasca - 2010/1/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdlib.h>
#include "RSLC_Exceptions.h"
#include "RSLC_TokenDefs.h"

//==================================================================
namespace RSLC
{

//==================================================================
#define OP_DEF(_STR_,_X_)	_STR_, T_TYPE_OPERATOR,	T_OP_##_X_
#define DT_DEF(_X_)			#_X_, T_TYPE_DATATYPE,	T_DT_##_X_
#define ST_DEF(_X_)			#_X_, T_TYPE_SHADERTYPE,T_ST_##_X_
#define DE_DEF(_X_)			#_X_, T_TYPE_DETAIL,	T_DE_##_X_
#define KW_DEF(_X_)			#_X_, T_TYPE_KEYWORD,	T_KW_##_X_
#define FO_DEF(_X_)			#_X_, T_TYPE_FUNCOP,	T_FO_##_X_

//==================================================================
TokenDef _TokenDefs[TOKEN_N] =
{
    NULL	,	T_TYPE_NONTERM	,	T_NONTERM	,

    NULL	,	T_TYPE_TEMPDEST	,	T_TD_TEMPDEST	,

    NULL	,	T_TYPE_VALUE	,	T_VL_NUMBER	,
    NULL	,	T_TYPE_VALUE	,	T_VL_STRING	,

    "true"	,	T_TYPE_VALUE	,	T_VL_BOOL_TRUE	,
    "false"	,	T_TYPE_VALUE	,	T_VL_BOOL_FALSE	,

    NULL	,	T_TYPE_OPERATOR,	T_OP_ASSIGN_ARR_ITEM,	// operator used internally

    OP_DEF(	"="		,	ASSIGN		)	,	// assign ops
    OP_DEF(	"+="	,	PLUSASS		)	,
    OP_DEF(	"-="	,	MINUSASS	)	,
    OP_DEF(	"*="	,	MULASS		)	,
    OP_DEF(	"/="	,	DIVASS		)	,

    OP_DEF(	"<="	,	LSEQ		)	,	// cmp ops
    OP_DEF(	">="	,	GEEQ		)	,
    OP_DEF(	"<"		,	LSTH		)	,
    OP_DEF(	">"		,	GRTH		)	,
    OP_DEF(	"=="	,	EQ			)	,
    OP_DEF(	"!="	,	NEQ			)	,

    OP_DEF(	"&&"	,	LOGIC_AND	)	,	// general bi-ops
    OP_DEF(	"||"	,	LOGIC_OR	)	,
    OP_DEF(	"+"		,	PLUS		)	,
    OP_DEF(	"-"		,	MINUS		)	,
    OP_DEF(	"*"		,	MUL			)	,
    OP_DEF(	"/"		,	DIV			)	,
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
    DT_DEF( bool			)	,
    DT_DEF( void			)	,

    DE_DEF( varying			)	,
    DE_DEF( uniform			)	,

    //FO_DEF( if				)	,
    FO_DEF( for				)	,
    FO_DEF( while			)	,

    KW_DEF( break			)	,
    KW_DEF( continue		)	,
    KW_DEF( return			)	,
    KW_DEF( output			)	,
    KW_DEF( __funcop		)	,

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
}
