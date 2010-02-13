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
Exception::Exception( const std::string &msg, const Token *pTok/*=NULL */ )
{
	if ( pTok )
	{
		mMsg = DUT::SSPrintFS(
					"%s ...\n...%s -- %i",
					msg.c_str(),
					pTok->pSourceFileName ? pTok->pSourceFileName : "<NO FILE>",
					pTok->sourceLine );
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
		mMsg = DUT::SSPrintFS(
					"%s ...\n...%s -- %i",
					msg.c_str(),
					pTok->pSourceFileName ? pTok->pSourceFileName : "<NO FILE>",
					pTok->sourceLine );
	}
	else
		mMsg = msg;
}

//==================================================================
Exception::Exception( const TokNode *pTokNode, const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[2048];

	vsnprintf_s( buff, sizeof(buff), _countof(buff)-1, pFmt, vl );
	va_end( vl );


	if ( pTokNode && pTokNode->mpToken )
	{
		const Token *pTok = pTokNode->mpToken;
		mMsg = DUT::SSPrintFS(
					"%s ...\n...%s -- %i",
					buff,
					pTok->pSourceFileName ? pTok->pSourceFileName : "<NO FILE>",
					pTok->sourceLine );
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

	mMsg = DUT::SSPrintFS(
					"%s ...\n...%s -- %i",
					buff,
					fatBase.mFileNames[ ch.FNameIdx ].c_str(),
					ch.SrcPos );
}

//==================================================================
}
