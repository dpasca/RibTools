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
SlShader::SlShader( const char *pShaderName, const char *pShaderSource ) :
	ResourceBase(pShaderName)
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
			{
				SlSymbol	*pFoundSymbol = NULL;

				pFoundSymbol =
					g.mSymbols.LookupVariable(
									symbol.mName.c_str(),
									symbol.mType,
									symbol.mIsVarying );

				if NOT( pFoundSymbol )
				{
					pFoundSymbol =
						mCallingParams.LookupVariable(
											symbol.mName.c_str(),
											symbol.mType,
											symbol.mIsVarying );
				}

				if NOT( pFoundSymbol )
				{
					if NOT( symbol.mpDefaultVal )
						DASSTHROW( 0, ("Could not bind symbol %s", symbol.mName.c_str()) );

					pDataSegment[i].Data.pVoidValue = symbol.mpDefaultVal;
				}
				else
					pDataSegment[i].Data.pVoidValue = pFoundSymbol->mpDefaultVal;

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
template <class T>
void Inst_Copy( SlRunContext &ctx )
{
		  T*	lhs	= (		 T*)ctx.GetVoid( 1 );
	const T*	op1	= (const T*)ctx.GetVoid( 2 );
	
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
template <class T, const char OPNAME>
void Inst_AlOp( SlRunContext &ctx )
{
		  T*	lhs	= (		 T*)ctx.GetFloat( 1 );
	const T*	op1	= (const T*)ctx.GetFloat( 2 );
	const T*	op2	= (const T*)ctx.GetFloat( 3 );
	
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
			{
				if ( OPNAME == '+' ) lhs[i] = op1[op1_offset] + op2[op2_offset]; else
				if ( OPNAME == '-' ) lhs[i] = op1[op1_offset] - op2[op2_offset]; else
				if ( OPNAME == '*' ) lhs[i] = op1[op1_offset] * op2[op2_offset]; else
				if ( OPNAME == '/' ) lhs[i] = op1[op1_offset] / op2[op2_offset];
			}
			
			if ( op1_varying )	++op1_offset;
			if ( op2_varying )	++op2_offset;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

		T	tmp;
		if ( OPNAME == '+' ) tmp = op1[0] + op2[0]; else
		if ( OPNAME == '-' ) tmp = op1[0] - op2[0]; else
		if ( OPNAME == '*' ) tmp = op1[0] * op2[0]; else
		if ( OPNAME == '/' ) tmp = op1[0] / op2[0];

		for (u_int i=0; i < ctx.mSIMDCount; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = tmp;
	}

	ctx.NextInstruction();
}

//==================================================================
#define F1	float
#define F2	Vector2
#define F3	Vector3
#define F4	Vector4
#define F44	Matrix44

//==================================================================
static ShaderInstruction	sInstructionTable[OP_N][OPRTYPE_N] =
{
Inst_Copy<F1>,	  Inst_Copy<F2>,	Inst_Copy<F3>,	  Inst_Copy<F4>,	Inst_Copy<F44>,
Inst_AlOp<F1,'+'>,Inst_AlOp<F2,'+'>,Inst_AlOp<F3,'+'>,Inst_AlOp<F4,'+'>,Inst_AlOp<F44,'+'>,
Inst_AlOp<F1,'-'>,Inst_AlOp<F2,'-'>,Inst_AlOp<F3,'-'>,Inst_AlOp<F4,'-'>,Inst_AlOp<F44,'-'>,
Inst_AlOp<F1,'*'>,Inst_AlOp<F2,'*'>,Inst_AlOp<F3,'*'>,Inst_AlOp<F4,'*'>,Inst_AlOp<F44,'*'>,
Inst_AlOp<F1,'/'>,Inst_AlOp<F2,'/'>,Inst_AlOp<F3,'/'>,Inst_AlOp<F4,'/'>,Inst_AlOp<F44,'/'>,
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
		
		sInstructionTable[pWord->mOpCode.mTableOffset][pWord->mOpCode.mDestOpType]( ctx );
	}
}

//==================================================================
}

