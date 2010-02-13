//==================================================================
/// RSLC_Prepro.cpp
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro.h"
#include "RSLC_Exceptions.h"

#define LF	0x0a
#define CR	0x0d

//==================================================================
namespace RSLC
{

//==================================================================
static bool matches( const DVec<Fat8> &str, size_t end, size_t i, const char *pFindStr )
{
	for (; i < end && *pFindStr; ++i)
		if ( str[i].Ch != *pFindStr++ )
			return false;

	return true;
}

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
			if ( matches( text, end, i, "*/" ) )
			{
				i += 2;
				isBlockComment = false;
			}
			else
				continue;
		}
		else
		if ( matches( text, end, i, "//" ) )
		{
			i += 2;
			isLineComment = true;
			continue;
		}
		else
		if ( matches( text, end, i, "/*" ) )
		{
			i += 2;
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
	// remove whitespaces before the beginning of each line
	size_t	j = start;
	for (size_t i=start; i < end; ++i)
	{
		for (; i < end; ++i)
			if ( text[i].Ch != ' ' && text[i].Ch != '\t' )
				break;

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
static size_t cleanChunk( DVec<Fat8> &text, size_t start=0, size_t end=NPOS )
{
	if ( end == NPOS )
		end = text.size();

	size_t newEnd;
	newEnd = spliceLines( text, start, end );
	newEnd = stripComments( text, start, newEnd );
	newEnd = optimizeWhitespaces( text, start, newEnd );

	return newEnd;
}

//==================================================================
static bool findAdvance( const DVec<Fat8> &text, size_t &io_from, size_t toEnd, const char *pFindStr )
{
	size_t i = io_from;

	for (; i < toEnd; ++i)
		if ( text[i].Ch != ' ' && text[i].Ch != '\t' )
			break;

	size_t	findIdx = 0;
	for (; i < toEnd && pFindStr[findIdx]; ++i, ++findIdx)
	{
		if ( text[i].Ch != pFindStr[findIdx] )
			break;
	}

	if ( pFindStr[findIdx] == 0 )
	{
		io_from = i;
		return true;
	}
	else
		return false;
}

//==================================================================
static bool findAdvanceChar( const DVec<Fat8> &text, size_t &io_from, size_t toEnd, const char *pChars )
{
	size_t i = io_from;

	for (; i < toEnd; ++i)
		if ( text[i].Ch != ' ' && text[i].Ch != '\t' )
			break;

	for (size_t j=0; pChars[j]; ++j)
		if ( text[i].Ch == pChars[j] )
		{
			io_from = i;
			return true;
		}

	return false;
}

//==================================================================
template <class _T>
void CutVector( DVec<_T> &vec, size_t start, size_t end )
{
	DASSERT( start <= end );

	for (size_t i=end; i < vec.size(); ++i)
		vec[start+i-end] = vec[i];

	vec.resize( vec.size() - (end - start) );
}

//==================================================================
static void handleInclude(
				DIO::FileManagerBase	&fmanager,
				FatBase					&fatBase,
				DVec<Fat8>				&text,
				size_t					&i,
				size_t					lineEnd,
				size_t					includePoint )
{
	if NOT( findAdvanceChar( text, i, lineEnd, "\"<" ) )
	{
		throw Exception( fatBase, text[i], "No parameter for 'include' ?" );
	}

	U8	closingSymbol;
	if ( text[i].Ch == '"' )
		closingSymbol = '"';
	else
		closingSymbol = '>';

	size_t nameStartIdx = i + 1;

	DStr	includeName;

	for (++i; i < lineEnd; ++i)
	{
		if ( text[i].Ch == closingSymbol )
		{
			DVec<U8> incFileData;
			fmanager.GrabFile( includeName.c_str(), incFileData );
			size_t insertEnd =
				fatBase.InsertNewFile(
						text,
						includePoint,
						lineEnd,
						includeName.c_str(),
						&incFileData[0],
						incFileData.size() );

			size_t newEnd = cleanChunk( text, includePoint, insertEnd );
			DASSERT( newEnd <= insertEnd );

			CutVector( text, newEnd, insertEnd );

			return;
		}

		includeName += (char)text[i].Ch;
	}

	throw Exception( fatBase, text[nameStartIdx], "No closing symbol for 'include'" );
}

//==================================================================
static void processFile(
				DIO::FileManagerBase	&fmanager,
				FatBase					&fatBase,
				DVec<Fat8>				&text,
				const char				*pIncFileName )
{
	for (size_t i=0; i < text.size();)
	{
		size_t lineEnd = text.size();
		for (size_t j=i; j < lineEnd; ++j)
		{
			if ( text[j].Ch == LF )
			{
				lineEnd = j;
				break;
			}
		}

		if ( text[i].Ch == '#' )
		{
			size_t	includePoint = i;

			++i;
			if ( findAdvance( text, i, lineEnd, "include" ) )
			{
				handleInclude( fmanager, fatBase, text, i, lineEnd, includePoint );

				i = includePoint;
				continue;
			}
		}

		i = lineEnd+1;
	}
}

//==================================================================
Prepro::Prepro(
		DIO::FileManagerBase	&fmanager,
		FatBase					&fatBase,
		const DVec<Fat8>		&inText,
		const char				*pBaseInclude,
		DVec<Fat8>				&out_text )
{
	out_text.clear();

	char defaultInclude[1024];

	sprintf_s( defaultInclude, "#include <%s>\012", pBaseInclude );
	fatBase.AppendNewFile(
					out_text,
					"Base Include",
					(const U8 *)defaultInclude,
					strlen( defaultInclude ) );

	fatBase.Append( out_text, inText );

	size_t newEnd = cleanChunk( out_text );
	out_text.resize( newEnd );

	processFile( fmanager, fatBase, out_text, pBaseInclude );
}

//==================================================================
}
