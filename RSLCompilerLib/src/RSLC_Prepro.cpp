//==================================================================
/// RSLC_Prepro.cpp
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <hash_map>
#include "RSLC_Prepro.h"
#include "RSLC_Exceptions.h"

#define LF	0x0a
#define CR	0x0d

//==================================================================
namespace RSLC
{

typedef stdext::hash_map<std::string,std::string>	SymbolsMap;

//==================================================================
static bool matchesAdvance( const DVec<Fat8> &str, size_t &io_i, size_t end, const char *pFindStr )
{
	size_t ii = io_i;

	for (; ii < end && *pFindStr; ++ii)
		if ( str[ii].Ch != *pFindStr++ )
			return false;

	io_i = ii;
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
			if ( matchesAdvance( text, i, end, "*/" ) )
			{
				isBlockComment = false;
			}
			else
				continue;
		}
		else
		if ( matchesAdvance( text, i, end, "//" ) )
		{
			isLineComment = true;
			continue;
		}
		else
		if ( matchesAdvance( text, i, end, "/*" ) )
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
static void skipHWhites( const DVec<Fat8> &text, size_t &i, size_t toEnd )
{
	for (; i < toEnd; ++i)
		if ( text[i].Ch != ' ' && text[i].Ch != '\t' )
			break;
}

//==================================================================
static size_t optimizeWhitespaces( DVec<Fat8> &text, size_t start, size_t end )
{
	// remove whitespaces before the beginning of each line
	size_t	j = start;
	for (size_t i=start; i < end; ++i)
	{
		skipHWhites( text, i, end );

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

	skipHWhites( text, i, toEnd );

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
template <class _T>
void ReplaceSubVector( DVec<_T> &vec, size_t start, size_t end, const DVec<_T> &newSpan )
{
	// VEC  : -----------|start------------|end-----------|vec.size()
	// EXPA :            |newSpan-------------|newSpan.size()
	// CONTR:            |newSpan--------|newSpan.size()

	DASSERT( start <= end );

	size_t		oldSpanLen = end - start;

	ptrdiff_t	oldToNewSizeDiff = (ptrdiff_t)newSpan.size() - (ptrdiff_t)oldSpanLen;

	if ( oldToNewSizeDiff > 0 )
	{
		// expand
		size_t	oldVecSize = vec.size();

		vec.resize( oldVecSize + oldToNewSizeDiff );	// enlarge the vec

		// move the elements down to make space
		for (size_t i=oldVecSize; i > end; --i)
			vec[i-1+oldToNewSizeDiff] = vec[i-1];
	}
	else
	if ( oldToNewSizeDiff < 0 )
	{
		// contract
		for (size_t i=end; i < vec.size(); ++i)
			vec[i+oldToNewSizeDiff] = vec[i];

		vec.resize( vec.size() + oldToNewSizeDiff );
	}

	// finally copy the new span
	for (size_t i=0; i < newSpan.size(); ++i)
		vec[start+i] = newSpan[i];
}

//==================================================================
static void handleInclude(
				DVec<Fat8>				&text,
				size_t					i,
				size_t					lineEnd,
				size_t					includePoint,
				DIO::FileManagerBase	&fmanager,
				FatBase					&fatBase
				)
{
	skipHWhites( text, i, lineEnd );

	U8	closingSymbol;
	if ( text[i].Ch == '"' )	closingSymbol = '"';	else
	if ( text[i].Ch == '<' )	closingSymbol = '>';
	else
		throw Exception( fatBase, text[i], "No parameter for 'include' ?" );


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
static bool isAlphaNumStart( char c )
{
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

//==================================================================
static bool isAlphaNumBody( char c )
{
	return c == '_' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

//==================================================================
static bool isAlphaNum( const std::string &str )
{
	if NOT( str.length() )
		return false;

	if NOT( isAlphaNumStart( str[0] ) )
		return false;

	for (size_t i=1; i < str.length(); ++i)
		if NOT( isAlphaNumBody( str[i] ) )
			return false;

	return true;
}

//==================================================================
static void handleDefine(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				size_t		startPoint,
				FatBase		&fatBase,
				SymbolsMap	&symbols
				)
{
	skipHWhites( text, i, lineEnd );
	size_t	symbolStartIdx = i;

	for (; i < lineEnd; ++i)
		if ( text[i].Ch == ' ' || text[i].Ch == '\t' )
			break;
	size_t	symbolEndIdx = i;

	std::string	symbolName;
	for (size_t j=symbolStartIdx; j < symbolEndIdx; ++j)
		symbolName += text[j].Ch;

	if NOT( isAlphaNum( symbolName ) )
		throw Exception(
					fatBase,
					text[symbolStartIdx],
					"Expecting an alphanumeric but found '%s'",
					symbolName.c_str() );

	//---
	skipHWhites( text, i, lineEnd );
	size_t	valueStartIdx = i;

	for (; i < lineEnd; ++i)
		if ( text[i].Ch == ' ' || text[i].Ch == '\t' )
			break;
	size_t	valueEndIdx = i;

	std::string	valueStr;
	for (size_t j=valueStartIdx; j < valueEndIdx; ++j)
		valueStr += text[j].Ch;

	symbols[symbolName] = valueStr;

	CutVector( text, startPoint, lineEnd );
}

//==================================================================
static size_t applySymbols(
				DVec<Fat8>			&text,
				size_t				start,
				size_t				lineEnd,
				const SymbolsMap	&symbols,
				DVec<Fat8>			&scrcpad
				)
{
	std::string	tmp;

	bool		replacementHappened = false;
	scrcpad.clear();

	size_t		i = start;

	// this one sort of tokenizes.. we just need to find
	// the alphanumeric stuff separated by special symbols
	// or white-spaces.. and completely skip strings

	// NOTE that we copy into the line scratchpad as we go
	// but only apply the scratchpad if an actual substitution
	// has happened
	for (; i < lineEnd; ++i)
	{
		char c = text[i].Ch;
		if ( c == ' ' || c == '\t' )
		{
			scrcpad.push_back( text[i] );
			continue;
		}
		else
		if ( c == '"' )
		{
			// skip to the end of the string
			for (; i < lineEnd; ++i)
			{
				c = text[i].Ch;
				scrcpad.push_back( text[i] );
	
				if ( c == '\\' )
				{
					++i;
					if ( i < lineEnd )
						scrcpad.push_back( text[i] );
				}
				else
				if ( c == '"' )
					break;
			}
		}
		else
		if ( isAlphaNumStart( c ) )
		{
			// grab this alphanum
			tmp.clear();
			size_t j = i;
			for (; j < lineEnd; ++j)
			{
				if NOT( isAlphaNumBody( text[j].Ch ) )
					break;

				tmp += text[j].Ch;
			}

			SymbolsMap::const_iterator foundIt = symbols.find( tmp );
			if ( foundIt != symbols.end() )
			{
				replacementHappened = true;

				const std::string &symbVal = foundIt->second;

				// copy defined symbol, using the current text location
				// ..current file name and line number
				for (size_t k=0; k < symbVal.length(); ++k)
				{
					Fat8 *pFatChar = scrcpad.grow();
					pFatChar->Ch		= symbVal[k];
					pFatChar->FNameIdx	= text[i].FNameIdx;
					pFatChar->SrcPos	= text[i].SrcPos;
				}

				i = j;
			}
			else
			{
				// copy the captured alphanumeric
				for (; i < j; ++i)
					scrcpad.push_back( text[i] );

			}

			// go back one to compensate for the for-loop next increment
			if ( i ) --i;
		}
		else
			scrcpad.push_back( text[i] );
	}

	if ( replacementHappened )
	{
		ReplaceSubVector( text, start, lineEnd, scrcpad );

		// return the new lineEnd position
		return start + scrcpad.size();
	}
	else
		return lineEnd;
}

//==================================================================
static void processFile(
				DIO::FileManagerBase	&fmanager,
				FatBase					&fatBase,
				DVec<Fat8>				&text,
				const char				*pIncFileName )
{
	SymbolsMap	symbols;

	// use this as a scratch-pad to avoid countless reallocations
	// on every macro substitution
	DVec<Fat8>	workTextLine;

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
			size_t	startPoint = i;

			++i;
			skipHWhites( text, i, lineEnd );

			if ( matchesAdvance( text, i, lineEnd, "include" ) )
			{
				handleInclude( text, i, lineEnd, startPoint, fmanager, fatBase );
				i = startPoint;
				continue;
			}
			else
			if ( matchesAdvance( text, i, lineEnd, "define" ) )
			{
				handleDefine( text, i, lineEnd, startPoint, fatBase, symbols );
				i = startPoint;
				continue;
			}
		}
		else
		{
			lineEnd = applySymbols( text, i, lineEnd, symbols, workTextLine );
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
