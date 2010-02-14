//==================================================================
/// RSLC_Prepro_Utils.cpp
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro_Utils.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
bool MatchesAdvance( const DVec<Fat8> &str, size_t &io_i, size_t end, const char *pFindStr )
{
	size_t ii = io_i;

	for (; ii < end && *pFindStr; ++ii)
		if ( str[ii].Ch != *pFindStr++ )
			return false;

	io_i = ii;
	return true;
}

//==================================================================
void SkipHWhites( const DVec<Fat8> &text, size_t &i, size_t toEnd )
{
	for (; i < toEnd; ++i)
		if ( text[i].Ch != ' ' && text[i].Ch != '\t' )
			break;
}

//==================================================================
}
//==================================================================
}
