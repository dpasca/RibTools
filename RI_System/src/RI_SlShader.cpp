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
typedef void (*ShaderInstruction)( SlRunContext &ctx );

//==================================================================
void Inst_SolarBegin_VS( SlRunContext &ctx )
{
	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	const SlVec3	*pAxis	= ctx.GetVoidRO( (		  SlVec3 *)0, 1 );
	const SlScalar	*pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 2 );

	SlVec3		*pL		= (SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	// 'L' becomes the normalized axis
	*pL = pAxis->GetNormalized();

	// TODO: check with angle to see if we need to skip this
	// start looping by the funcop end
	if ( 0 )
	{
		// if we are skipping then Cl = 0 (do we really need it ?)

		SlColor	*pCl = (SlColor *)ctx.mpGridSymIList->FindSymbolIData( "Cl" );
		*pCl = SlColor( 0 );

		ctx.GotoInstruction( ctx.GetOp(0)->mOpCode.mFuncopEndAddr );
	}

	ctx.NextInstruction();
}

//==================================================================
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Illuminance( SlRunContext &ctx )
{
	u_short funcOpEndAddr = ctx.GetOp(0)->mOpCode.mFuncopEndAddr;

	// any lights ?
	if NOT( ctx.mpAttribs->mActiveLights.size() )
	{
		// no ? Skip the whole thing
		ctx.GotoInstruction( funcOpEndAddr );
		ctx.NextInstruction();
		return;
	}

	const SlVec3*	pPos	= ctx.GetVoidRO( (		SlVec3 *)0, 1 );
	const SlVec3*	pAxis	;
	const SlScalar*	pAngle	;

	if ( INCLUDES_AXIS_ANGLE )
	{
		pAxis	= ctx.GetVoidRO( (		  SlVec3 *)0, 2 );
		pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 3 );
	}
	else
	{
		pAxis	= NULL;
		pAngle	= NULL;
	}

	SlColor	*pCl	= (	SlColor	*)ctx.mpGridSymIList->FindSymbolIData( "Cl" );
	SlVec3	*pL		= (	 SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	ctx.NextInstruction();
	u_int bodyStartAddr = ctx.GetCurPC();

	ctx.mSlIlluminanceCtx.Init(
			bodyStartAddr,
			pAxis,
			pAngle,
			pL,
			ctx.mpAttribs->mActiveLights.size()
			);

	// start looping by the funcop end
	ctx.GotoInstruction( funcOpEndAddr );
}

//==================================================================
void Inst_FuncopEnd( SlRunContext &ctx )
{
	// are we doing illuminance ?
	if ( ctx.mSlIlluminanceCtx.IsActive() )
	{
		if ( ctx.mSlIlluminanceCtx.Next() )
		{
			ctx.mSlIlluminanceCtx.Reset();
			ctx.NextInstruction();
		}
		else
			ctx.GotoInstruction( ctx.mSlIlluminanceCtx.mBodyStartAddr );
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
	Inst_Illuminance<true>,
	Inst_Illuminance<false>,
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
