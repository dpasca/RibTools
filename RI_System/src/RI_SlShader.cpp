//==================================================================
/// RI_SlShader.cpp
///
/// Created by Davide Pasca - 2009/2/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlShader.h"
#include "RI_SlRunContext.h"
#include "RI_SlShaderAsmParser.h"
#include "RI_Attributes.h"
#include "RI_State.h"
#include "DUtils.h"
#include "RI_Noise.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader( const CtorParams &params ) :
	ResourceBase(params.pName, ResourceBase::TYPE_SHADER),
	mType(TYPE_UNKNOWN)
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
SlShaderInstance::SlShaderInstance( size_t maxPointsN ) :
	mpShader(NULL),
	mMaxPointsN(maxPointsN)
{
}

//==================================================================
SlShaderInstance::~SlShaderInstance()
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
SlValue	*SlShaderInstance::Bind( const SlSymbolList &gridSymbols ) const
{
	size_t	symbolsN = mpShader->mSymbols.size();

	SlValue	*pDataSegment = DNEW SlValue [ symbolsN ];
	
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
				const SlSymbol	*pFoundSymbol = NULL;

				pFoundSymbol =
					gridSymbols.LookupVariable(
									symbol.mName.c_str(),
									symbol.mType,
									symbol.mIsVarying );

				if NOT( pFoundSymbol )
				{
					// calling params should really just point to attributes
					// I guess..
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
			pDataSegment[i].Data.pVoidValue = symbol.AllocClone( mMaxPointsN );
			break;

		case SlSymbol::GLOBAL:
			break;
		}
	}

	return pDataSegment;
}

//==================================================================
void SlShaderInstance::Unbind( SlValue * &pDataSegment ) const
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
	
	DSAFE_DELETE_ARRAY( pDataSegment );
	pDataSegment = NULL;
}


//==================================================================
typedef void (*ShaderInstruction)( SlRunContext &ctx );

//==================================================================
template <class TA, class TB, const OpCodeID opCodeID>
void Inst_1Op( SlRunContext &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetVoid( 1 );
	const TA*	op1	= (const TA*)ctx.GetVoid( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	const u_int opCodeMsk = opCodeID & ~OPERANDS_VEC_MSK;

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( opCodeMsk == OP_SS_MOV	) lhs[i] = op1[op1_offset]; else
				if ( opCodeMsk == OP_SS_ABS	) lhs[i] = DAbs( op1[op1_offset] );
					else { DASSERT( 0 ); }
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( opCodeMsk == OP_SS_MOV	) lhs[0] = op1[0]; else
			if ( opCodeMsk == OP_SS_ABS	) lhs[0] = DAbs( op1[0] );
				else { DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, const OpCodeID opCodeID>
void Inst_2Op( SlRunContext &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetVoid( 1 );
	const TA*	op1	= (const TA*)ctx.GetVoid( 2 );
	const TB*	op2	= (const TB*)ctx.GetVoid( 3 );
	
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	const u_int opCodeMsk = opCodeID & ~OPERANDS_VEC_MSK;

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( opCodeMsk == OP_SS_ADD ) lhs[i] = op1[op1_offset] + op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_SUB ) lhs[i] = op1[op1_offset] - op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_MUL ) lhs[i] = op1[op1_offset] * op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_DIV ) lhs[i] = op1[op1_offset] / op2[op2_offset];
					else { DASSERT( 0 ); }
			}
			
			op1_offset	+= op1_step;
			op2_offset	+= op2_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( opCodeMsk == OP_SS_ADD ) lhs[0] = op1[0] + op2[0]; else
			if ( opCodeMsk == OP_SS_SUB ) lhs[0] = op1[0] - op2[0]; else
			if ( opCodeMsk == OP_SS_MUL ) lhs[0] = op1[0] * op2[0]; else
			if ( opCodeMsk == OP_SS_DIV ) lhs[0] = op1[0] / op2[0];
				else { DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
static void Inst_Faceforward( SlRunContext &ctx )
{
		  SlVec3* lhs	=	(		SlVec3*)ctx.GetVoid( 1 );
	const SlVec3* pN	=	(const	SlVec3*)ctx.GetVoid( 2 );
	const SlVec3* pI	=	(const	SlVec3*)ctx.GetVoid( 3 );

	const SlSymbol*	 pNgSymbol = ctx.mpSymbols->LookupVariable( "Ng", SlSymbol::NORMAL );
	const SlVec3* pNg = (const SlVec3 *)pNgSymbol->GetData();

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

		if ( ctx.IsProcessorActive( 0 ) )
			lhs[0] = pN[0] * DSign( -pI[0].GetDot( pNg[0] ) );
	}

	ctx.NextInstruction();
}

//==================================================================
static inline void illuminate(
				SlColor &accCol,
				const DVec<LightSourceT *>	&pLights,
				const DVec<U16>				&activeLights,
				const Point3 &pos,
				const SlVec3 &Nn,
				float illConeCosA )
{
	for (size_t i=0; i < activeLights.size(); ++i)
	{
		size_t		li	= activeLights[i];

		const LightSourceT	&light = *pLights[ li ];

		if ( light.mType == LightSourceT::TYPE_DISTANT )
		{
			SlColor	lightCol( light.mColor.x(), light.mColor.y(), light.mColor.z() );
			SlVec3	lightDir( light.mRend.mDistant.mDirCS.x(), light.mRend.mDistant.mDirCS.y(), light.mRend.mDistant.mDirCS.z() );

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
	const SlVec3* op1	= (const SlVec3*)ctx.GetVoid( 2 );

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
				SlVec3	Nn = op1[op1_offset].GetNormalized();

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

		if ( ctx.IsProcessorActive( 0 ) )
		{
			SlVec3	Nn = op1[0].GetNormalized();
			SlColor	col( 0.0f );
			illuminate( col, pLights, actLights, *pP, Nn, illConeCosA );

			lhs[0] = col;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
// this is a simplified version.. until lights become available 8)
static void Inst_Ambient( SlRunContext &ctx )
{
	SlColor*	lhs	= (SlColor*)ctx.GetVoid( 1 );

	if NOT( ctx.mCache.mAmbientColDone )
	{
		ctx.mCache.mAmbientColDone = true;

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

		ctx.mCache.mAmbientCol = ambCol;
	}

	//DASSERT( ambCol.x >= 0 && ambCol.y >= 0 && ambCol.z >= 0 );

	for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
	{
		if ( ctx.IsProcessorActive( i ) )
			lhs[i] = ctx.mCache.mAmbientCol;
	}

	ctx.NextInstruction();
}

//==================================================================
static void Inst_Normalize( SlRunContext &ctx )
{
		  SlVec3*	lhs	= (		 SlVec3*)ctx.GetVoid( 1 );
	const SlVec3*	op1	= (const SlVec3*)ctx.GetVoid( 2 );
	
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

		if ( ctx.IsProcessorActive( 0 ) )
			lhs[0] = op1[0].GetNormalized();
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB>
inline void Inst_Noise1( SlRunContext &ctx )
{
		  SlScalar*	lhs	= (SlScalar*)ctx.GetVoid( 1 );
	const TB*		op1	= (const TB*)ctx.GetVoid( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = Noise::unoise1( op1[op1_offset] );

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = Noise::unoise1( op1[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB>
inline void Inst_Noise3( SlRunContext &ctx )
{
		  SlVec3*	lhs	= (	 SlVec3*)ctx.GetVoid( 1 );
	const TB*		op1	= (const TB*)ctx.GetVoid( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mSIMDBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = Noise::unoise3( op1[op1_offset] );

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = Noise::unoise3( op1[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
#define SINGLE	SlScalar
#define VECTOR	SlVec3
//#define MATRIX	Matrix44

//==================================================================
static ShaderInstruction	sInstructionTable[OP_N] =
{
	Inst_1Op<SINGLE,SINGLE,OP_SS_MOV>,
	0,//Inst_1Op<SINGLE,VECTOR,OP_SV_MOV>,
	Inst_1Op<VECTOR,SINGLE,OP_VS_MOV>,
	Inst_1Op<VECTOR,VECTOR,OP_VV_MOV>,

	Inst_1Op<SINGLE,SINGLE,OP_SS_ABS>,
	0,//Inst_1Op<SINGLE,VECTOR,OP_SV_ABS>,
	Inst_1Op<VECTOR,SINGLE,OP_VS_ABS>,
	Inst_1Op<VECTOR,VECTOR,OP_VV_ABS>,

	Inst_2Op<SINGLE,SINGLE,OP_SS_ADD>,
	0,//Inst_2Op<SINGLE,VECTOR,OP_SV_ADD>,
	Inst_2Op<VECTOR,SINGLE,OP_VS_ADD>,
	Inst_2Op<VECTOR,VECTOR,OP_VV_ADD>,

	Inst_2Op<SINGLE,SINGLE,OP_SS_SUB>,
	0,//Inst_2Op<SINGLE,VECTOR,OP_SV_SUB>,
	Inst_2Op<VECTOR,SINGLE,OP_VS_SUB>,
	Inst_2Op<VECTOR,VECTOR,OP_VV_SUB>,

	Inst_2Op<SINGLE,SINGLE,OP_SS_MUL>,
	0,//Inst_2Op<SINGLE,VECTOR,OP_SV_MUL>,
	Inst_2Op<VECTOR,SINGLE,OP_VS_MUL>,
	Inst_2Op<VECTOR,VECTOR,OP_VV_MUL>,

	Inst_2Op<SINGLE,SINGLE,OP_SS_DIV>,
	0,//Inst_2Op<SINGLE,VECTOR,OP_SV_DIV>,
	Inst_2Op<VECTOR,SINGLE,OP_VS_DIV>,
	Inst_2Op<VECTOR,VECTOR,OP_VV_DIV>,

	Inst_Normalize,
	Inst_Faceforward,
	Inst_Diffuse,
	Inst_Ambient,

	Inst_Noise1<SlScalar>,
	Inst_Noise1<SlVec2>,
	Inst_Noise1<SlVec3>,

	Inst_Noise3<SlScalar>,
	Inst_Noise3<SlVec2>,
	Inst_Noise3<SlVec3>,
};

//==================================================================
void SlShaderInstance::Run( SlRunContext &ctx ) const
{
	while ( ctx.mProgramCounter < mpShader->mCode.size() )
	{
		const SlCPUWord	*pWord = ctx.GetOp( 0 );
		
		// [pWord->mOpCode.mDestOpType]
		sInstructionTable[pWord->mOpCode.mTableOffset]( ctx );
	}
}

//==================================================================
}

