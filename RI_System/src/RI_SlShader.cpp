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
#include "RI_SlShader_Ops_Misc.h"
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
	// this shouldn't really happen at this stage
	DASSTHROW( a.IsName( b.GetNameChr() ),
		("Names not matching ! %s != %s", a.GetNameChr(), b.GetNameChr()) );

	DASSTHROW( a.mType == b.mType,
				("Type is %i but expecting %i for '%s'",
					a.mType,
					b.mType,
					a.mName.c_str()) );

	DASSTHROW( a.IsVarying() == b.IsVarying(),
				("Class is %s but expecting %s for '%s'",
					a.IsVarying() ? "varying" : "not-varying",
					b.IsVarying() ? "varying" : "not-varying",
					a.mName.c_str()) );

	DASSTHROW( a.IsConstant() == b.IsConstant(),
				("Class is %s but expecting %s for '%s'",
					a.IsConstant() ? "constant" : "not-constant",
					b.IsConstant() ? "constant" : "not-constant",
					a.mName.c_str()) );
}

//==================================================================
static void bindGlobalStorageSymbol(
						SlValue				&destValue	,
						const Symbol		&shaSym		,
						SymbolIList			&gridSymIList,
						const SymbolList	&globalSyms
						)
{
	// instances of all symbols considered as globals for the shader are already
	// in the grid, therefore we look into the grid
	// Notice that in the state machine, symbols such as "Ka" are marked as global,
	// but in fact, those are considered as parameters in the shader.. while they
	// are global in the context of the state machine.. basically a different kind
	// of globals !

	if ( shaSym.IsConstant() )
	{
		// Example symbols: PI

		// find the global symbol in the globals from the state machine

		const Symbol	*pGlobalSym = globalSyms.FindSymbol( shaSym.GetNameChr() );

		DASSTHROW( pGlobalSym != NULL, ("Could not find the global symbol %s !\n", shaSym.GetNameChr()) );

		matchSymbols( shaSym, *pGlobalSym );

		// grab the constant data from the global in the state machine.. not from the
		// symbol in the shader.. which is not supposed to exist 8)
		destValue.Flags.mOwnData = 0;
		destValue.SetDataR( pGlobalSym->GetConstantData(), pGlobalSym );
	}
	else
	{
		// Example symbols: P, Cs, N, Ng ...

		// find the global symbol's instance in the grid

		SymbolI	*pGridGlobalSymI = gridSymIList.FindSymbolI( shaSym.GetNameChr() );

		DASSTHROW( pGridGlobalSymI != NULL, ("Could not find the global symbol %s !\n", shaSym.GetNameChr()) );

		matchSymbols( shaSym, *pGridGlobalSymI->mpSrcSymbol );

		// grab the data from the symbol the symbol instance in the grid
		destValue.Flags.mOwnData = 0;
		destValue.SetDataRW( pGridGlobalSymI->GetRWData(), pGridGlobalSymI->mpSrcSymbol );
	}
}

//==================================================================
static void bindTemporaryStorageSymbol(
						SlValue				&destValue	,
						const Symbol		&shaSym		,
						size_t				maxPoints
						)
{
	if ( shaSym.IsConstant() )
	{
		// Example symbols: MY_FACTOR ...

		// temporary const is a local const ..grab it from the shader

		destValue.Flags.mOwnData = 0;
		destValue.SetDataR( shaSym.GetConstantData(), &shaSym );
	}
	else
	{
		// Example symbols: temp2, $su1, $v1 ...
		// Note: Indeed, $ registers are considered as temporaries

		// temporary non-const requires local allocation

		size_t	allocN = shaSym.IsVarying() ? maxPoints : 1;

		destValue.Flags.mOwnData = 1;
		destValue.SetDataRW( shaSym.AllocClone( allocN ), &shaSym );
	}
}

//==================================================================
SlValue	*SlShaderInst::Bind(
					const SymbolList	&globalSyms,
					SymbolIList			&gridSymIList,
					DVec<u_int>			&out_defParamValsStartPCs ) const
{
	out_defParamValsStartPCs.clear();

	size_t	symbolsN = moShader->mpShaSyms.size();

	SlValue	*pDataSegment = DNEW SlValue [ symbolsN ];

	for (size_t i=0; i < symbolsN; ++i)
	{
		const Symbol		&shaSym			= *moShader->mpShaSyms[i];

		switch ( shaSym.mStorage )
		{
		case Symbol::STOR_GLOBAL:
			bindGlobalStorageSymbol(
						pDataSegment[i],
						shaSym,
						gridSymIList,
						globalSyms );
			break;

		case Symbol::STOR_PARAMETER:
			{
				// paranoia check for the moment..
				DASSERT( NULL == gridSymIList.FindSymbolI( shaSym.GetNameChr() ) );

				/*
				//---- Handle in-line params

				// will need to ensure that in-line params are uniform and constant !

				const SymbolI	*pParamSymI = mCallSymIList.FindSymbolI( pShaSymName );

				// additionally look into params in attributes ?

				if ( pParamSymI )
				{
					matchSymbols( shaSym, *pParamSymI->mpSrcSymbol );

					pDataSegment[i].Flags.mOwnData = 0;
					pDataSegment[i].SetDataR( pParamSymI->GetUniformParamData(), pParamSymI->mpSrcSymbol );
				}
				*/

				SlValue	&destValue = pDataSegment[i];

				// is the param constant ?  ..odd, but...
				if ( shaSym.IsConstant() )
				{
					// parameter const is a local const ..grab it from the shader
					destValue.Flags.mOwnData = 0;
					destValue.SetDataR( shaSym.GetConstantData(), &shaSym );
				}
				else
				{
					// allocate storage for the param
					size_t	allocN = shaSym.IsVarying() ? mMaxPointsN : 1;

					pDataSegment[i].Flags.mOwnData = 1;
					pDataSegment[i].SetDataRW( shaSym.AllocClone( allocN ), &shaSym );

					// setup init code !
					if ( moShader->mpShaSymsStartPCs[i] != INVALID_PC )
						out_defParamValsStartPCs.push_back( moShader->mpShaSymsStartPCs[i] );
				}
			}
			break;

		case Symbol::STOR_TEMPORARY:
			bindTemporaryStorageSymbol( pDataSegment[i], shaSym, mMaxPointsN );
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
void Inst_SolarBegin_VS( SlRunContext &ctx )
{
	const SlVec3*	pAxis	= ctx.GetVoidRO( (		SlVec3 *)0, 1 );
	const SlScalar*	pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 2 );

	// SlColor*	pCl	= (		SlColor	*)ctx.mpGridSymIList->FindSymbolIData( "Cl" );

	SlVec3		*pL	= (SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	DASSTHROW( !ctx.mSolarCtx.IsActive(), ("Nested solar ?!") );

	ctx.NextInstruction();

	u_int bodyStartAddr = ctx.GetCurPC();

	ctx.mSolarCtx.Init(
			bodyStartAddr,
			pAxis,
			pAngle,
			pL
			);

	// TODO: setup L and increase light idx
}

//==================================================================
void Inst_FuncopEnd( SlRunContext &ctx )
{
	// are we doing solar ?
	if ( ctx.mSolarCtx.IsActive() )
	{
		// TODO: check light idx, and eventually setup the new L
		// and go back to beginning of statement (or else fall through)
		//ctx.GotoInstruction( ctx.mSolarCtx.mBodyStartAddr );

		ctx.NextInstruction();
	}
	else
	{
		DASSTHROW( 0, ("'funcop' not matching !") );
	}
}

//==================================================================
#define S SlScalar
#define V SlVec3
//#define MATRIX	Matrix44

//==================================================================
static ShaderInstruction	sInstructionTable[] =
{
	NULL,	// ret

	SOP::Inst_1Op<S,S,OBT_MOV>,
	SOP::Inst_1Op<V,S,OBT_MOV>,
	SOP::Inst_1Op<V,V,OBT_MOV>,

	SOP::Inst_MovXX,

	SOP::Inst_1Op<S,S,OBT_ABS>,
	SOP::Inst_1Op<V,S,OBT_ABS>,
	SOP::Inst_1Op<V,V,OBT_ABS>,

	SOP::Inst_Sign<S,S>,

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

	SOP::Inst_Normalize,
	SOP::Inst_Faceforward,
	SOP::Inst_Diffuse,
	SOP::Inst_Ambient,
	SOP::Inst_CalculateNormal,

	Inst_SolarBegin_VS,
	Inst_FuncopEnd,
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
