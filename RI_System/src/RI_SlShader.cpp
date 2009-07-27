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
#include "RI_SlShader_Ops_Base.h"
#include "RI_SlShader_Ops_Lighting.h"
#include "RI_SlShader_Ops_Noise.h"
#include "RSLCompilerLib/include/RSLCompiler.h"

//==================================================================
//#define FORCE_MEM_CORRUPTION_CHECK

//==================================================================
namespace RI
{

//==================================================================
static std::string compileSLToAsm(
						DUT::MemFile &slSource,
						const char *pSrcFPathName,
						const char *pAppResDir )
{
	// compile
	char	asmOutName[1024];

	strcpy_s( asmOutName, pSrcFPathName );
	DUT::GetFileNameExt( asmOutName )[0] = 0;	// cut the extension
	strcat_s( asmOutName, "autogen.rrasm" );	// make the .autogen etc name

	std::string	basInclude( pAppResDir );
	basInclude += "/RSLC_Builtins.sl";

	try {
		RSLCompiler::Params	params;
		params.mDbgOutputTree = false;

		// try compile
		RSLCompiler	compiler(
						(const char *)slSource.GetData(),
						slSource.GetDataSize(),
						basInclude.c_str(),
						params
					);

		// save autogen rrasm file
		compiler.SaveASM( asmOutName, pSrcFPathName );
	}
	catch ( RSLC::Exception &e )
	{
		printf(
			"ERROR: while compiling '%s'..\n%s\n",
			pSrcFPathName,
			e.GetMessage().c_str() );
	}
	catch ( ... )
	{
		printf( "ERROR while compiling '%s'\n", pSrcFPathName );
	}

	return asmOutName;
}

//==================================================================
static void compileFromMemFile(
				DUT::MemFile &file,
				SlShader *pShader,
				const char *pFileName,
				const char *pShaderName,
				const char *pAppResDir )
{
	// umm.. really ?
	bool	isSL =
		(0 == strcasecmp( DUT::GetFileNameExt( pFileName ), "sl" ) );

	if ( isSL )
	{
		std::string asmFileName = compileSLToAsm(
											file,
											pFileName,
											pAppResDir );

		DUT::MemFile	tmpFile( asmFileName.c_str() );

		ShaderAsmParser	parser( tmpFile, pShader, pShaderName );
	}
	else
	{
		ShaderAsmParser	parser( file, pShader, pShaderName );
	}
}

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader( const CtorParams &params ) :
	ResourceBase(params.pName, ResourceBase::TYPE_SHADER),
	mType(TYPE_UNKNOWN),
	mStartPC(0)
{
	if ( params.pSource )
	{
		DUT::MemFile	file((const void *)params.pSource,
							  strlen(params.pSource) );

		compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pAppResDir );
	}
	else
	if ( params.pSourceFileName )
	{
		DUT::MemFile	file( params.pSourceFileName );

		compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pAppResDir );
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
	mMaxPointsN(maxPointsN),
	mCallingParams(32)
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
static void matchSymbols( const SlSymbol &a, const SlSymbol &b )
{
	if ( strcasecmp( a.mName.c_str(), b.mName.c_str() ) )
	{
		DASSTHROW( 0, ("Names not matching ! %s != %s", a.mName.c_str(), b.mName.c_str()) );
	}

	DASSTHROW( a.mType == b.mType,
				("Type is %i but expecting %i for '%s'",
					a.mType,
					b.mType,
					a.mName.c_str()) );

	DASSTHROW( a.mIsVarying == b.mIsVarying,
				("Detail is %s but expecting %s for '%s'",
					a.mIsVarying ? "varying" : "not-varying",
					b.mIsVarying ? "varying" : "not-varying",
					a.mName.c_str()) );
}

//==================================================================
SlValue	*SlShaderInstance::Bind( const SlSymbolList &gridSymbols ) const
{
	size_t	symbolsN = mpShader->mSymbols.size();

	SlValue	*pDataSegment = DNEW SlValue [ symbolsN ];

	for (size_t i=0; i < symbolsN; ++i)
	{
		const SlSymbol	&symbol = *mpShader->mSymbols[i];

		const char		*pFindName = symbol.mName.c_str();

		switch ( symbol.mStorage )
		{
		case SlSymbol::CONSTANT:
			DASSERT( symbol.mIsVarying == false );
			pDataSegment[i].Flags.mOwnData = 0;
			pDataSegment[i].SetDataR( symbol.GetConstantData(), &symbol );
			break;

		case SlSymbol::GLOBAL:
			{
				const SlSymbol	*pFoundSymbol = gridSymbols.LookupVariable( pFindName );

				DASSTHROW( pFoundSymbol != NULL, ("Could not find the global %s !", pFindName) );

				matchSymbols( symbol, *pFoundSymbol );

				pDataSegment[i].Flags.mOwnData = 0;
				pDataSegment[i].SetDataRW( ((SlSymbol *)pFoundSymbol)->GetRWData(), pFoundSymbol );
			}
			break;

		case SlSymbol::PARAMETER:
			{
				const SlSymbol	*pFoundSymbol = gridSymbols.LookupVariable( pFindName );

				if ( pFoundSymbol )
				{
					matchSymbols( symbol, *pFoundSymbol );

					pDataSegment[i].Flags.mOwnData = 0;
					pDataSegment[i].SetDataRW( ((SlSymbol *)pFoundSymbol)->GetRWData(), pFoundSymbol );
				}
				else
				{
					DASSTHROW( symbol.mIsVarying == false,
								("Currently not supporting varying parameters %s", pFindName) );

					// $$$ when supporting varying, will have to allocate data here and fill with default
					// at setup time.. like for temporaries with default values..

					// calling params should come from "surface" ?
					pFoundSymbol = mCallingParams.LookupVariable( pFindName );

					// additionally look into params in attributes ?

					if ( pFoundSymbol )
					{
						matchSymbols( symbol, *pFoundSymbol );

						pDataSegment[i].Flags.mOwnData = 0;
						pDataSegment[i].SetDataR( pFoundSymbol->GetUniformParamData(), pFoundSymbol );
					}
					else
					{
						pDataSegment[i].Flags.mOwnData = 0;
						pDataSegment[i].SetDataR( symbol.GetUniformParamData(), &symbol );

						//DASSTHROW( 0, ("Could not find symbol %s", pFindName) );
					}
				}
			}
			break;

		case SlSymbol::TEMPORARY:
			//DASSERT( symbol.mIsVarying == true );
			pDataSegment[i].Flags.mOwnData = 1;
			if ( symbol.mIsVarying )
				pDataSegment[i].SetDataRW( symbol.AllocClone( mMaxPointsN ), &symbol );
			else
				pDataSegment[i].SetDataRW( symbol.AllocClone( 1 ), &symbol );
			break;

		default:
			DASSTHROW( 0, ("Unsupported data type !") );
			break;
		}
	}

	return pDataSegment;
}

//==================================================================
void SlShaderInstance::Unbind( SlValue * &pDataSegment ) const
{
	size_t	symbolsN = mpShader->mSymbols.size();

/*
	for (size_t i=0; i < symbolsN; ++i)
	{
		if ( pDataSegment[i].Flags.mOwnData )
			pDataSegment[i].mpSrcSymbol->FreeClone( pDataSegment[i].Data.pVoidValue );
	}
*/

	DSAFE_DELETE_ARRAY( pDataSegment );
	pDataSegment = NULL;
}

//==================================================================
typedef void (*ShaderInstruction)( SlRunContext &ctx );

//==================================================================
static void Inst_Faceforward( SlRunContext &ctx )
{
		  SlVec3* lhs	= ctx.GetVoidRW( (		SlVec3 *)0, 1 );
	const SlVec3* pN	= ctx.GetVoidRO( (const SlVec3 *)0, 2 );
	const SlVec3* pI	= ctx.GetVoidRO( (const SlVec3 *)0, 3 );

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

		for (u_int i=0; i < ctx.mBlocksN; ++i)
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
static void Inst_Normalize( SlRunContext &ctx )
{
		  SlVec3*	lhs	= ctx.GetVoidRW( (		SlVec3 *)0, 1 );
	const SlVec3*	op1	= ctx.GetVoidRO( (const SlVec3 *)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
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
static void Inst_CalculateNormal( SlRunContext &ctx )
{
		  SlVec3*	lhs	= ctx.GetVoidRW( (		SlVec3 *)0, 1 );
	const SlVec3*	op1	= ctx.GetVoidRO( (const SlVec3 *)0, 2 );

	const SlScalar*	pOODu	= (const SlScalar*)ctx.mpSymbols->LookupVariableData( "oodu", SlSymbol::FLOAT );
	const SlScalar*	pOODv	= (const SlScalar*)ctx.mpSymbols->LookupVariableData( "oodv", SlSymbol::FLOAT );

	// only varying input and output !
	DASSERT( ctx.IsSymbolVarying( 1 ) && ctx.IsSymbolVarying( 2 ) );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	//if ( ctx.IsProcessorActive( i ) )

	SlVec3	dPDu[ MicroPolygonGrid::MAX_SIMD_BLKS ];

	{
		u_int	blk = 0;
		for (u_int iy=0; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				const SlVec3	&blkOp1 = op1[blk];
				SlVec3			&blkdPDu = dPDu[blk];

				for (u_int sub=1; sub < RI_SIMD_BLK_LEN; ++sub)
				{
					blkdPDu[0][sub] = blkOp1[0][sub] - blkOp1[0][sub-1];
					blkdPDu[1][sub] = blkOp1[1][sub] - blkOp1[1][sub-1];
					blkdPDu[2][sub] = blkOp1[2][sub] - blkOp1[2][sub-1];
				}

				blkdPDu[0][0] = blkdPDu[0][1];
				blkdPDu[1][0] = blkdPDu[1][1];
				blkdPDu[2][0] = blkdPDu[2][1];

				blkdPDu = blkdPDu * pOODu[blk];
			}
		}
	}

	{
		u_int	blk = ctx.mBlocksXN;
		for (u_int iy=1; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				SlVec3	dPDv = (op1[blk] - op1[blk - ctx.mBlocksXN]) * pOODv[blk];

				lhs[blk] = dPDu[blk].GetCross( dPDv ).GetNormalized();
			}
		}

		for (u_int blk=0; blk < ctx.mBlocksXN; ++blk)
			lhs[blk] = lhs[blk + ctx.mBlocksXN];
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
	SOP::Inst_1Op<SINGLE,SINGLE,OP_SS_MOV>,
	0,//SOP::Inst_1Op<SINGLE,VECTOR,OP_SV_MOV>,
	SOP::Inst_1Op<VECTOR,SINGLE,OP_VS_MOV>,
	SOP::Inst_1Op<VECTOR,VECTOR,OP_VV_MOV>,

	SOP::Inst_1Op<SINGLE,SINGLE,OP_SS_ABS>,
	0,//SOP::Inst_1Op<SINGLE,VECTOR,OP_SV_ABS>,
	SOP::Inst_1Op<VECTOR,SINGLE,OP_VS_ABS>,
	SOP::Inst_1Op<VECTOR,VECTOR,OP_VV_ABS>,

	SOP::Inst_2Op<SINGLE,SINGLE,OP_SS_ADD>,
	0,//SOP::Inst_2Op<SINGLE,VECTOR,OP_SV_ADD>,
	SOP::Inst_2Op<VECTOR,SINGLE,OP_VS_ADD>,
	SOP::Inst_2Op<VECTOR,VECTOR,OP_VV_ADD>,

	SOP::Inst_2Op<SINGLE,SINGLE,OP_SS_SUB>,
	0,//SOP::Inst_2Op<SINGLE,VECTOR,OP_SV_SUB>,
	SOP::Inst_2Op<VECTOR,SINGLE,OP_VS_SUB>,
	SOP::Inst_2Op<VECTOR,VECTOR,OP_VV_SUB>,

	SOP::Inst_2Op<SINGLE,SINGLE,OP_SS_MUL>,
	0,//SOP::Inst_2Op<SINGLE,VECTOR,OP_SV_MUL>,
	SOP::Inst_2Op<VECTOR,SINGLE,OP_VS_MUL>,
	SOP::Inst_2Op<VECTOR,VECTOR,OP_VV_MUL>,

	SOP::Inst_2Op<SINGLE,SINGLE,OP_SS_DIV>,
	0,//SOP::Inst_2Op<SINGLE,VECTOR,OP_SV_DIV>,
	SOP::Inst_2Op<VECTOR,SINGLE,OP_VS_DIV>,
	SOP::Inst_2Op<VECTOR,VECTOR,OP_VV_DIV>,

	SOP::Inst_MOVVS3<SlVec3,SlScalar>,

	SOP::Inst_LD1<SINGLE>,
	SOP::Inst_LD3<VECTOR>,

	SOP::Inst_CMPLT<SINGLE>,	// cmplt

	SOP::Inst_Noise1<SlScalar>,
	SOP::Inst_Noise1<SlVec2>,
	SOP::Inst_Noise1<SlVec3>,

	SOP::Inst_Noise3<SlScalar>,
	SOP::Inst_Noise3<SlVec2>,
	SOP::Inst_Noise3<SlVec3>,

	SOP::Inst_GetVComp<0>,
	SOP::Inst_GetVComp<1>,
	SOP::Inst_GetVComp<2>,
	SOP::Inst_SetVComp<0>,
	SOP::Inst_SetVComp<1>,
	SOP::Inst_SetVComp<2>,

	Inst_Normalize,
	Inst_Faceforward,
	SOP::Inst_Diffuse,
	SOP::Inst_Ambient,
	Inst_CalculateNormal,

	NULL,	// ret
};

//==================================================================
void SlShaderInstance::Run( SlRunContext &ctx ) const
{
	const SlCPUWord	*pWord = NULL;

	try {
		while ( true )
		{
			if ( ctx.mProgramCounter[ctx.mProgramCounterIdx] >= mpShader->mCode.size() )
				return;

			pWord = ctx.GetOp( 0 );

			if ( pWord->mOpCode.mTableOffset == OP_RET )
			{
				if ( ctx.mProgramCounterIdx == 0 )
					return;

				ctx.mProgramCounterIdx -= 1;
			}

			// [pWord->mOpCode.mDestOpType]
			sInstructionTable[pWord->mOpCode.mTableOffset]( ctx );

#if defined(FORCE_MEM_CORRUPTION_CHECK)
			const char *pDude = DNEW char;
			delete pDude;
#endif
		}
	}
	catch ( ... )
	{
		printf( "SHADER ERROR: %s failed at line %i !!\n",
					mpShader->mShaderName.c_str(),
						pWord->mOpCode.mDbgLineNum );
	}
}

//==================================================================
}
