//==================================================================
/// RibRenderLib_Translator_Transforms.cpp
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
bool Translator::addCommand_transforms(
							const DStr		&nm,
							RI::ParamList	&p )
{
	if ( nm == "Identity" )			{ exN( 0, p ); mState.Identity();							}	else
	if ( nm == "ConcatTransform" )	{ exN( 1, p ); mState.ConcatTransform(	p[0].PFlt(16) );	}	else
	if ( nm == "Transform" )		{ exN( 1, p ); mState.TransformCmd(	p[0].PFlt(16) );	}	else
	if ( nm == "Scale" )			{ exN( 3, p ); mState.Scale(		p[0], p[1], p[2] );	}	else
	if ( nm == "Rotate" )			{ exN( 4, p ); mState.Rotate(		p[0], p[1], p[2], p[3] ); }	else
	if ( nm == "Translate" )		{ exN( 3, p ); mState.Translate(	p[0], p[1], p[2] );	}
	else
		return false;

	return true;
}

//==================================================================
}
