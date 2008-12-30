/*
 *  DTypes.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include <exception>
#include "DTypes.h"

//===============================================================
void DAssThrow( bool ok, const char *pMsg )
{
	if ( ok )
		return;
		
	puts( pMsg );

	throw pMsg;
}

