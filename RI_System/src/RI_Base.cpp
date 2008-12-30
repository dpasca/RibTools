/*
 *  RI_Base.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Base.h"

//==================================================================
namespace RI
{
	
//==================================================================
const char *ErrorToString( Error errCode )
{
	switch ( errCode )
	{
	case E_OK			:	return "E_OK";
	case E_NESTING		:	return "E_NESTING";
	case E_NOTOPTIONS	:	return "E_NOTOPTION";
	case E_NOTATTRIBS	:	return "E_NOTATTRIB";
	case E_NOTPRIMS		:	return "E_NOTPRIMS";
	case E_NOTSTARTED	:	return "E_NOTSTARTE";
	case E_ILLSTATE		:	return "E_ILLSTATE";
	default:				return "Unknown error !!!";
	}
}

//==================================================================
}