//==================================================================
/// RSLC_TextUtils.cpp
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DTypes.h"
#include "RSLC_TextUtils.h"

//==================================================================
namespace RSLC
{

//==================================================================
bool IsAlphaNumStart( char c )
{
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//==================================================================
bool IsAlphaNumBody( char c )
{
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

//==================================================================
bool IsAlphaNumStr( const DStr &str )
{
	if NOT( str.length() )
		return false;

	if NOT( IsAlphaNumStart( str[0] ) )
		return false;

	for (size_t i=1; i < str.length(); ++i)
		if NOT( IsAlphaNumBody( str[i] ) )
			return false;

	return true;
}

//==================================================================
}
