/*
 *  RI_SlShader.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/19.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_SlShader.h"
#include "RI_SlRunContext.h"
#include "RI_SlShaderAsmParser.h"
#include "RI_Attributes.h"
#include "RI_State.h"
#include "DUtils.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader( const CtorParams &params ) :
	ResourceBase(params.pName)
{
	if ( params.pSource )
	{
		DUT::MemFile	file((const void *)params.pSource,
							  strlen(params.pSource) );

		ShaderAsmParser	parser( file, this, params.pName );
	}
	else
	if ( params.pSourceFileName )
	{
		DUT::MemFile	file( params.pSourceFileName );

		ShaderAsmParser	parser( file, this, params.pName );
	}
	else
	{
		DASSTHROW( 0, ("Missing parameters !") );
	}
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
			pDataSegment[i].Data.pVoidValue = symbol.AllocClone( g.mPointsN );
			break;

		case SlSymbol::GLOBAL:
			break;
		}
	}

	return pDataSegment;
}

//==================================================================
void SlShaderInstance::Unbind( SlValue * &pDataSegment )
{
	size_t	symbolsN = mpShader->mSymbols.size();

	for (size_t i=0; i < symbolsN; ++i)
	{
		SlSymbol	&symbol = mpShader->mSymbols[i];

		switch ( symbol.mStorage )
		{
		case SlSymbol::CONSTANT:
			pDataSegment[i].Data.pVoidValue = NULL;
			break;

		case SlSymbol::PARAMETER:
			pDataSegment[i].Data.pVoidValue = NULL;
			break;

		case SlSymbol::TEMPORARY:			
			symbol.FreeClone( pDataSegment[i].Data.pVoidValue );
			break;

		case SlSymbol::GLOBAL:
			break;
		}
	}
	
	delete [] pDataSegment;
	pDataSegment = NULL;
}


//==================================================================
typedef void (*ShaderInstruction)( SlRunContext &ctx );

//==================================================================
template <class TA, class TB>
void Inst_Copy( SlRunContext &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetVoid( 1 );
	const TB*	op1	= (const TB*)ctx.GetVoid( 2 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		bool	op1_varying = ctx.IsSymbolVarying( 2 );
		int		op1_offset = 0;
		
		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[op1_offset];

			if ( op1_varying )	++op1_offset;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );
				 
		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[0];
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, const char OPNAME>
void Inst_AlOp( SlRunContext &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetVoid( 1 );
	const TA*	op1	= (const TA*)ctx.GetVoid( 2 );
	const TB*	op2	= (const TB*)ctx.GetVoid( 3 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		bool	op1_varying = ctx.IsSymbolVarying( 2 );
		bool	op2_varying = ctx.IsSymbolVarying( 3 );
		int		op1_offset = 0;
		int		op2_offset = 0;
		
		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
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

		TA	tmp;
		if ( OPNAME == '+' ) tmp = op1[0] + op2[0]; else
		if ( OPNAME == '-' ) tmp = op1[0] - op2[0]; else
		if ( OPNAME == '*' ) tmp = op1[0] * op2[0]; else
		if ( OPNAME == '/' ) tmp = op1[0] / op2[0];

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = tmp;
	}

	ctx.NextInstruction();
}

//==================================================================
static void Inst_Normalize( SlRunContext &ctx )
{
		  SlVector*	lhs	= (		 SlVector*)ctx.GetVoid( 1 );
	const SlVector*	op1	= (const SlVector*)ctx.GetVoid( 2 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;
		
		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = op1[op1_offset].GetNormalized();

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		SlVector	tmp = op1[0].GetNormalized();

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = tmp;
	}

	ctx.NextInstruction();
}

//==================================================================
static void Inst_Faceforward( SlRunContext &ctx )
{
		  SlVector* lhs =	(		SlVector*)ctx.GetVoid( 1 );
	const SlVector* pN	 =	(const	SlVector*)ctx.GetVoid( 2 );
	const SlVector* pI	 =	(const	SlVector*)ctx.GetVoid( 3 );

	const SlSymbol*	 pNgSymbol = ctx.mpSymbols->LookupVariable( "Ng", SlSymbol::NORMAL );
	const SlVector* pNg = (const SlVector *)pNgSymbol->GetData();

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	bool	N_step	= ctx.IsSymbolVarying( 2 );
	bool	I_step	= ctx.IsSymbolVarying( 3 );
	bool	Ng_step	= pNgSymbol->mIsVarying ? 1 : 0;

	if ( lhs_varying )
	{
		int		N_offset	= 0;
		int		I_offset	= 0;
		int		Ng_offset	= 0;
		
		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = pN[N_offset] * DSign( -pI[I_offset].GetDot( pNg[Ng_offset] ) );

			N_offset	+= N_step	;
			I_offset	+= I_step	;
			Ng_offset	+= Ng_step	;
		}
	}
	else
	{
		DASSERT( !N_step		);
		DASSERT( !I_step		);
		DASSERT( !Ng_step	);

		SlVector	tmp = pN[0] * DSign( -pI[0].GetDot( pNg[0] ) );

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = tmp;
	}

	ctx.NextInstruction();
}

//==================================================================
static inline void illuminate(
				SlColor &accCol,
				const DVec<LightSourceT *>	&pLights,
				const DVec<U16>				&activeLights,
				const Point3 &pos,
				const SlVector &Nn,
				float illConeCosA )
{
	for (size_t i=0; i < activeLights.size(); ++i)
	{
		size_t		li	= activeLights[i];

		const LightSourceT	&light = *pLights[ li ];

		if ( light.mType == LightSourceT::TYPE_DISTANT )
		{
			SlColor	lightCol( light.mColor.x(), light.mColor.y(), light.mColor.z() );
			SlVector	lightDir( light.mRend.mDistant.mDirCS.x(), light.mRend.mDistant.mDirCS.y(), light.mRend.mDistant.mDirCS.z() );

			SlScalar	norLightCosA = Nn.GetDot( lightDir );
			//if ( norLightCosA < illConeCosA )
				accCol += lightCol * norLightCosA;
		}
	}

	//DASSERT( accCol.x >= 0 && accCol.y >= 0 && accCol.z >= 0 );
}

//==================================================================
static inline Color specularbrdf(
						 const Vec3f &L,
						 const Vec3f &V,
						 const Vec3f &N,
						 float ooRoughness
						)
{
	Vec3f	H = (L + V).GetNormalized();
	float	nh = N.GetDot( H );
	float	a = powf( DMAX( 0, nh ), ooRoughness );
	
	return Color( 1, 0, 0 );
}

//==================================================================
// this is a simplified version.. until lights become available 8)
/*
{
color C = 0;
illuminance( P, N, PI/2 )
	C += Cl * normalize(L).N;
return C;
*/

static void Inst_Diffuse( SlRunContext &ctx )
{
		  SlColor* lhs	= (		 SlColor*)ctx.GetVoid( 1 );
	const SlVector* op1	= (const SlVector*)ctx.GetVoid( 2 );

	const SlSymbol*	pPSymbol = ctx.mpSymbols->LookupVariable( "P", SlSymbol::POINT );
	const Point3*	pP = (const Point3 *)pPSymbol->GetData();

	const DVec<LightSourceT *>	&pLights	= ctx.mpAttribs->mpState->GetLightSources();
	const DVec<U16>				&actLights	= ctx.mpAttribs->mActiveLights;

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	float	illConeCosA = cosf( FM_PI_2 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				SlVector	Nn = op1[op1_offset].GetNormalized();

				SlColor	col( 0.0f );

				illuminate( col, pLights, actLights, *pP, Nn, illConeCosA );

				lhs[i] = col;
			}

			++pP;

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		SlVector	Nn = op1[0].GetNormalized();

		SlColor	col( 0.0f );
		illuminate( col, pLights, actLights, *pP, Nn, illConeCosA );

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = col;
	}

	ctx.NextInstruction();
}

//==================================================================
// this is a simplified version.. until lights become available 8)
static void Inst_Ambient( SlRunContext &ctx )
{
	SlColor*	lhs	= (SlColor*)ctx.GetVoid( 1 );

	SlColor	ambCol( 0.f );

	const DVec<LightSourceT *>	&pLights	= ctx.mpAttribs->mpState->GetLightSources();
	const DVec<U16>				&actLights	= ctx.mpAttribs->mActiveLights;

	for (size_t i=0; i < actLights.size(); ++i)
	{
		size_t		li	= actLights[i];

		const LightSourceT	&light = *pLights[ li ];

		SlColor	lightCol( light.mColor.x(), light.mColor.y(), light.mColor.z() );
		SlScalar	lightInt( light.mIntesity );

		if ( light.mType == LightSourceT::TYPE_AMBIENT )
			ambCol += lightCol * lightInt;
	}

	//DASSERT( ambCol.x >= 0 && ambCol.y >= 0 && ambCol.z >= 0 );

	for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
	{
		if ( ctx.IsProcessorActive( i ) )
			lhs[i] = ambCol;
	}

	ctx.NextInstruction();
}

//==================================================================
#define SINGLE	SlScalar
#define VECTOR	SlVector
//#define MATRIX	Matrix44

//==================================================================
static ShaderInstruction	sInstructionTable[OP_N] =
{
Inst_Copy<SINGLE,SINGLE>,
Inst_Copy<VECTOR,SINGLE>,
Inst_Copy<VECTOR,VECTOR>,

Inst_AlOp<SINGLE,SINGLE,'+'>,
Inst_AlOp<VECTOR,SINGLE,'+'>,
Inst_AlOp<VECTOR,VECTOR,'+'>,

Inst_AlOp<SINGLE,SINGLE,'-'>,
Inst_AlOp<VECTOR,SINGLE,'-'>,
Inst_AlOp<VECTOR,VECTOR,'-'>,

Inst_AlOp<SINGLE,SINGLE,'*'>,
Inst_AlOp<VECTOR,SINGLE,'*'>,
Inst_AlOp<VECTOR,VECTOR,'*'>,

Inst_AlOp<SINGLE,SINGLE,'/'>,
Inst_AlOp<VECTOR,SINGLE,'/'>,
Inst_AlOp<VECTOR,VECTOR,'/'>,

Inst_Normalize,
Inst_Faceforward,
Inst_Diffuse,
Inst_Ambient,
};

//==================================================================
void SlShaderInstance::Run( MicroPolygonGrid &g, const Attributes *pAttribs )
{
	SlRunContext	ctx;
	
	ctx.mProgramCounter = 0;
	ctx.mpDataSegment	= Bind( g );
	ctx.InitializeSIMD( g );
	ctx.mpShaderInst	= this;
	ctx.mpAttribs		= pAttribs;

	while ( ctx.mProgramCounter < mpShader->mCode.size() )
	{
		const SlCPUWord	*pWord = ctx.GetOp( 0 );
		
		// [pWord->mOpCode.mDestOpType]
		sInstructionTable[pWord->mOpCode.mTableOffset]( ctx );
	}
	
	Unbind( ctx.mpDataSegment );
}

//==================================================================
}

