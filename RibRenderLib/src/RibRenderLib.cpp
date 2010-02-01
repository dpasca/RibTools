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
	RI::Parser		parser;
	Translator		translator( params.mTrans );

	translator.GetState().Begin( "dummy" );

	readArchive( params.mpFileName, params, parser, translator );

	translator.GetState().End();
}

//==================================================================
void Render::readArchive(
					const char *pFileName,
					const Params &params,
					RI::Parser &parser, 
					Translator &translator )
{
	DUT::MemFile	file;

	params.mTrans.mState.mpFileManager->GrabFile( pFileName, file );

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
				// add a command
				Translator::RetCmd	retCmd = translator.AddCommand( cmdName, cmdParams );

				switch ( retCmd )
				{
				case Translator::CMD_WORLDEND:
					{
						RI::Options	&options = translator.GetState().GetCurOptions();

						// if the world definition has ended, then we should have some displays
						const DisplayList &dispList = options.GetDisplays();
						
						// see if we have a callback to process the displays
						if ( params.mpOnFrameEndCB )
							params.mpOnFrameEndCB( params.mpOnFrameEndCBData, dispList );

						// free the displays
						options.FreeDisplays();
					}
					break;

				case Translator::CMD_READARCHIVE:

					break;

				default:
				case Translator::CMD_GENERIC:
					break;
				}

			} catch ( std::runtime_error ex )
			{
				printf( "Error while parsing line %i\n> CMD: %s\n",
					cmdLine,
					cmdName.c_str() );
				break;
			}
		}
	}
}

//==================================================================
}
