//==================================================================
/// RSLC_Prepro_Utils.cpp
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Exceptions.h"
#include "RSLC_TextUtils.h"
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
void CutVectorInclusive( DVec<Fat8> &vec, size_t start, size_t end )
{
	if ( (end+1) < vec.size() )
		++end;

	CutVector( vec, start, end );
}

//==================================================================
size_t GetAlphaNumBetweenSpaces(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				FatBase		&fatBase,
				DStr	&out_symName )
{
	SkipHWhites( text, i, lineEnd );

	size_t symStart = i;

	for (; i < lineEnd; ++i)
		if ( text[i].Ch == ' ' || text[i].Ch == '\t' )
			break;
	
	size_t symEnd = i;

	out_symName.clear();

	DStr	symbolName;
	for (size_t j=symStart; j < symEnd; ++j)
		out_symName += text[j].Ch;

	if NOT( IsAlphaNumStr( out_symName ) )
		throw Exception(
					fatBase,
					text[i],
					"Expecting an alphanumeric but found '%s'",
					out_symName.c_str() );

	return symEnd;
}

//==================================================================
}
//==================================================================
}
