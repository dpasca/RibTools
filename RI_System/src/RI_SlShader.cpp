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
#include "RI_SlShader_Ops_XForm.h"
#include "RSLCompilerLib/include/RSLCompiler.h"
#include "DSystem/include/DUtils_Files.h"

//==================================================================
//#define FORCE_MEM_CORRUPTION_CHECK

//==================================================================
namespace RI
{

//==================================================================
static void compileSLToAsm(
						DUT::MemFile &slSource,
						const char *pSrcFPathName,
						const char *pAppResDir,
						const char *pAsmOutName )
{
	// compile
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
		compiler.SaveASM( pAsmOutName, pSrcFPathName );
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
}

//==================================================================
static void compileFromMemFile(
				DUT::MemFile &file,
				SlShader *pShader,
				const char *pFileName,
				const char *pShaderName,
				const char *pAppResDir,
				FileManagerBase &fileManager )
{
	bool	isSL =
		(0 == strcasecmp( DUT::GetFileNameExt( pFileName ), "sl" ) );

	if NOT( isSL )
	{
		// compile/parse the rrasm file and return right away
		ShaderAsmParser	parser( file, pShader, pShaderName );
		return;
	}

	// make the autogen name
	char	asmOutName[1024];
	strcpy_s( asmOutName, pFileName );
	DUT::GetFileNameExt( asmOutName )[0] = 0;	// cut the extension
	strcat_s( asmOutName, "autogen.rrasm" );	// make the .autogen etc name

	DUT::MemFile	autogenAsmFile;

	// does the autogen file already exist ?
	if ( fileManager.FileExists( asmOutName ) )
	{
		// just get it from the file manager
		fileManager.GrabFile( asmOutName, autogenAsmFile );
	}
	else
	{
		// ..otherwise.. compile the sl into an autogen.rrasm
		compileSLToAsm( file, pFileName, pAppResDir, asmOutName );
		// ..and also read in the file..
		autogenAsmFile.Init( asmOutName );
	}

	// now parse/compile the rrasm file
	ShaderAsmParser	parser( autogenAsmFile, pShader, pShaderName );
}

//==================================================================
/// SlShader
//==================================================================
SlShader::SlShader( const CtorParams &params, FileManagerBase &fileManager ) :
	ResourceBase(params.pName, ResourceBase::TYPE_SHADER),
	mType(TYPE_UNKNOWN),
	mStartPC(INVALID_PC)
{
	DUT::MemFile	file;

	if ( params.pSource )
	{
		file.Init( (const void *)params.pSource, strlen(params.pSource) );

		compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pAppResDir, fileManager );
	}
	else
	if ( params.pSourceFileName )
	{
		fileManager.GrabFile( params.pSourceFileName, file );

		compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pAppResDir, fileManager );
	}
	else
	{
		DASSTHROW( 0, ("Missing parameters !") );
	}
}

//==================================================================
/// SlShaderInst
//==================================================================
SlShaderInst::SlShaderInst( SlShader *pShader, size_t maxPointsN ) :
	moShader(pShader),
	mMaxPointsN(maxPointsN)
{
}

//==================================================================
SlShaderInst::~SlShaderInst()
{
}

//==================================================================
static void matchSymbols( const Symbol &a, const Symbol &b )
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

	DASSTHROW( a.IsVarying() == b.IsVarying(),
				("Detail is %s but expecting %s for '%s'",
					a.IsVarying() ? "varying" : "not-varying",
					b.IsVarying() ? "varying" : "not-varying",
					a.mName.c_str()) );
}

//==================================================================
SlValue	*SlShaderInst::Bind(
					SymbolIList		&gridSymIList,
					DVec<u_int>		&out_defParamValsStartPCs ) const
{
	out_defParamValsStartPCs.clear();

	size_t	symbolsN = moShader->mpShaSyms.size();

	SlValue	*pDataSegment = DNEW SlValue [ symbolsN ];

	for (size_t i=0; i < symbolsN; ++i)
	{
		const Symbol		&shaSym			= *moShader->mpShaSyms[i];
		const char			*pShaSymName	= shaSym.mName.c_str();

		switch ( shaSym.mStorage )
		{
		case Symbol::STOR_CONSTANT:
			DASSERT( shaSym.IsUniform() );
			pDataSegment[i].Flags.mOwnData = 0;
			pDataSegment[i].SetDataR( shaSym.GetConstantData(), &shaSym );
			break;

		case Symbol::STOR_GLOBAL:
			{
				SymbolI	*pGridSymI = gridSymIList.FindSymbolI( pShaSymName );

				DASSTHROW( pGridSymI != NULL, ("Could not find the global %s !\n", pShaSymName) );

				matchSymbols( shaSym, *pGridSymI->mpSrcSymbol );

				pDataSegment[i].Flags.mOwnData = 0;
				pDataSegment[i].SetDataRW( pGridSymI->GetRWData(), pGridSymI->mpSrcSymbol );
			}
			break;

		case Symbol::STOR_PARAMETER:
			{
				SymbolI	*pGridSymI = gridSymIList.FindSymbolI( pShaSymName );

				if ( pGridSymI )
				{
					// verify shaSym type
					matchSymbols( shaSym, *pGridSymI->mpSrcSymbol );

					pDataSegment[i].Flags.mOwnData = 0;
					pDataSegment[i].SetDataRW( pGridSymI->GetRWData(), pGridSymI->mpSrcSymbol );
				}
				else
				{
					DASSTHROW( shaSym.IsUniform(),
								("Currently not supporting varying parameters %s", pShaSymName) );

					// (Comment below still relevant ? Probably not..)
					// $$$ when supporting varying, will have to allocate data here and fill with default
					// at setup time.. like for temporaries with default values..

					// calling params should come from "surface" ?
					const SymbolI	*pParamSymI = mCallingParams.FindSymbolI( pShaSymName );

					// additionally look into params in attributes ?

					if ( pParamSymI )
					{
						matchSymbols( shaSym, *pParamSymI->mpSrcSymbol );

						pDataSegment[i].Flags.mOwnData = 0;
						pDataSegment[i].SetDataR( pParamSymI->GetUniformParamData(), pParamSymI->mpSrcSymbol );
					}
					else
					{
						pDataSegment[i].Flags.mOwnData = 1;

						size_t	allocN = shaSym.IsVarying() ? mMaxPointsN : 1;

						pDataSegment[i].SetDataRW( shaSym.AllocClone( allocN ), &shaSym );

						if ( moShader->mpShaSymsStartPCs[i] != INVALID_PC )
							out_defParamValsStartPCs.push_back( moShader->mpShaSymsStartPCs[i] );
/*
						else
						if ( shaSym.GetUniformParamData() )
							pDataSegment[i].mpSrcSymbol->mpConstVal = 
							pDataSegment[i].SetDataRW( , &shaSym );
*/


/*
						pDataSegment[i].Flags.mOwnData = 0;
						pDataSegment[i].SetDataR( shaSym.GetUniformParamData(), &shaSym );
*/

						//DASSTHROW( 0, ("Could not find shaSym %s", pShaSymName) );
					}
				}
			}
			break;

		case Symbol::STOR_TEMPORARY:
			{
				//DASSERT( shaSym.mIsVarying == true );
				pDataSegment[i].Flags.mOwnData = 1;

				size_t	allocN = shaSym.IsVarying() ? mMaxPointsN : 1;

				pDataSegment[i].SetDataRW( shaSym.AllocClone( allocN ), &shaSym );
			}
			break;

		default:
			DASSTHROW( 0, ("Unsupported data type !") );
			break;
		}
	}

	return pDataSegment;
}

//==================================================================
void SlShaderInst::Unbind( SlValue * &pDataSegment ) const
{
	size_t	symbolsN = moShader->mpShaSyms.size();

	for (size_t i=0; i < symbolsN; ++i)
	{
		if ( pDataSegment[i].Flags.mOwnData )
			pDataSegment[i].mpSrcSymbol->FreeClone( pDataSegment[i].Data.pVoidValue );
	}

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

	const SymbolI*	pNgSymI = ctx.mpGridSymIList->FindSymbolI( "Ng" );
	const SlVec3*	pNg = (const SlVec3 *)pNgSymI->GetData();

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	bool	N_step	= ctx.IsSymbolVarying( 2 );
	bool	I_step	= ctx.IsSymbolVarying( 3 );
	bool	Ng_step	= pNgSymI->IsVarying() ? 1 : 0;

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

	const SlScalar*	pOODu	= (const SlScalar*)ctx.mpGridSymIList->FindSymbolIData( "_oodu" );
	const SlScalar*	pOODv	= (const SlScalar*)ctx.mpGridSymIList->FindSymbolIData( "_oodv" );

	// only varying input and output !
	DASSERT( ctx.IsSymbolVarying( 1 ) && ctx.IsSymbolVarying( 2 ) );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	//if ( ctx.IsProcessorActive( i ) )

	SlVec3	dPDu[ MP_GRID_MAX_SIMD_BLKS ];

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
#define S SlScalar
#define V SlVec3
//#define MATRIX	Matrix44

//==================================================================
static ShaderInstruction	sInstructionTable[OP_N] =
{
	SOP::Inst_1Op<S,S,OBT_MOV>,
	SOP::Inst_1Op<V,S,OBT_MOV>,
	SOP::Inst_1Op<V,V,OBT_MOV>,

	SOP::Inst_MovXX,

	SOP::Inst_1Op<S,S,OBT_ABS>,
	SOP::Inst_1Op<V,S,OBT_ABS>,
	SOP::Inst_1Op<V,V,OBT_ABS>,

	SOP::Inst_2Op<S,S,S,OBT_ADD>,
	SOP::Inst_2Op<V,V,S,OBT_ADD>,
	SOP::Inst_2Op<V,S,V,OBT_ADD>,
	SOP::Inst_2Op<V,V,V,OBT_ADD>,

	SOP::Inst_2Op<S,S,S,OBT_SUB>,
	SOP::Inst_2Op<V,V,S,OBT_SUB>,
	SOP::Inst_2Op<V,S,V,OBT_SUB>,
	SOP::Inst_2Op<V,V,V,OBT_SUB>,

	SOP::Inst_2Op<S,S,S,OBT_MUL>,
	SOP::Inst_2Op<V,V,S,OBT_MUL>,
	SOP::Inst_2Op<V,S,V,OBT_MUL>,
	SOP::Inst_2Op<V,V,V,OBT_MUL>,

	SOP::Inst_2Op<S,S,S,OBT_DIV>,
	SOP::Inst_2Op<V,S,V,OBT_DIV>,
	SOP::Inst_2Op<V,V,S,OBT_DIV>,
	SOP::Inst_2Op<V,V,V,OBT_DIV>,

	SOP::Inst_MOVVS3<SlVec3,SlScalar>,

	SOP::Inst_Dot_SVV,

	SOP::Inst_LD1<S>,
	SOP::Inst_LD3<V>,

	SOP::Inst_CMPLT<S>,	// cmplt

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

	SOP::Inst_PXFormname_VXV,
	SOP::Inst_VXFormname_VXV,
	SOP::Inst_NXFormname_VXV,
	SOP::Inst_CXFormname_VXV,

	Inst_Normalize,
	Inst_Faceforward,
	SOP::Inst_Diffuse,
	SOP::Inst_Ambient,
	Inst_CalculateNormal,

	NULL,	// ret
};

#undef S
#undef V

//==================================================================
void SlShaderInst::runFrom( SlRunContext &ctx, u_int startPC ) const
{
	ctx.mProgramCounterIdx = 0;
	ctx.mProgramCounter[ ctx.mProgramCounterIdx ] = startPC;

	const SlCPUWord	*pWord = NULL;

	try {
		while ( true )
		{
			if ( ctx.mProgramCounter[ctx.mProgramCounterIdx] >= moShader->mCode.size() )
				return;

			pWord = ctx.GetOp( 0 );

			if ( pWord->mOpCode.mTableOffset == OP_RET )
			{
				// what ? Subroutines ?
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
					moShader->mShaderName.c_str(),
						pWord->mOpCode.mDbgLineNum );
	}
}

//==================================================================
void SlShaderInst::Run( SlRunContext &ctx ) const
{
	// reset the program counter
	ctx.mProgramCounterIdx = 0;
	ctx.mProgramCounter[ 0 ] = INVALID_PC;

	// initialize the SIMD state
	ctx.InitializeSIMD( mMaxPointsN );

	// initialize the non uniform/constant values with eventual default data
	for (size_t i=0; i < ctx.mpShaderInst->moShader->mpShaSyms.size(); ++i)
	{
		SlValue	&slValue = ctx.mpDataSegment[i];

		if ( slValue.Flags.mOwnData &&
			 slValue.mpSrcSymbol->mpConstVal != NULL )
		{
			DASSERT(
				slValue.Data.pVoidValue != NULL &&
				slValue.mpSrcSymbol->IsConstant() );

			slValue.mpSrcSymbol->CopyConstValue( slValue.Data.pVoidValue );
		}
	}

	// run the default params fill subroutines (the instance
	// determines which need to be called)
	for (size_t i=0; i < ctx.mDefParamValsStartPCs.size(); ++i)
	{
		runFrom( ctx, ctx.mDefParamValsStartPCs[i] );
	}

	// run the main
	runFrom( ctx, moShader->mStartPC );
}

//==================================================================
}
