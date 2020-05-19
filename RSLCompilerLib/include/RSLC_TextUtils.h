//==================================================================
/// RSLC_TextUtils.h
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_TEXTUTILS_H
#define RSLC_TEXTUTILS_H

#include "DSystem/include/DStr.h"

//==================================================================
namespace RSLC
{

//==================================================================
bool IsAlphaNumStart( char c );
bool IsAlphaNumBody( char c );
bool IsAlphaNumStr( const DStr &str );

//==================================================================
}

#endif
