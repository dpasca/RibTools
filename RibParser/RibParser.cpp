/*
 *  RibParser.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include <stdio.h>
#include "RI_Parser.h"
#include "RI_Machine.h"
#include "DUtils.h"

#include "RibParser.h"

//==================================================================
int main( int argc, char *argv[] )
{
	if ( argc != 2 )
	{
		printf( "Invalid param count. Quitting !\n" );
		return -1;
	}

	void	*pData;
	size_t	dataSize;

	if NOT( DUT::GrabFile( argv[1], pData, dataSize ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return -1;
	}

	RI::Parser		parser;

	for (size_t i=0; i <= dataSize; ++i)
	{
		if ( i == dataSize )
			parser.AddChar( 0 );
		else
			parser.AddChar( ((char *)pData)[i] );

		if ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;
			int				cmdLine;

			parser.FlushNewCommand( &cmdName, &cmdParams, &cmdLine );

			printf( "CMD %s ", cmdName.c_str() );
			
			if ( cmdParams.size() )
				printf( "(%i params)", cmdParams.size() );

			puts( "" );
		}
		
	}
	return 0;
}


