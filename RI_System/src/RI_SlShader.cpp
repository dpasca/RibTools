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
#include "RI_SlShaderAsmParser.h"
#include "DUtils.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader( const char *pShaderSource )
{
	DUT::MemFile	file((const void *)pShaderSource,
						  strlen(pShaderSource) );

	ShaderAsmParser	parser( file, this );
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

