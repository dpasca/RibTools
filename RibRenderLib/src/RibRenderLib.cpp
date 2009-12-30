//==================================================================
/// RibRenderLib.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RibRenderLib.h"
#include "DSystem/include/DNetwork_Connecter.h"
#include "RI_System/include/RI_Parser.h"

//==================================================================
namespace RRL
{

//==================================================================
/// Render
//==================================================================
Render::Render( Params &params )
{
	DUT::MemFile	file;

	params.mTrans.mState.mpFileManager->GrabFile( params.mpFileName, file );

	RI::Parser	parser;

	Translator		translator( params.mTrans );

	translator.GetState().Begin( "dummy" );

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

			if ( params.mVerbose )
			{
				printf( "CMD %s ", cmdName.c_str() );

				if ( cmdParams.size() )
					printf( "(%i params)", cmdParams.size() );

				puts( "" );
			}


			try {
				translator.AddCommand( cmdName, cmdParams );
			} catch ( std::runtime_error ex )
			{
				printf( "Error while parsing line %i\n> CMD: %s\n",
					cmdLine,
					cmdName.c_str() );
				break;
			}
		}
	}

	translator.GetState().End();
}

//==================================================================
}
