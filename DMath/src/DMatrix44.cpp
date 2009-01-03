/*
 *  DMatrix44.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/22.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "DMatrix44.h"

//==================================================================
void Matrix44::PrintOut() const
{
	printf( "[" );
	for (size_t r=0; r < 4; ++r)
	{
		if ( r != 0 )
			printf( " |" );

		for (size_t c=0; c < 4; ++c)
		{
			printf( " %f", u.m44[r][c] );
		}
	}
	printf( "]\n" );
}
