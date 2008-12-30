/*
 *  DUtils.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include <stdio.h>
#include "DTypes.h"

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, void * &out_pData, size_t &out_dataSize )
{
	out_pData		= NULL;
	out_dataSize	= 0;
	
	FILE	*pFile = fopen( pFileName, "rb" );
	if NOT( pFile )
	{
		return false;
	}
	
	fseek( pFile, 0, SEEK_END );
	out_dataSize = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );
	
	out_pData = malloc( out_dataSize );
	
	fread( out_pData, 1, out_dataSize, pFile );
	
	fclose( pFile );
	
	return true;
}

//==================================================================
}