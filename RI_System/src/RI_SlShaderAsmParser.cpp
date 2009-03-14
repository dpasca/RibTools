/*
 *  RI_SlShaderAsmParser.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/03/04.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "DUtils.h"
#include "RI_SlRunContext.h"
#include "RI_SlShaderAsmParser.h"

//==================================================================
namespace RI
{

//==================================================================
struct OpCodeDef
{
	const char	*pName;
	u_int		OperCnt;
};

//==================================================================
static OpCodeDef	gsOpCodeDefs[] =
{
	"movss",		2,
	"movvs",		2,
	"movvv",		2,
	"addss",		3,
	"addvs",		3,
	"addvv",		3,
	"subss",		3,
	"subvs",		3,
	"subvv",		3,
	"mulss",		3,
	"mulvs",		3,
	"mulvv",		3,
	"divss",		3,
	"divvs",		3,
	"divvv",		3,
	"normalize",	2,
	NULL
};

//==================================================================
/// ShaderAsmParser
//==================================================================
ShaderAsmParser::ShaderAsmParser( DUT::MemFile &file, SlShader *pShader ) :
	mpShader(pShader)
{
	if NOT( doParse( file ) )
	{
		DASSTHROW( 0, ("Error parsing shader") );
	}
}

//==================================================================
bool ShaderAsmParser::doParse( DUT::MemFile &file )
{
	char		lineBuff[1024];
	Section		curSection = CODE;
	int			lineCnt = 0;
	
	while ( file.ReadTextLine( lineBuff, sizeof(lineBuff) ) )
	{
		DUT::StrStripBeginEndWhite( lineBuff );
		
		if ( 0 == strcasecmp( lineBuff, ".data" ) )
			curSection = DATA;
		else
		if ( 0 == strcasecmp( lineBuff, ".code" ) )
			curSection = CODE;
		else
		if ( curSection == DATA )
		{
			if NOT( parseDataLine( lineBuff, lineCnt ) )
			{
				printf( "On line (%i)\n", lineCnt );
				return false;
			}
		}
		else
		{
			if NOT( parseCodeLine( lineBuff, lineCnt ) )
			{
				printf( "On line (%i)\n", lineCnt );
				return false;
			}
		}

		++lineCnt;
	}
	
	return true;
}

//==================================================================
bool ShaderAsmParser::parseDataLine( char lineBuff[], int lineCnt )
{
	//printf( "DATA: %s\n", lineBuff );
	
	const char *pLineEnd = lineBuff + strlen( lineBuff );
	
	char *pTokCtx;
	char *pTok;
	
	SlSymbol	symbol;		
	symbol.Reset();

	if NOT( pTok = strtok_r( lineBuff, " \t", &pTokCtx ) )
		return true;

	symbol.mName = pTok;

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		printf( "ERROR: Expecting storage definition\n" );
		return false;
	}
		
	if ( 0 == strcmp( pTok, "constant"  ) ) symbol.mStorage = SlSymbol::CONSTANT ; else
	if ( 0 == strcmp( pTok, "parameter" ) ) symbol.mStorage = SlSymbol::PARAMETER; else
	if ( 0 == strcmp( pTok, "temporary" ) ) symbol.mStorage = SlSymbol::TEMPORARY; else
	if ( 0 == strcmp( pTok, "global"	) ) symbol.mStorage = SlSymbol::GLOBAL   ; else
	{
		printf( "ERROR: Invalid storage definition: '%s'\n", pTok );
		return false;
	}

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		printf( "ERROR: Expecting variability definition\n" );
		return false;
	}

	if ( 0 == strcmp( pTok, "varying"	) ) symbol.mIsVarying = true ; else
	if ( 0 == strcmp( pTok, "uniform"	) ) symbol.mIsVarying = false; else
	{
		printf( "ERROR: Invalid variability definition: '%s'\n", pTok );
		return false;
	}

	if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
	{
		printf( "ERROR: Expecting type definition\n" );
		return false;
	}

	if ( 0 == strcmp( pTok, "float"  ) ) symbol.mType = SlSymbol::FLOAT ; else
	if ( 0 == strcmp( pTok, "point"  ) ) symbol.mType = SlSymbol::POINT ; else
	if ( 0 == strcmp( pTok, "color"  ) ) symbol.mType = SlSymbol::COLOR ; else
	if ( 0 == strcmp( pTok, "string" ) ) symbol.mType = SlSymbol::STRING; else
	if ( 0 == strcmp( pTok, "vector" ) ) symbol.mType = SlSymbol::VECTOR; else
	if ( 0 == strcmp( pTok, "normal" ) ) symbol.mType = SlSymbol::NORMAL; else
	if ( 0 == strcmp( pTok, "matrix" ) ) symbol.mType = SlSymbol::MATRIX; else
	{
		printf( "ERROR: Invalid type definition: '%s'\n", pTok );
		return false;
	}

	const char *pDefaultValueStr = pTok + strlen(pTok) + 1;
	if ( pDefaultValueStr < pLineEnd )
		printf( "Default value '%s'\n", pDefaultValueStr );
		
	mpShader->mSymbols.push_back( symbol );

	return true;
}

//==================================================================
const OpCodeDef	*ShaderAsmParser::findOpDef( const char *pOpName, u_int &opCodeIdx )
{
	for (size_t i=0; gsOpCodeDefs[i].pName != NULL; ++i)
		if ( 0 == strcasecmp( pOpName, gsOpCodeDefs[i].pName ) )
		{
			opCodeIdx = i;
			return gsOpCodeDefs + i;
		}
		
	return NULL;
}

//==================================================================
int ShaderAsmParser::findSymbol( const char *pName ) const
{
	for (size_t i=0; i < mpShader->mSymbols.size(); ++i)
	{
		const char *pShaderSymName = mpShader->mSymbols[i].mName.c_str();
		if ( 0 == strcasecmp( pName, pShaderSymName ) )
		{
			return i;
		}
	}

	return -1;
}

//==================================================================
static OperTypeID getOperTypeFromSlSymbolType( SlSymbol::Type slSymType, bool &out_success )
{
	out_success = true;

	switch ( slSymType )
	{
	case SlSymbol::FLOAT:	return OPRTYPE_F1 ;
	//case SlSymbol:::		return OPRTYPE_F2 ;
	case SlSymbol::POINT:
	case SlSymbol::COLOR:
	case SlSymbol::VECTOR:
							return OPRTYPE_F3 ;
	//case SlSymbol:::		return OPRTYPE_F4 ;
	case SlSymbol::MATRIX:	return OPRTYPE_M44;
	
	default:
		out_success = false;
		return OPRTYPE_F1;
	}
}

//==================================================================
static bool isTempSymbol( const char *pTok )
{
	return pTok[0] == '$';
}

//==================================================================
bool ShaderAsmParser::parseCodeLine( char lineBuff[], int lineCnt )
{
	//printf( "CODE: %s\n", lineBuff );
	
	char *pTokCtx;
	char *pTok;

	if NOT( pTok = strtok_r( lineBuff, " \t", &pTokCtx ) )
		return true;
		
	DUT::StrStripBeginEndWhite( pTok );
	
	u_int	opCodeIdx;
	const OpCodeDef	*pOpDef = findOpDef( pTok, opCodeIdx );
	
	if NOT( pOpDef )
	{
		printf( "ERROR: Unknown opcode '%s' !\n", pTok );
		return false;
	}

	SlCPUWord		instruction;
	instruction.mOpCode.mTableOffset	= opCodeIdx;
	instruction.mOpCode.mOperandCount	= pOpDef->OperCnt;
	instruction.mOpCode.mDestOpType		= 0;
	size_t	instrIdx = mpShader->mCode.size();
	mpShader->mCode.push_back( instruction );

	for (u_int i=0; i < pOpDef->OperCnt; ++i)
	{
		SlCPUWord	word;
		
		if NOT( pTok = strtok_r(NULL, " \t", &pTokCtx) )
		{
			printf( "ERROR: missing operand #%i, "
					"expecting %i operands\n",
					i+1,
					pOpDef->OperCnt );

			return false;
		}

		DUT::StrStripBeginEndWhite( pTok );

		int	symbolIdx = findSymbol( pTok );
		if ( symbolIdx == -1 )
		{
			printf( "ERROR: Symbol '%s' unknown !\n", pTok );
			return false;
		}

		word.mSymbol.mTableOffset = (u_int)symbolIdx;
		word.mSymbol.mIsVarying	  = mpShader->mSymbols[symbolIdx].mIsVarying;

		mpShader->mCode.push_back( word );
		
		// copy the type of the first operator into the opcode
		if ( i == 0 )
		{
			bool success;
			instruction.mOpCode.mDestOpType =
				getOperTypeFromSlSymbolType( mpShader->mSymbols[symbolIdx].mType, success );
				
			if NOT( success )
			{
				printf( "ERROR: Invalid operand type !\n", pTok );
				return false;
			}
		}
	}

	// write again
	mpShader->mCode[instrIdx] = instruction;

	return true;
}

//==================================================================
}

