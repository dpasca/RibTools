/*
 *  RI_SlShader.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/19.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_SlShader.h"
#include "RI_SlRunContext.h"
#include "DUtils.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlShader
//==================================================================
const char *pColorCopyShader =
".data"	"\n"
"	Ci		parameter varying point"	"\n"
"	Oi		parameter varying color"	"\n"
""	"\n"
".code"	"\n"
"	move	Oi, Ci"	"\n"
;

//==================================================================
class ShaderAsmParser
{
	enum Section
	{
		DATA,
		CODE,
	};
	
public:
	ShaderAsmParser( DUT::MemFile &file )
	{
		doParse( file );
	}

private:
	//==================================================================
	void doParse( DUT::MemFile &file )
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
				parseDataLine( lineBuff, lineCnt );
			}
			else
			{
				parseCodeLine( lineBuff, lineCnt );
			}

			++lineCnt;
		}
	}

	//==================================================================
	bool parseDataLine( char lineBuff[], int lineCnt )
	{
		printf( "DATA: %s\n", lineBuff );
		
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

		return true;
	}

	//==================================================================
	void parseCodeLine( const char lineBuff[], int lineCnt )
	{
		printf( "CODE: %s\n", lineBuff );
	}
};

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader()
{
	DUT::MemFile	file(	(const void *)pColorCopyShader,
							strlen(pColorCopyShader) );

	ShaderAsmParser	parser( file );

	// initialize with a custom default shader for now !
	SlCPUWord		word;
	
	word.mOpCode.mTableOffset = 0;
	word.mOpCode.mOperandCount = 2;
	mCode.push_back( word );
	
	word.mSymbol.mTableOffset = 0;
	word.mSymbol.mIsVarying = true;
	mCode.push_back( word );

	word.mSymbol.mTableOffset = 0;
	word.mSymbol.mIsVarying = true;
	mCode.push_back( word );
}

//==================================================================
/// SlShaderInstance
//==================================================================
SlShaderInstance::SlShaderInstance() :
	mpShader(NULL)
{
}

//==================================================================
void SlShaderInstance::SetParameter(
			const char		*pParamName,
			SlSymbol::Type	type,
			bool			isVarying,
			void			*pValue )
{
	
}

//==================================================================
SlValue	*SlShaderInstance::Bind( MicroPolygonGrid &g )
{
	size_t	symbolsN = mpShader->mSymbols.size();

	SlValue	*pDataSegment = new SlValue [ symbolsN ];
	
	for (size_t i=0; i < symbolsN; ++i)
	{
		SlSymbol	&symbol = mpShader->mSymbols[i];

		switch ( symbol.mStorage )
		{
		case SlSymbol::CONSTANT:
			pDataSegment[i].Data.pVoidValue =
								symbol.mpDefaultVal;
			break;

		case SlSymbol::PARAMETER:
			pDataSegment[i].Data.pVoidValue = 
				g.mSymbols.LookupVariable(
								symbol.mName.c_str(),
								symbol.mType,
								symbol.mIsVarying );
				
			if NOT( pDataSegment[i].Data.pVoidValue )
			{
				pDataSegment[i].Data.pVoidValue = 
					mCallingParams.LookupVariable(
										symbol.mName.c_str(),
										symbol.mType,
										symbol.mIsVarying );
			}

			if NOT( pDataSegment[i].Data.pVoidValue )
			{
				pDataSegment[i].Data.pVoidValue = 
									symbol.mpDefaultVal;
			}
			break;

		case SlSymbol::TEMPORARY:
			break;

		case SlSymbol::GLOBAL:
			break;
		}
	}

	return pDataSegment;
}

//==================================================================
typedef void (*ShaderInstruction)( SlRunContext &ctx );

//==================================================================
void SlInst_CopyFF( SlRunContext &ctx )
{
	float		*lhs	= ctx.GetFloat( 1 );
	const float	*op1	= ctx.GetFloat( 2 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		bool	op1_varying = ctx.IsSymbolVarying( 2 );
		int		op1_offset = 0;
		
		for (u_int i=0; i < ctx.mSIMDCount; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[op1_offset];
			
			if ( op1_varying )	++op1_offset;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );
				 
		for (u_int i=0; i < ctx.mSIMDCount; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[0];
	}

	ctx.NextInstruction();
}

//==================================================================
void SlInst_AddFFF( SlRunContext &ctx )
{
	float		*lhs	= ctx.GetFloat( 1 );
	const float	*op1	= ctx.GetFloat( 2 );
	const float	*op2	= ctx.GetFloat( 3 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		bool	op1_varying = ctx.IsSymbolVarying( 2 );
		bool	op2_varying = ctx.IsSymbolVarying( 3 );
		int		op1_offset = 0;
		int		op2_offset = 0;
		
		for (u_int i=0; i < ctx.mSIMDCount; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[op1_offset] + op2[op2_offset];
			
			if ( op1_varying )	++op1_offset;
			if ( op2_varying )	++op2_offset;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );
				 
		for (u_int i=0; i < ctx.mSIMDCount; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[0] + op2[0];
	}

	ctx.NextInstruction();
}

//==================================================================
static ShaderInstruction	sInstructionTable[] =
{
	SlInst_CopyFF,
	SlInst_AddFFF
};

//==================================================================
void SlShaderInstance::Run( MicroPolygonGrid &g )
{

	SlRunContext	ctx;
	
	ctx.mProgramCounter = 0;
	ctx.mpDataSegment	= Bind( g );
	ctx.InitializeSIMD( g );
	ctx.mpShaderInst	= this;

	while ( ctx.mProgramCounter < mpShader->mCode.size() )
	{
		const SlCPUWord	*pWord = ctx.GetOp( 0 );
		
		sInstructionTable[pWord->mOpCode.mTableOffset]( ctx );
	}
}

//==================================================================
}

