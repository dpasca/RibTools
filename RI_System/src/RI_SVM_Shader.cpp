//==================================================================
/// RI_SVM_Shader.cpp
///
/// Created by Davide Pasca - 2009/2/19
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Shader.h"
#include "RI_SVM_Context.h"
#include "RI_RRASM_Parser.h"
#include "RI_Attributes.h"
#include "RI_State.h"
#include "RI_SVM_OpCodeFuncs.h"
#include "RSLCompilerLib/include/RSLCompiler.h"
#include "DSystem/include/DUtils_Files.h"

#include "RI_RRASM_OpCodeDefs.h"

//==================================================================
//#define FORCE_MEM_CORRUPTION_CHECK

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
static void compileSLToAsm(
                        DIO::FileManagerBase &fmanager,
                        DUT::MemFile &slSource,
                        const char *pSrcFPathName,
                        const char *pBaseIncDir,
                        const char *pAsmOutName )
{
    // compile
    std::string	basInclude( pBaseIncDir );
    basInclude += "/RSLC_Builtins.sl";

    try {
        RSLCompiler::Params	params;
        params.mDbgOutputTree = false;
        params.mpFileManager = &fmanager;

        // try compile
        RSLCompiler	compiler(
                        pSrcFPathName,
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
            "%s"
            "SHADER ERR> FROM: %s\n\n",
            e.GetMessage().c_str(),
            pSrcFPathName
            );
    }
    catch ( ... )
    {
        printf(
            "SHADER ERR> FROM: %s\n\n",
            pSrcFPathName
            );
    }
}

//==================================================================
static void compileFromMemFile(
                DUT::MemFile &file,
                Shader *pShader,
                const char *pFileName,
                const char *pShaderName,
                const char *pBaseIncDir,
                DIO::FileManagerBase &fileManager )
{
    bool	isSL =
        (0 == strcasecmp( DUT::GetFileNameExt( pFileName ), "sl" ) );

    if NOT( isSL )
    {
        // compile/parse the rrasm file and return right away
        RRASM::Parser	parser( file, pShader, pShaderName );
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
        compileSLToAsm( fileManager, file, pFileName, pBaseIncDir, asmOutName );
        // ..and also read in the file..
        autogenAsmFile.Init( asmOutName );
    }

    // now parse/compile the rrasm file
    RRASM::Parser	parser( autogenAsmFile, pShader, pShaderName );
}

//==================================================================
/// Shader
//==================================================================
Shader::Shader( const CtorParams &params, DIO::FileManagerBase &fileManager ) :
    ResourceBase(params.pName, ResourceBase::TYPE_SHADER),
    mType(TYPE_UNKNOWN),
    mStartPC(INVALID_PC),
    mHasDirPosInstructions(false)
{
    DUT::MemFile	file;

    if ( params.pSource )
    {
        file.Init( (const void *)params.pSource, strlen(params.pSource) );

        compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pBaseIncDir, fileManager );
    }
    else
    if ( params.pSourceFileName )
    {
        fileManager.GrabFile( params.pSourceFileName, file );

        compileFromMemFile( file, this, params.pSourceFileName, params.pName, params.pBaseIncDir, fileManager );
    }
    else
    {
        DASSTHROW( 0, ("Missing parameters !") );
    }
}

//==================================================================
/// ShaderInst
//==================================================================
ShaderInst::ShaderInst( Shader *pShader, size_t maxPointsN ) :
    moShader(pShader),
    mMaxPointsN(maxPointsN)
{
    DASSERT( pShader != NULL );
}

//==================================================================
ShaderInst::~ShaderInst()
{
}

//==================================================================
inline bool ShaderInst::verifyOpParams( Context &ctx, const RRASM::OpCodeDef &opCodeDef ) const
{
#ifdef _DEBUG

    // special case, only check fir the 1st type
    if ( opCodeDef.Flags & RRASM::OPC_FLG_RIGHTISIMM )
        return ctx.isOpcodeTypeCompatible( 1, opCodeDef.Types[0] );

    for (u_int i=0; i < opCodeDef.OperCnt; ++i)
    {
        // don't che address.. it's not a symbol
        if ( opCodeDef.Types[i] == Symbol::TYP_ADDR )
            continue;

        if NOT( ctx.isOpcodeTypeCompatible( i + 1, opCodeDef.Types[i] ) )
        {
            DASSERT( 0 );
            return false;
        }
    }

#endif

    return true;
}

//==================================================================
inline u_int getDestBlocksN( Context &ctx, const RRASM::OpCodeDef &opCodeDef )
{
    if ( 0 != (opCodeDef.Flags & RRASM::OPC_FLG_1STISDEST) && ctx.IsSymbolVarying( 1 ) )
        return ctx.mBlocksN;
    else
        return 1;
}

//==================================================================
void ShaderInst::runFrom( Context &ctx, u_int startPC ) const
{
    ctx.mProgramCounterIdx = 0;
    ctx.mProgramCounter[ ctx.mProgramCounterIdx ] = startPC;
    ctx.mIlluminanceCtx.Reset();
    ctx.mFopStack.clear();

    const CPUWord	*pWord = NULL;

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

            u_int					opCodeIdx = pWord->mOpCode.mTableOffset;

            const RRASM::OpCodeDef	&opCodeDef = RRASM::_gOpCodeDefs[ opCodeIdx ];

            // only an assert because by now the RRASM should have
            // verified this !
            verifyOpParams( ctx, opCodeDef );

            u_int	blocksN = getDestBlocksN( ctx, opCodeDef );

            // get the opcode functions
            SlOpCodeFunc	nextFunc = _gSlOpCodeFuncs[ opCodeIdx ];
            // execute the opcode !
            nextFunc( ctx, blocksN );

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
void ShaderInst::Run( Context &ctx ) const
{
    // reset the program counter
    ctx.mProgramCounterIdx = 0;
    ctx.mProgramCounter[ 0 ] = INVALID_PC;

    // initialize the SIMD state
    ctx.InitializeSIMD( mMaxPointsN );

    // initialize the non uniform/constant values with eventual default data
    for (size_t i=0; i < ctx.mpShaderInst->moShader->mpShaSyms.size(); ++i)
    {
        Value	&slValue = ctx.mpDataSegment[i];

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
//==================================================================
}
