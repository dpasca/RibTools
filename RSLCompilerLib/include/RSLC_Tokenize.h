//==================================================================
/// RSLC_Tokenize.h
///
/// Created by Davide Pasca - 2010/3/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_TOKENIZE_H
#define RSLC_TOKENIZE_H

#include "RSLC_Token.h"
#include "RSLC_FatChars.h"

//==================================================================
namespace RSLC
{


//==================================================================
/// Tokenize
//==================================================================
class Tokenize
{
public:
	const Fat8Vec	&mStr;
	DVec<Token>		&mTokens;
	FatBase			&mFatBase;

	//==================================================================
	Tokenize( const Fat8Vec &str, DVec<Token> &tokens, FatBase &fatBase );

	void newToken( const Fat8 &fatCh );
	bool matchTokenDef( size_t &i, bool wasPrecededByWS );
	bool handleAlphanum( size_t &i );
	bool handleNumber( size_t &i );
	bool handleString( size_t &i );

//==================================================================
};


//=============================================================================
}

#endif
