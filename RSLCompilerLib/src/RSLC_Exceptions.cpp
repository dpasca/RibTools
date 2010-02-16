//==================================================================
/// RSLC_Exceptions.cpp
///
/// Created by Davide Pasca - 2009/5/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdarg.h>
#include "DSystem/include/DUtils.h"
#include "RSLC_Token.h"
#include "RSLC_Tree.h"
#include "RSLC_FatChars.h"
#include "RSLC_Exceptions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void makeMsg(
				std::string &out_msg,
				const char *pFile,
				int line,
				const char *pSrcMsg )
{
	out_msg = DUT::SSPrintFS(
				"SHADER ERR> MSG : %s\n"
				"SHADER ERR> AT  : %s : %i\n",
				pSrcMsg,
				pFile ? pFile : "<NO FILE>", line
				);
}

//==================================================================
Exception::Exception( const std::string &msg, const Token *pTok/*=NULL */ )
{
	if ( pTok )
	{
		makeMsg( mMsg, pTok->pSourceFileName, pTok->sourceLine, msg.c_str() );
	}
	else
		mMsg = msg;
}

//==================================================================
Exception::Exception( const std::string &msg, const TokNode *pTokNode )
{
	if ( pTokNode && pTokNode->mpToken )
	{
		const Token *pTok = pTokNode->mpToken;
		makeMsg( mMsg, pTok->pSourceFileName, pTok->sourceLine, msg.c_str() );
	}
	else
		mMsg = msg;
}

//==================================================================
Exception::Exception( const TokNode *pTokNode, const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[4096];

	vsnprintf_s( buff, sizeof(buff), _countof(buff)-1, pFmt, vl );
	va_end( vl );


	if ( pTokNode && pTokNode->mpToken )
	{
		const Token *pTok = pTokNode->mpToken;
		makeMsg( mMsg, pTok->pSourceFileName, pTok->sourceLine, buff );
	}
	else
	{
		mMsg = buff;
	}
}

//==================================================================
Exception::Exception( const FatBase &fatBase, const Fat8 &ch, const char *pFmt, ...  )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[2048];

	vsnprintf_s( buff, sizeof(buff), _countof(buff)-1, pFmt, vl );
	va_end( vl );

	makeMsg( mMsg, fatBase.mFileNames[ ch.FNameIdx ].c_str(), ch.SrcPos, buff );
}

//==================================================================
}
