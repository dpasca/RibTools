//==================================================================
/// RibRenderLib.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib.h"
#include "DSystem/include/DNetwork_Connecter.h"
#include "RI_System/include/RI_Parser.h"

//==================================================================
namespace RRL
{

//==================================================================
/// Render
//==================================================================
Render::Render( const char			*pFileName,
			    RI::Machine			&machine,
				RI::FileManagerBase	&fileManager,
				bool				verbose )
{
	DUT::MemFile	file;

	fileManager.GrabFile( pFileName, file );

	RI::Parser	parser;

	for (size_t i=0; i <= file.GetDataSize(); ++i)
	{
		if ( i == file.GetDataSize() )
			parser.AddChar( 0 );
		else
			parser.AddChar( (char)file.GetData()[i] );

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
