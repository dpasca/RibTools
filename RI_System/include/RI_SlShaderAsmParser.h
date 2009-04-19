/*
 *  RI_SlShaderAsmParser.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/03/04.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_SLSHADERASMPARSER_H
#define RI_SLSHADERASMPARSER_H

#include "RI_Base.h"
#include "RI_SlShader.h"

//==================================================================
namespace RI
{

//==================================================================
struct OpCodeDef;

//==================================================================
/// ShaderAsmParser
//==================================================================
class ShaderAsmParser
{
	enum Section
	{
		DATA,
		CODE,
	};
	
	SlShader		*mpShader;
	DUT::MemFile	*mpFile;
	const char		*mpName;
	
public:
	//==================================================================
	ShaderAsmParser( DUT::MemFile &file, SlShader *pShader, const char *pName );

private:
	void doParse( DUT::MemFile &file );
	void parseDataLine( char lineBuff[], int lineCnt );
	const OpCodeDef	*findOpDef( const char *pOpName, u_int &opCodeIdx );
	int findSymbol( const char *pName, bool ignoreCase ) const;
	int findOrAddTempSymbol( const char *pName );
	void parseCodeLine( char lineBuff[], int lineCnt );

	void onError( const char *pFmt, ... );
};

//==================================================================
}

#endif
