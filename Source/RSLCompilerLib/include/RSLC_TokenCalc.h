//==================================================================
/// RSLC_TokenCalc.h
///
/// Created by Davide Pasca - 2010/3/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_TOKENCALC_H
#define RSLC_TOKENCALC_H

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
Token *TokenCalcBiOp(
				TokenID			operTokID,
				const Token		&val1,
				const Token		&val2 );

//==================================================================
}

#endif
