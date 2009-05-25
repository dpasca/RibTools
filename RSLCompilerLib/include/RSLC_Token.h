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
#include "DSystem/include/DContainers.h"

//==================================================================
namespace RSLC
{

//==================================================================
enum TokenIDType
{
	T_TYPE_UNKNOWN		,

	T_TYPE_VALUE		,

	T_TYPE_OPERATOR		,
	T_TYPE_DATATYPE		,
	T_TYPE_SHADERTYPE	,
	T_TYPE_DETAIL		,
	T_TYPE_KEYWORD		,
	T_TYPE_STDFUNC		,

	T_TYPE_N
};

//==================================================================
enum AreaType
{
	AT_UNKNOWN,
	AT_SHPARAMS,
	AT_CODEBLOCK,
	AT_EXPRESSION,
	AT_N
};

//==================================================================
#define OP_DEF(_DUMMY_,_X_)	T_OP_##_X_
#define DT_DEF(_X_)			T_DT_##_X_
#define ST_DEF(_X_)			T_ST_##_X_
#define DE_DEF(_X_)			T_DE_##_X_
#define KW_DEF(_X_)			T_KW_##_X_
#define SF_DEF(_X_)			T_SF_##_X_

//==================================================================
enum TokenID
{
	T_UNKNOWN		,

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
	OP_DEF(	"+="	,	PLUSEQ		)	,
	OP_DEF(	"-="	,	MINUSEQ		)	,
	OP_DEF(	"*="	,	MULEQ		)	,
	OP_DEF(	"/="	,	DIVEQ		)	,
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

	DT_DEF( float			)	,
	DT_DEF( vector			)	,
	DT_DEF( point			)	,
	DT_DEF( normal			)	,
	DT_DEF( color			)	,
	DT_DEF( string			)	,

	DE_DEF( varying			)	,
	DE_DEF( uniform			)	,

	KW_DEF( if				)	,
	KW_DEF( for				)	,
	KW_DEF( while			)	,
	KW_DEF( solar			)	,
	KW_DEF( illuminate		)	,
	KW_DEF( illuminance		)	,
	KW_DEF( break			)	,
	KW_DEF( continue		)	,
	KW_DEF( return			)	,

	KW_DEF( texture			)	,	// texture_type
	KW_DEF( environment		)	,
	KW_DEF( bump			)	,
	KW_DEF( shadow			)	,

	ST_DEF( light			)	,	// shader types
	ST_DEF( surface			)	,
	ST_DEF( volume			)	,
	ST_DEF( displacement	)	,
	ST_DEF( transformation	)	,
	ST_DEF( imager			)	,

	SF_DEF( abs				)	,
	SF_DEF( ambient			)	,
	SF_DEF( calculatenormal	)	,
	SF_DEF( cellnoise		)	,
	SF_DEF( clamp			)	,
	SF_DEF( comp			)	,
	SF_DEF( concat			)	,
	SF_DEF( ctransform		)	,
	SF_DEF( determinant		)	,
	SF_DEF( diffuse			)	,
	SF_DEF( environment		)	,
	SF_DEF( faceforward		)	,
	SF_DEF( filterstep		)	,
	SF_DEF( format			)	,
	SF_DEF( inversesqrt		)	,
	SF_DEF( match			)	,
	SF_DEF( max				)	,
	SF_DEF( min				)	,
	SF_DEF( mix				)	,
	SF_DEF( noise			)	,
	SF_DEF( normalize		)	,
	SF_DEF( ntransform		)	,
	SF_DEF( pnoise			)	,
	SF_DEF( ptlined			)	,
	SF_DEF( random			)	,
	SF_DEF( reflect			)	,
	SF_DEF( rotate			)	,
	SF_DEF( scale			)	,
	SF_DEF( setcomp			)	,
	SF_DEF( shadow			)	,
	SF_DEF( specular		)	,
	SF_DEF( specularbrdf	)	,
	SF_DEF( spline			)	,
	SF_DEF( sqrt			)	,
	SF_DEF( texture			)	,
	SF_DEF( transform		)	,
	SF_DEF( translate		)	,
	SF_DEF( vtransform		)	,

	TOKEN_N
};

//==================================================================
#undef OP_DEF
#undef DT_DEF
#undef ST_DEF
#undef DE_DEF
#undef KW_DEF
#undef SF_DEF

//==================================================================
struct Token
{
	std::string	str;
	TokenID		id;
	TokenIDType	idType;
	bool		isPrecededByWS;
	bool		isBadNumber;
	int			sourceLine;

	Token() :
		id(T_UNKNOWN),
		idType(T_TYPE_UNKNOWN),
		isPrecededByWS(false),
		isBadNumber(false),
		sourceLine(0)
		{}
};

//==================================================================
const char *GetTokenTypeStr( TokenIDType tokidtype );
const char *GetTokenIDStr( TokenID tokid );
void Tokenizer( DVec<Token> &tokens, const char *pSource, size_t sourceSize );

//==================================================================
}

#endif
