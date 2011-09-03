//==================================================================
/// RRL_Translator_Transforms.cpp
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
bool Translator::addCommand_transforms(
							const DStr		&nm,
							RI::ParamList	&p )
{
	if ( 0==strcasecmp( nm.c_str(), "Identity" ))		{ exN( 0, p ); mState.Identity();							}	else
	if ( 0==strcasecmp( nm.c_str(), "ConcatTransform" )){ exN( 1, p ); mState.ConcatTransform(	p[0].PFlt(16) );	}	else
	if ( 0==strcasecmp( nm.c_str(), "Transform" ))		{ exN( 1, p ); mState.TransformCmd(	p[0].PFlt(16) );	}	else
	if ( 0==strcasecmp( nm.c_str(), "Scale" ))			{ exN( 3, p ); mState.Scale(		p[0], p[1], p[2] );	}	else
	if ( 0==strcasecmp( nm.c_str(), "Rotate" ))			{ exN( 4, p ); mState.Rotate(		p[0], p[1], p[2], p[3] ); }	else
	if ( 0==strcasecmp( nm.c_str(), "Translate" ))		{ exN( 3, p ); mState.Translate(	p[0], p[1], p[2] );	}
	else
		return false;

	return true;
}

//==================================================================
}
