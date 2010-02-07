//==================================================================
/// RSLC_Prepro.cpp
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro.h"

#define LF	0x0a
#define CR	0x0d

//==================================================================
namespace RSLC
{

//==================================================================
static bool matches( DVec<U8> &str, size_t i, const char *pFindStr )
{
	for (; i < str.size() && *pFindStr; ++i)
		if ( str[i] != *pFindStr++ )
			return false;

	return true;
}

//==================================================================
static void convertToUnixNewlines( DVec<U8> &text )
{
	size_t	j = 0;
	for (size_t i=0; i < text.size(); ++i)
	{
		if ( text[i] == CR )
			continue;

		text[ j++ ] = text[ i ];
	}

	text.resize( j );
}

//==================================================================
// relies on newlines being unix-stile here
static void spliceLines( DVec<U8> &text )
{
	size_t	j = 0;
	for (size_t i=0; i < text.size(); ++i)
	{
		bool	splice = false;

		if ( text[i] == '\\' )
		{
			// look for a newline and skip trailing whitespaces
			for (size_t i2=i+1; i2 < text.size(); ++i2)
			{
				if ( text[i2] != ' ' && text[i2] != '\t' )
				{
					// found newline or file end.. so stitch
					if ( text[i2] == LF || (i2+1) == text.size() )
					{	// we simply set scan to continue after the newline or file-end
						i = i2;
						splice = true;
						break;
					}

				}
			}
		}

		if NOT( splice )
			text[ j++ ] = text[ i ];
	}

	text.resize( j );
}

//==================================================================
static void stripComments( DVec<U8> &text )
{
	bool	isLineComment = false;
	bool	isBlockComment = false;

	size_t	j = 0;

	for (size_t i=0; i < text.size(); ++i)
	{
		if ( isLineComment )
		{
			if ( text[i] == '\n' )
				isLineComment = false;
			else
				continue;
		}
		else
		if ( isBlockComment )
		{
			if ( matches( text, i, "*/" ) )
			{
				i += 2;
				isBlockComment = false;
			}
			else
				continue;
		}
		else
		if ( matches( text, i, "//" ) )
		{
			i += 2;
			isLineComment = true;
			continue;
		}
		else
		if ( matches( text, i, "/*" ) )
		{
			i += 2;
			isBlockComment = true;
			text[ j++ ] = LF;	// add a whitespace in place of a whole comment
			continue;
		}

		text[ j++ ] = text[ i ];
	}

	text.resize( j );
}

//==================================================================
static void optimizeWhitespaces( DVec<U8> &text )
{
	bool	wasCR = false;
	size_t	j = 0;
	for (size_t i=0; i < text.size(); ++i)
	{
		if ( text[i] == LF )
		{
			wasCR = true;
			continue;
		}

		if ( wasCR )
		{
			wasCR = false;
			text[ j++ ] = LF;
		}

		text[ j++ ] = text[ i ];
	}

	text.resize( j );
}

//==================================================================
static void includeFile(
				DIO::FileManagerBase &fmanager,
				DVec<U8> &globalFile,
				const char *pIncFileName )
{
	DVec<U8> incFileData;

	//try {
		fmanager.GrabFile( pIncFileName, incFileData );
	//}
	//catch ( ... )
	{
		//std::string	msg = DUT::SSPrintFS( "Failed to open %s for inclusion", pIncFileName );
		//DASSTHROW( 0, (msg.c_str()) );
	}

	globalFile.append_array( &incFileData[0], incFileData.size() );
}

//==================================================================
Prepro::Prepro(
		DIO::FileManagerBase	&fmanager,
		const DVecRO<U8>		&inText,
		const char				*pBaseInclude,
		Map						&out_map,
		DVec<U8>				&out_text )
{
	out_text.clear();

	includeFile( fmanager, out_text, pBaseInclude );

	out_text.append_array( &inText[0], inText.size() );

	convertToUnixNewlines( out_text );

	spliceLines( out_text );

	stripComments( out_text );

	optimizeWhitespaces( out_text );
}

//==================================================================
}
