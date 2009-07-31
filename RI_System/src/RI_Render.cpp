//==================================================================
/// RI_Render.cpp
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Parser.h"
#include "RI_Machine.h"
#include "RI_Render.h"

//==================================================================
namespace RI
{

//==================================================================
/// Render
//==================================================================
Render::Render( const char *pFileName, Machine &machine, SOCKET ioSocket, bool verbose )
{
	DVec<U8>	fileData;

	if NOT( DUT::GrabFile( pFileName, fileData ) )
	{
		DASSTHROW( 0, ( "Could not open the file in input. Quitting !\n" ) );
	}

	RI::Parser	parser;

	for (size_t i=0; i <= fileData.size(); ++i)
	{
		if ( i == fileData.size() )
			parser.AddChar( 0 );
		else
			parser.AddChar( (char)fileData[i] );

		while ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;
			int				cmdLine;

			parser.FlushNewCommand( &cmdName, &cmdParams, &cmdLine );

			if ( verbose )
			{
				printf( "CMD %s ", cmdName.c_str() );

				if ( cmdParams.size() )
					printf( "(%i params)", cmdParams.size() );

				puts( "" );
			}


			try {
				machine.AddCommand( cmdName, cmdParams );
			} catch ( std::runtime_error ex )
			{
				printf( "ERROR at line: %i\n", cmdLine );
				break;
			}
		}
	}
}

//==================================================================
}
