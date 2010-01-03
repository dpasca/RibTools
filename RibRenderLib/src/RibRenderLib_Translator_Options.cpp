//==================================================================
/// RibRenderLib_Translator_Options.cpp
///
/// Created by Davide Pasca - 2009/12/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RibRenderLib_Translator.h"

//==================================================================
namespace RRL
{

//==================================================================

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
	if ( nm == "PixelSamples" )		{ exN( 2, p ); mState.PixelSamples( p[0], p[1] );	}
	else
		return false;

	return true;
}

//==================================================================
}