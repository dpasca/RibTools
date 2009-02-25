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

//==================================================================
namespace RI
{

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
								symbol.mpToken,
								symbol.mType,
								symbol.mIsVarying );
				
			if NOT( pDataSegment[i].Data.pVoidValue )
			{
				pDataSegment[i].Data.pVoidValue = 
					mCallingParams.LookupVariable(
										symbol.mpToken,
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
void SlInst_AddFFF( SlRunContext &ctx )
{
	// ...
}

//==================================================================
static ShaderInstruction	sInstructionTable[] =
{
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

