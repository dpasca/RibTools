
#include <stdio.h>
#include "RI_Parser.h"
#include "RibParser.h"
#include "RI_Machine.h"

//==================================================================
static bool grabFile( const char *pFileName, void * &pData, size_t &dataSize )
{
	pData		= NULL;
	dataSize	= 0;
	
	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}
	
	fseek( pFile, 0, SEEK_END );
	dataSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	
	pData = malloc( dataSize );
	
	fread( pData, 1, dataSize, pFile );
	
	fclose( pFile );
	
	return true;
}

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

	if NOT( grabFile( argv[1], pData, dataSize ) )
	{
		printf( "Could not open the file in input. Quitting !\n" );
		return -1;
	}

	RI::Parser	parser;
	RI::Machine		machine;

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

			parser.FlushNewCommand( &cmdName, &cmdParams );

			printf( "CMD %s ", cmdName.c_str() );
			
			if ( cmdParams.size() )
				printf( "(%i params)", cmdParams.size() );

			puts( "" );
			
			machine.AddCommand( cmdName, cmdParams );
		}
		
	}
	return 0;
}


