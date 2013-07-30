//==================================================================
/// RSLC_Prepro_ClearChunk.cpp
///
/// Created by Davide Pasca - 2010/2/14
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro_Utils.h"
#include "RSLC_Prepro_ClearChunk.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
// relies on newlines being unix-stile here
static size_t spliceLines( DVec<Fat8> &text, size_t start, size_t end )
{
	size_t	j = start;
	for (size_t i=start; i < end; ++i)
	{
		bool	splice = false;

		if ( text[i].Ch == '\\' )
		{
			// look for a newline and skip trailing whitespaces
			for (size_t i2=i+1; i2 < end; ++i2)
			{
				if ( text[i2].Ch != ' ' && text[i2].Ch != '\t' )
				{
					// found newline or file end.. so stitch
					if ( text[i2].Ch == LF || (i2+1) == end )
					{	// we simply set scan to continue after the newline or file-end
						splice = true;
						i = i2;
					}
					else
						i = i2-1;
		
					break;
				}
			}
		}

		if NOT( splice )
			text[ j++ ] = text[ i ];
	}

	return j;
}

//==================================================================
static size_t stripComments( DVec<Fat8> &text, size_t start, size_t end )
{
	bool	isLineComment = false;
	bool	isBlockComment = false;

	size_t	j = start;
	for (size_t i=start; i < end; ++i)
	{
		if ( isLineComment )
		{
			if ( text[i].Ch == '\n' )
				isLineComment = false;
			else
				continue;
		}
		else
		if ( isBlockComment )
		{
			if ( MatchesAdvance( text, i, end, "*/" ) )
			{
				isBlockComment = false;
			}
			else
				continue;
		}
		else
		if ( MatchesAdvance( text, i, end, "//" ) )
		{
			isLineComment = true;
			continue;
		}
		else
		if ( MatchesAdvance( text, i, end, "/*" ) )
		{
			isBlockComment = true;
			text[ j++ ].Ch = LF;	// add a whitespace in place of a whole comment
			continue;
		}

		text[ j++ ] = text[ i ];
	}

	return j;
}

//==================================================================
static size_t optimizeWhitespaces( DVec<Fat8> &text, size_t start, size_t end )
{
	// remove white-spaces before the beginning of each line
	size_t	j = start;
	for (size_t i=start; i < end; ++i)
	{
		SkipHWhites( text, i, end );

		for (; i < end; ++i)
		{
			text[j++] = text[i];

			if ( text[i].Ch == LF )
				break;
		}
	}
	end = j;

	// convert multiple newlines to a single newline
	j = start;
	for (size_t i=start; i < end; ++i)
	{
		if ( text[i].Ch == LF )
		{
			text[ j++ ] = text[ i ];

			for (; i < end; ++i)
				if ( text[i].Ch != LF )
				{
					text[ j++ ] = text[ i ];
					break;
				}
		}
		else
			text[ j++ ] = text[ i ];
	}

	return j;
}

//==================================================================
size_t ClearChunk( DVec<Fat8> &text, size_t start, size_t end )
{
	if ( end == DNPOS )
		end = text.size();

	size_t newEnd;
	newEnd = spliceLines( text, start, end );
	newEnd = stripComments( text, start, newEnd );
	newEnd = optimizeWhitespaces( text, start, newEnd );

	return newEnd;
}


//==================================================================
}
//==================================================================
}
