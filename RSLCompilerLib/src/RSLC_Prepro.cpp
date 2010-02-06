//==================================================================
/// RSLC_Prepro.cpp
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro.h"

//==================================================================
namespace RSLC
{

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
static bool matches( DVec<U8> &str, size_t i, const char *pFindStr )
{
	for (; i < str.size() && *pFindStr; ++i)
		if ( str[i] != *pFindStr++ )
			return false;

	return true;
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
			text[ j++ ] = ' ';	// add a whitespace in place of a whole comment
			continue;
		}

		text[ j++ ] = text[ i ];
	}

	text.resize( j );
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

	stripComments( out_text );
}

//==================================================================
}
