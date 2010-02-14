//==================================================================
/// RSLC_Prepro.cpp
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <hash_map>
#include "RSLC_TextUtils.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Prepro_Utils.h"
#include "RSLC_Prepro_ClearChunk.h"
#include "RSLC_Prepro.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

typedef stdext::hash_map<std::string,std::string>	SymbolsMap;

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
	SkipHWhites( text, i, lineEnd );

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

			size_t newEnd = ClearChunk( text, includePoint, insertEnd );
			DASSERT( newEnd <= insertEnd );

			CutVector( text, newEnd, insertEnd );

			return;
		}

		includeName += (char)text[i].Ch;
	}

	throw Exception( fatBase, text[nameStartIdx], "No closing symbol for 'include'" );
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
	SkipHWhites( text, i, lineEnd );
	size_t	symbolStartIdx = i;

	for (; i < lineEnd; ++i)
		if ( text[i].Ch == ' ' || text[i].Ch == '\t' )
			break;
	size_t	symbolEndIdx = i;

	std::string	symbolName;
	for (size_t j=symbolStartIdx; j < symbolEndIdx; ++j)
		symbolName += text[j].Ch;

	if NOT( IsAlphaNumStr( symbolName ) )
		throw Exception(
					fatBase,
					text[symbolStartIdx],
					"Expecting an alphanumeric but found '%s'",
					symbolName.c_str() );

	//---
	SkipHWhites( text, i, lineEnd );
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
		if ( IsAlphaNumStart( c ) )
		{
			// grab this alphanum
			tmp.clear();
			size_t j = i;
			for (; j < lineEnd; ++j)
			{
				if NOT( IsAlphaNumBody( text[j].Ch ) )
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
			SkipHWhites( text, i, lineEnd );

			if ( MatchesAdvance( text, i, lineEnd, "include" ) )
			{
				handleInclude( text, i, lineEnd, startPoint, fmanager, fatBase );
				i = startPoint;
				continue;
			}
			else
			if ( MatchesAdvance( text, i, lineEnd, "define" ) )
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

	size_t newEnd = ClearChunk( out_text );
	out_text.resize( newEnd );

	processFile( fmanager, fatBase, out_text, pBaseInclude );
}

//==================================================================
}
//==================================================================
}
