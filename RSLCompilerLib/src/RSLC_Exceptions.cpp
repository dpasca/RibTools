//==================================================================
/// RSLC_Exceptions.cpp
///
/// Created by Davide Pasca - 2009/5/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DUtils.h"
#include "RSLC_Token.h"
#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"

//==================================================================
namespace RSLC
{

//==================================================================
Exception::Exception( const std::string &msg, const Token *pTok/*=NULL */ )
{
	if ( pTok )
	{
		mMsg = DUT::SSPrintFS( "%s at line %i", msg.c_str(), pTok->sourceLine );
	}
	else
		mMsg = msg;
}

//==================================================================
Exception::Exception( const std::string &msg, const TokNode *pTokNode )
{
	if ( pTokNode && pTokNode->mpToken )
	{
		mMsg = DUT::SSPrintFS( "%s at line %i", msg.c_str(), pTokNode->mpToken->sourceLine );
	}
	else
		mMsg = msg;
}

//==================================================================
}
