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
	T_POW		,
	T_DOT		,
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
struct Token
{
	std::string	str;
	TokenType	id;
	bool		isPrecededByWS;

	Token() : id(T_UNKNOWN), isPrecededByWS(false) {}
};

//==================================================================
void Tokenizer( DVec<Token> &tokens, const char *pSource, size_t sourceSize );

//==================================================================
}

#endif
