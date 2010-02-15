//==================================================================
/// RRL_Translator_Options.cpp
///
/// Created by Davide Pasca - 2009/12/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RRL_Translator.h"

//==================================================================
namespace RRL
{

//==================================================================
static void splitSearchPath( const char *pSrc, DVec<DStr> &out_strings )
{
	char	buff[4096];

	DASSTHROW( strlen(pSrc) < _countof(buff), ("Search path string too long !") );

	strcpy_s( buff, pSrc );

	char *pTokCtx;
	char *pTok;

	if ( pTok = strtok_r( buff, ":", &pTokCtx ) )
	{
		do 
		{
			out_strings.push_back( pTok );
		} while ( pTok = strtok_r(NULL, ":", &pTokCtx) );
	}
}

//==================================================================
static void processSearchPath( DVec<DStr> &curSPathList, DVec<DStr> &newStrings )
{
	// make a copy
	DVec<DStr>	oldList = curSPathList;

	curSPathList.clear();

	bool	gotPrevPath = false;
	bool	gotStdPath = false;

	for (size_t i=0; i < newStrings.size(); ++i)
	{
		if ( newStrings[i] == "&" )	// cur path
		{
			DASSTHROW( gotPrevPath == false, ("'&' used more than once in 'searchpath'") );
			gotPrevPath = true;

			// append old list at this point
			for (size_t j=0; j < oldList.size(); ++j)
				curSPathList.push_back( oldList[j] );
		}
		else
		if ( newStrings[i] == "@" )	// default path
		{	
			DASSTHROW( gotStdPath == false, ("'@' used more than once in 'searchpath'") );
			gotStdPath = true;
			// add as is
			curSPathList.push_back( "@" );
		}
		else
		{
			// just a path string
			curSPathList.push_back( newStrings[i] );
		}
	}
}

//==================================================================
static RI::Options::SearchPathh findSearchPathType( const char *pPathType )
{
	char buff[1024];
	strcpy_s( buff, pPathType );


	char *pTokCtx;
	char *pTok;

	if NOT( pTok = strtok_r( buff, " \t", &pTokCtx ) )
		return RI::Options::SEARCHPATH_UNKNOWN;

	// annoying, but plausible
	if ( 0 == strcmp( pTok, "string" ) )
	{
		if NOT( pTok = strtok_r( NULL, " \t", &pTokCtx ) )
			return RI::Options::SEARCHPATH_UNKNOWN;
	}

	if ( 0 == strcmp( pTok, "shader" ) )	return RI::Options::SEARCHPATH_SHADER; else
	if ( 0 == strcmp( pTok, "texture" ) )	return RI::Options::SEARCHPATH_TEXTURE; else
	if ( 0 == strcmp( pTok, "archive" ) )	return RI::Options::SEARCHPATH_ARCHIVE;
	else
	{
		return RI::Options::SEARCHPATH_UNKNOWN;
	}

}

//==================================================================
bool Translator::addCommand_options(
							const DStr		&nm,
							RI::ParamList	&p )
{
	if ( nm == "Option" )
	{
		geN( 2, p );

		const char *pOpionName = p[0].PChar();

		if ( 0 == strcmp( pOpionName, "searchpath" ) )
		{
			// example: Option "searchpath" "shader" ["@:./Shaders"] 

			const char *pPathType = p[1].PChar();
			const char *pPathString = p[2].PChar();

			RI::Options::SearchPathh	idx = findSearchPathType( pPathType );

			if ( idx == RI::Options::SEARCHPATH_UNKNOWN )
			{
				printf( "Warning: unrecognized searchpath type\n" );
				return true;
			}

			DVec<DStr>	strings;
			splitSearchPath( pPathString, strings );

			DVec<DStr>	&spathList = GetState().GetCurOptions().mSearchPaths[ idx ];
			processSearchPath( spathList, strings );
		}
	}
	else
	if ( nm == "Format" )
	{
		addFormatCmd( p );
	}
	else
	if ( nm == "FrameAspectRatio" )	{ exN( 1, p ); mState.FrameAspectRatio( p[0] );	}	else
	if ( nm == "ScreenWindow" )		{ exN( 4, p ); mState.ScreenWindow(		p[0], p[1], p[2], p[3] );	}	else
	if ( nm == "CropWindow" )		{ exN( 4, p ); mState.CropWindow(		p[0], p[1], p[2], p[3] );	}	else
	if ( nm == "Projection" )		{ geN( 1, p ); mState.Projection(	p );	}	else
	if ( nm == "Clipping" )			{ exN( 2, p ); mState.Clipping(		p[0], p[1] );	}	else
	if ( nm == "DepthOfField" )		{ exN( 3, p ); mState.DepthOfField(	p[0], p[1], p[2] );	}	else
	if ( nm == "Shutter" )			{ exN( 2, p ); mState.Shutter(		p[0], p[1] );	}	else
	if ( nm == "Color"	)			
	{
		geN( 1, p );
		if ( p.size() == 1 )
			mState.ColorSet( p[0].PFlt(3) );
		else
		if ( p.size() == 3 )
			mState.ColorSet( Color( p[0].Flt(), p[1].Flt(), p[2].Flt() ) );
		else
		{
			DASSTHROW( false, ("Wrong param count") );
		}
	}
	else
	if ( nm == "Opacity" )			{ exN( 1, p ); mState.Opacity(		p[0].PFlt(3) );	}	else
	if ( nm == "Display" )			{ geN( 3, p ); mState.Display(		p[0], p[1], p[2], p );	} else
	if ( nm == "PixelSamples" )		{ exN( 2, p ); mState.PixelSamples( p[0], p[1] );	}
	else
		return false;

	return true;
}

//==================================================================
}
