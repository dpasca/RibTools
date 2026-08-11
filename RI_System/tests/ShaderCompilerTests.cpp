#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <exception>
#include <filesystem>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <system_error>
#include <unordered_set>
#include <utility>

#include "DSystem/include/DIO_FileManager.h"
#include "RI_System/include/RI_RRASM_OpCodeDefs.h"
#include "RI_System/include/RI_SVM_Context.h"
#include "RI_System/include/RI_SVM_OpCodeFuncs.h"
#include "RI_System/include/RI_SVM_Shader.h"
#include "RSLCompilerLib/include/RSLCompiler.h"

namespace
{

constexpr auto gpUnaryShaderSource = R"RSL(
surface unary_shader_load( float scale = 2 )
{
    float negativeScale = -scale;
    vector negativeDirection = -I;

    Ci = color( negativeScale );
    Oi = 1;
}
)RSL";

constexpr auto gpArrayDeclarationShaderSource = R"RSL(
surface array_declaration_load( float values[2] )
{
    Ci = 1;
    Oi = 1;
}
)RSL";

constexpr auto gpFloatArrayShaderSource = R"RSL(
surface float_array_load( float selectedIndex = 1 )
{
    float red[2];
    float green[2];
    float blue[2];

    red[0] = 1;
    red[1] = 0;
    green[0] = 0;
    green[1] = 1;
    blue[0] = blue[1] = 0;

    Ci = color(
        red[selectedIndex],
        green[selectedIndex],
        blue[selectedIndex] );
    Oi = 1;
}
)RSL";

constexpr auto gpUnsupportedEnvironmentShaderSource = R"RSL(
surface unsupported_environment( string textureName = "reflection.tx" )
{
    Ci = color environment( textureName, I );
    Oi = 1;
}
)RSL";

constexpr auto gpUnsupportedAlphaShaderSource = R"RSL(
imager unsupported_alpha()
{
    Ci = (1 - alpha) * Ci;
}
)RSL";

constexpr auto gpInvalidArrayIndexShaderSource = R"RSL(
surface invalid_array_index()
{
    float values[2];
    Ci = values[2];
    Oi = 1;
}
)RSL";

constexpr auto gpInvalidArraySizeShaderSource = R"RSL(
surface invalid_array_size()
{
    float values[0];
    Ci = 1;
    Oi = 1;
}
)RSL";

constexpr auto gpUnsupportedArrayTypeShaderSource = R"RSL(
surface unsupported_array_type()
{
    color values[2];
    Ci = 1;
    Oi = 1;
}
)RSL";

constexpr auto gpScalarIndexShaderSource = R"RSL(
surface scalar_index()
{
    float value = 1;
    Ci = value[0];
    Oi = 1;
}
)RSL";

constexpr auto gpInvalidArrayArgumentShaderSource = R"RSL(
void use_array( float values[2] )
{
    Oi = values[0];
}

surface invalid_array_argument()
{
    float values[2];
    use_array( values[0] );
}
)RSL";

constexpr auto gpArrayChainedAssignmentShaderSource = R"RSL(
surface array_chained_assignment()
{
    float values[2];
    values[0] = values[1] = 1;

    Ci = color( values[0], values[1], 0 );
    Oi = 1;
}
)RSL";

constexpr auto gpUnimplementedOpcodeSource = R"RRASM(
.code
surface
__main:
    seteq.bmm      $bu1 leftMatrix rightMatrix
    ret
)RRASM";

constexpr auto gpArrayExecutionSource = R"RRASM(
.data
source      temporary varying float
values      temporary varying array 2 float
index       temporary varying float
result      temporary varying float

.code
surface
__main:
    arrst.sss      source values index
    arrld.sss      result values index
    ret
)RRASM";

constexpr auto gpUniformIfExecutionSource = R"RRASM(
.data
condition   temporary uniform  float
result      temporary uniform  float
zero        temporary constant float   0
one         temporary constant float   1

.code
surface
__main:
    seteq.bss      $bu1 condition zero
    iftrue.b       $bu1
    mov.ss         result zero
    orelse
    mov.ss         result one
    funcopend
    ret
)RRASM";

class ScopedFileRemoval
{
public:
    explicit ScopedFileRemoval( std::filesystem::path path ) :
        mPath( std::move( path ) )
    {
    }

    ~ScopedFileRemoval()
    {
        std::error_code error;
        std::filesystem::remove( mPath, error );
    }

private:
    std::filesystem::path mPath;
};

bool shaderContainsOpcode(
                    const RI::SVM::Shader &shader,
                    const char *pOpcodeName );

std::string normalizedOpcodeName( const char *pName )
{
    std::string name( pName );
    for (auto &character : name)
    {
        character = static_cast<char>(
            std::tolower( static_cast<unsigned char>(character) ) );
    }

    return name;
}

bool validateOpcodeContract()
{
    const size_t definitionCount = RI::RRASM::GetOpCodeDefCount();
    const size_t callbackCount = RI::SVM::GetSlOpCodeFuncCount();
    if ( definitionCount != callbackCount )
    {
        std::fprintf(
            stderr,
            "Opcode definition/callback count mismatch: %zu definitions, %zu callbacks\n",
            definitionCount,
            callbackCount );
        return false;
    }

    const u_int knownFlags =
        RI::RRASM::OPC_FLG_1STISDEST |
        RI::RRASM::OPC_FLG_RIGHTISIMM |
        RI::RRASM::OPC_FLG_UNIFORMOPERS |
        RI::RRASM::OPC_FLG_FUNCOP_BEGIN |
        RI::RRASM::OPC_FLG_FUNCOP_MIDDLE |
        RI::RRASM::OPC_FLG_FUNCOP_END |
        RI::RRASM::OPC_FLG_DIRPOSLIGHT |
        RI::RRASM::OPC_FLG_VM_HANDLED |
        RI::RRASM::OPC_FLG_UNIMPLEMENTED |
        RI::RRASM::OPC_FLG_2NDISARRAY;

    std::unordered_set<std::string> opcodeNames;
    for (size_t opcodeIndex = 0; opcodeIndex < definitionCount; ++opcodeIndex)
    {
        const auto &definition = RI::RRASM::_gOpCodeDefs[opcodeIndex];
        const auto callback = RI::SVM::_gSlOpCodeFuncs[opcodeIndex];
        const size_t typeSlotCount =
            sizeof(definition.Types) / sizeof(definition.Types[0]);

        if ( definition.pName == nullptr || definition.pName[0] == '\0' )
        {
            std::fprintf( stderr, "Opcode %zu has no name\n", opcodeIndex );
            return false;
        }

        const bool wasInserted =
            opcodeNames.insert( normalizedOpcodeName( definition.pName ) ).second;
        if ( !wasInserted )
        {
            std::fprintf( stderr, "Duplicate opcode name '%s'\n", definition.pName );
            return false;
        }

        if ( definition.OperCnt > typeSlotCount )
        {
            std::fprintf(
                stderr,
                "Opcode '%s' declares %u operands but only %zu type slots\n",
                definition.pName,
                definition.OperCnt,
                typeSlotCount );
            return false;
        }

        for (size_t operandIndex = 0; operandIndex < typeSlotCount; ++operandIndex)
        {
            const bool operandIsDeclared = operandIndex < definition.OperCnt;
            const bool typeIsDeclared =
                definition.Types[operandIndex] != RI::Symbol::TYP_UNKNOWN;
            if ( operandIsDeclared != typeIsDeclared )
            {
                std::fprintf(
                    stderr,
                    "Opcode '%s' has inconsistent type metadata at operand %zu\n",
                    definition.pName,
                    operandIndex + 1 );
                return false;
            }
        }

        if ( definition.Flags & ~knownFlags )
        {
            std::fprintf( stderr, "Opcode '%s' uses unknown flags\n", definition.pName );
            return false;
        }

        if (
            (definition.Flags & RI::RRASM::OPC_FLG_1STISDEST) &&
            definition.OperCnt == 0 )
        {
            std::fprintf( stderr, "Opcode '%s' has a destination flag but no operands\n", definition.pName );
            return false;
        }

        if (
            (definition.Flags & RI::RRASM::OPC_FLG_RIGHTISIMM) &&
            definition.OperCnt < 2 )
        {
            std::fprintf( stderr, "Opcode '%s' has an immediate flag but no immediate operand\n", definition.pName );
            return false;
        }

        if (
            (definition.Flags & RI::RRASM::OPC_FLG_2NDISARRAY) &&
            definition.OperCnt < 2 )
        {
            std::fprintf( stderr, "Opcode '%s' has no second array operand\n", definition.pName );
            return false;
        }

        const int functionFlowFlagCount =
            ((definition.Flags & RI::RRASM::OPC_FLG_FUNCOP_BEGIN) != 0) +
            ((definition.Flags & RI::RRASM::OPC_FLG_FUNCOP_MIDDLE) != 0) +
            ((definition.Flags & RI::RRASM::OPC_FLG_FUNCOP_END) != 0);
        if ( functionFlowFlagCount > 1 )
        {
            std::fprintf( stderr, "Opcode '%s' has conflicting flow-control flags\n", definition.pName );
            return false;
        }

        const bool callbackExpected =
            !(definition.Flags & RI::RRASM::OPC_FLG_VM_HANDLED) &&
            !(definition.Flags & RI::RRASM::OPC_FLG_UNIMPLEMENTED);

        if (
            (definition.Flags & RI::RRASM::OPC_FLG_VM_HANDLED) &&
            (definition.Flags & RI::RRASM::OPC_FLG_UNIMPLEMENTED) )
        {
            std::fprintf( stderr, "Opcode '%s' has conflicting execution flags\n", definition.pName );
            return false;
        }

        if ( (callback != nullptr) != callbackExpected )
        {
            std::fprintf(
                stderr,
                "Opcode '%s' has an inconsistent VM callback\n",
                definition.pName );
            return false;
        }
    }

    return true;
}

bool rejectsUnimplementedOpcode( DIO::FileManagerBase &fileManager )
{
    RI::SVM::Shader::CtorParams params;
    params.pName = "unimplemented_opcode";
    params.pSource = gpUnimplementedOpcodeSource;
    params.pSourceFileName = "unimplemented_opcode.rrasm";

    try
    {
        RI::SVM::Shader shader( params, fileManager );
    }
    catch ( const std::runtime_error &exception )
    {
        return std::strstr( exception.what(), "not implemented" ) != nullptr;
    }

    std::fprintf( stderr, "RRASM accepted an opcode without a VM implementation\n" );
    return false;
}

bool compilerRejectsSourceWithMessage(
                    const char *pSourceName,
                    const char *pSource,
                    const char *pExpectedMessage,
                    const char *pBuiltinsPath,
                    DIO::FileManagerBase &fileManager )
{
    try
    {
        RSLCompiler::Params compilerParams;
        compilerParams.mpFileManager = &fileManager;
        RSLCompiler compiler(
            pSourceName,
            pSource,
            std::strlen( pSource ),
            pBuiltinsPath,
            compilerParams );
    }
    catch ( const RSLC::Exception &exception )
    {
        const auto &message = exception.GetMessage();
        if (
            std::strstr( message.c_str(), pExpectedMessage ) != nullptr &&
            std::strstr( message.c_str(), pSourceName ) != nullptr )
        {
            return true;
        }

        std::fprintf(
            stderr,
            "Compiler rejected '%s' with an unexpected diagnostic:\n%s\n",
            pSourceName,
            message.c_str() );
        return false;
    }

    std::fprintf( stderr, "Compiler unexpectedly accepted '%s'\n", pSourceName );
    return false;
}

RI::SVM::Value *findShaderValue(
                    const RI::SVM::Shader &shader,
                    RI::SVM::Value *pValues,
                    const char *pName )
{
    for (size_t symbolIndex = 0; symbolIndex < shader.mpShaSyms.size(); ++symbolIndex)
    {
        if ( shader.mpShaSyms[symbolIndex]->IsName( pName ) )
            return pValues + symbolIndex;
    }

    return nullptr;
}

bool executesVaryingFloatArray( DIO::FileManagerBase &fileManager )
{
    RI::SVM::Shader::CtorParams shaderParams;
    shaderParams.pName = "array_execution";
    shaderParams.pSource = gpArrayExecutionSource;
    shaderParams.pSourceFileName = "array_execution.rrasm";
    RI::SVM::Shader shader( shaderParams, fileManager );

    if (
        !shaderContainsOpcode( shader, "arrld.sss" ) ||
        !shaderContainsOpcode( shader, "arrst.sss" ) )
    {
        std::fprintf( stderr, "Array execution shader is missing array opcodes\n" );
        return false;
    }

    constexpr size_t pointCount = DMT_SIMD_FLEN * 2 + 1;
    RI::SVM::ShaderInst shaderInstance( &shader, pointCount );
    RI::SymbolList globalSymbols;
    RI::SymbolIList gridSymbols;
    RI::SVM::Context context( gridSymbols, pointCount );
    context.mpShaderInst = &shaderInstance;
    context.mpDataSegment = shaderInstance.Bind(
        globalSymbols,
        gridSymbols,
        context.mDefParamValsStartPCs );

    auto *pSourceValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "source" );
    auto *pIndexValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "index" );
    auto *pResultValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "result" );
    if ( pSourceValue == nullptr || pIndexValue == nullptr || pResultValue == nullptr )
    {
        std::fprintf( stderr, "Array execution shader is missing test values\n" );
        return false;
    }

    auto *pSource = static_cast<Float_ *>(pSourceValue->Data.pVoidValue);
    auto *pIndex = static_cast<Float_ *>(pIndexValue->Data.pVoidValue);
    for (size_t pointIndex = 0; pointIndex < pointCount; ++pointIndex)
    {
        const size_t blockIndex = DMT_SIMD_BLK_IDX( pointIndex );
        const size_t laneIndex = DMT_SIMD_SUB_IDX( pointIndex );
        pSource[blockIndex][laneIndex] = 10.0f + static_cast<float>(pointIndex);
        pIndex[blockIndex][laneIndex] = static_cast<float>(pointIndex % 2);
    }

    shaderInstance.Run( context );

    const auto *pResult = static_cast<const Float_ *>(pResultValue->Data.pConstVoidValue);
    for (size_t pointIndex = 0; pointIndex < pointCount; ++pointIndex)
    {
        const size_t blockIndex = DMT_SIMD_BLK_IDX( pointIndex );
        const size_t laneIndex = DMT_SIMD_SUB_IDX( pointIndex );
        if (
            std::fabs(
                pResult[blockIndex][laneIndex] -
                pSource[blockIndex][laneIndex] ) > 0.0001f )
        {
            std::fprintf(
                stderr,
                "Array round-trip failed at point %zu: got %g, expected %g\n",
                pointIndex,
                pResult[blockIndex][laneIndex],
                pSource[blockIndex][laneIndex] );
            return false;
        }
    }

    return true;
}

bool executesUniformIf( DIO::FileManagerBase &fileManager )
{
    RI::SVM::Shader::CtorParams shaderParams;
    shaderParams.pName = "uniform_if_execution";
    shaderParams.pSource = gpUniformIfExecutionSource;
    shaderParams.pSourceFileName = "uniform_if_execution.rrasm";
    RI::SVM::Shader shader( shaderParams, fileManager );

    RI::SVM::ShaderInst shaderInstance( &shader, 1 );
    RI::SymbolList globalSymbols;
    RI::SymbolIList gridSymbols;
    RI::SVM::Context context( gridSymbols, 1 );
    context.mpShaderInst = &shaderInstance;
    context.mpDataSegment = shaderInstance.Bind(
        globalSymbols,
        gridSymbols,
        context.mDefParamValsStartPCs );

    auto *pConditionValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "condition" );
    auto *pResultValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "result" );
    auto *pConditionMaskValue = findShaderValue(
        shader,
        context.mpDataSegment,
        "$bu1" );
    if (
        pConditionValue == nullptr ||
        pResultValue == nullptr ||
        pConditionMaskValue == nullptr )
    {
        std::fprintf( stderr, "Uniform-if shader is missing test values\n" );
        return false;
    }

    auto *pCondition = static_cast<Float_ *>(pConditionValue->Data.pVoidValue);
    auto *pResult = static_cast<Float_ *>(pResultValue->Data.pVoidValue);
    for (size_t caseIndex = 0; caseIndex < 2; ++caseIndex)
    {
        *pCondition = Float_( static_cast<float>(caseIndex) );
        *pResult = Float_( -1.0f );
        shaderInstance.Run( context );

        const float expected = static_cast<float>(caseIndex);
        if ( std::fabs((*pResult)[0] - expected) > 0.0001f )
        {
            const auto *pConditionMask = static_cast<const VecNMask *>(
                pConditionMaskValue->Data.pConstVoidValue );
            std::fprintf(
                stderr,
                "Uniform if failed for condition %zu: got %g, expected %g "
                "(mask is %s)\n",
                caseIndex,
                (*pResult)[0],
                expected,
                *pConditionMask == VecNMaskFull ? "full" : "not full" );

            for (size_t wordIndex = 0; wordIndex < shader.mCode.size();)
            {
                const auto &opcode = shader.mCode[wordIndex].mOpCode;
                const auto &definition =
                    RI::RRASM::_gOpCodeDefs[opcode.mTableOffset];
                if (
                    std::strcmp( definition.pName, "iftrue.b" ) == 0 ||
                    std::strcmp( definition.pName, "orelse" ) == 0 )
                {
                    std::fprintf(
                        stderr,
                        "%s at %zu points to %u\n",
                        definition.pName,
                        wordIndex,
                        opcode.mFuncopEndAddr );
                }

                wordIndex += 1 + opcode.mOperandCount;
            }
            return false;
        }
    }

    return true;
}

bool shaderContainsOpcode(
                    const RI::SVM::Shader &shader,
                    const char *pOpcodeName )
{
    for (size_t wordIndex = 0; wordIndex < shader.mCode.size();)
    {
        const auto &opcode = shader.mCode[wordIndex].mOpCode;
        const auto &definition = RI::RRASM::_gOpCodeDefs[opcode.mTableOffset];
        if ( std::strcmp( definition.pName, pOpcodeName ) == 0 )
            return true;

        wordIndex += 1 + opcode.mOperandCount;
    }

    return false;
}

bool validateChainedArrayStores( const RI::SVM::Shader &shader )
{
    size_t firstSourceSymbol = shader.mpShaSyms.size();
    size_t storeCount = 0;

    for (size_t wordIndex = 0; wordIndex < shader.mCode.size();)
    {
        const auto &opcode = shader.mCode[wordIndex].mOpCode;
        const auto &definition = RI::RRASM::_gOpCodeDefs[opcode.mTableOffset];
        if ( std::strcmp( definition.pName, "arrst.sss" ) == 0 )
        {
            const size_t sourceSymbol =
                shader.mCode[wordIndex + 1].mSymbol.mTableOffset;
            if ( storeCount == 0 )
                firstSourceSymbol = sourceSymbol;
            else if ( sourceSymbol != firstSourceSymbol )
            {
                std::fprintf(
                    stderr,
                    "Chained array stores use different source values\n" );
                return false;
            }

            ++storeCount;
        }

        wordIndex += 1 + opcode.mOperandCount;
    }

    if ( storeCount != 2 )
    {
        std::fprintf(
            stderr,
            "Expected two chained array stores, found %zu\n",
            storeCount );
        return false;
    }

    return true;
}

bool validateLoadedShader(
                    const RI::SVM::Shader &shader,
                    const char *pSourceName )
{
    if ( shader.mType == RI::SVM::Shader::TYPE_UNKNOWN || shader.mCode.empty() )
    {
        std::fprintf( stderr, "Shader '%s' did not produce a loadable program\n", pSourceName );
        return false;
    }

    const size_t opcodeCount = RI::RRASM::GetOpCodeDefCount();
    for (size_t wordIndex = 0; wordIndex < shader.mCode.size();)
    {
        const auto &opcode = shader.mCode[wordIndex].mOpCode;
        if ( opcode.mTableOffset >= opcodeCount )
        {
            std::fprintf( stderr, "Shader '%s' contains an invalid opcode index\n", pSourceName );
            return false;
        }

        const auto &definition = RI::RRASM::_gOpCodeDefs[opcode.mTableOffset];
        if ( opcode.mOperandCount != definition.OperCnt )
        {
            std::fprintf(
                stderr,
                "Shader '%s' loaded opcode '%s' with the wrong operand count\n",
                pSourceName,
                definition.pName );
            return false;
        }

        if ( definition.Flags & RI::RRASM::OPC_FLG_UNIMPLEMENTED )
        {
            std::fprintf(
                stderr,
                "Shader '%s' loaded unimplemented opcode '%s'\n",
                pSourceName,
                definition.pName );
            return false;
        }

        const size_t nextInstruction = wordIndex + 1 + opcode.mOperandCount;
        if ( nextInstruction > shader.mCode.size() )
        {
            std::fprintf( stderr, "Shader '%s' has a truncated instruction\n", pSourceName );
            return false;
        }

        wordIndex = nextInstruction;
    }

    return true;
}

bool compileAndLoadShader(
                    const std::string &sourceName,
                    const char *pSource,
                    size_t sourceSize,
                    const std::filesystem::path &assemblyPath,
                    const char *pBuiltinsPath,
                    DIO::FileManagerBase &fileManager,
                    std::initializer_list<const char *> requiredOpcodes = {},
                    bool requireChainedArrayStores = false )
{
    ScopedFileRemoval removeAssembly( assemblyPath );

    try
    {
        RSLCompiler::Params compilerParams;
        compilerParams.mpFileManager = &fileManager;
        RSLCompiler compiler(
            sourceName.c_str(),
            pSource,
            sourceSize,
            pBuiltinsPath,
            compilerParams );

        const std::string assemblyName = assemblyPath.string();
        compiler.SaveASM( assemblyName.c_str(), sourceName.c_str() );

        const std::string shaderName =
            std::filesystem::path( sourceName ).stem().string();
        RI::SVM::Shader::CtorParams shaderParams;
        shaderParams.pName = shaderName.c_str();
        shaderParams.pSourceFileName = assemblyName.c_str();
        RI::SVM::Shader shader( shaderParams, fileManager );

        if ( !validateLoadedShader( shader, sourceName.c_str() ) )
            return false;

        for (const char *pOpcodeName : requiredOpcodes)
        {
            if ( !shaderContainsOpcode( shader, pOpcodeName ) )
            {
                std::fprintf(
                    stderr,
                    "Shader '%s' did not emit required opcode '%s'\n",
                    sourceName.c_str(),
                    pOpcodeName );
                return false;
            }
        }

        if (
            requireChainedArrayStores &&
            !validateChainedArrayStores( shader ) )
        {
            return false;
        }
    }
    catch ( const RSLC::Exception &exception )
    {
        std::fprintf(
            stderr,
            "Shader contract compilation failed for '%s':\n%s\n",
            sourceName.c_str(),
            exception.GetMessage().c_str() );
        return false;
    }
    catch ( const std::exception &exception )
    {
        std::fprintf(
            stderr,
            "Shader contract load failed for '%s': %s\n",
            sourceName.c_str(),
            exception.what() );
        return false;
    }

    return true;
}

bool compileAndLoadFile(
                    const char *pSourceName,
                    const std::filesystem::path &assemblyPath,
                    const char *pBuiltinsPath,
                    DIO::FileManagerBase &fileManager )
{
    DVec<U8> source;
    fileManager.GrabFile( pSourceName, source );
    if ( source.empty() )
    {
        std::fprintf( stderr, "Shader source '%s' is empty\n", pSourceName );
        return false;
    }

    return compileAndLoadShader(
        pSourceName,
        reinterpret_cast<const char *>(source.data()),
        source.size(),
        assemblyPath,
        pBuiltinsPath,
        fileManager );
}

} // namespace

int main( int argc, char **ppArgs )
{
    if ( argc < 4 )
    {
        std::fprintf(
            stderr,
            "Expected an output directory, the built-ins file, and shader source files\n" );
        return 1;
    }

    const std::filesystem::path outputDirectory( ppArgs[1] );
    std::error_code directoryError;
    std::filesystem::create_directories( outputDirectory, directoryError );
    if ( directoryError )
    {
        std::fprintf(
            stderr,
            "Could not create test output directory '%s': %s\n",
            outputDirectory.string().c_str(),
            directoryError.message().c_str() );
        return 2;
    }

    DUT::InstallFileManagerStd();
    DIO::FileManagerDisk fileManager;

    if ( !validateOpcodeContract() )
        return 3;

    if ( !rejectsUnimplementedOpcode( fileManager ) )
        return 4;

    if ( !compilerRejectsSourceWithMessage(
            "unsupported_environment.sl",
            gpUnsupportedEnvironmentShaderSource,
            "Built-in function 'environment' is not implemented",
            ppArgs[2],
            fileManager ) )
    {
        return 5;
    }

    if ( !compilerRejectsSourceWithMessage(
            "unsupported_alpha.sl",
            gpUnsupportedAlphaShaderSource,
            "Global variable 'alpha' is not implemented",
            ppArgs[2],
            fileManager ) )
    {
        return 6;
    }

    if ( !executesVaryingFloatArray( fileManager ) )
        return 7;

    if ( !executesUniformIf( fileManager ) )
        return 8;

    if ( !compilerRejectsSourceWithMessage(
            "invalid_array_index.sl",
            gpInvalidArrayIndexShaderSource,
            "Array index 2 is outside 'values[2]'",
            ppArgs[2],
            fileManager ) )
    {
        return 9;
    }

    if ( !compilerRejectsSourceWithMessage(
            "invalid_array_size.sl",
            gpInvalidArraySizeShaderSource,
            "Array size must be positive integral value",
            ppArgs[2],
            fileManager ) )
    {
        return 10;
    }

    if ( !compilerRejectsSourceWithMessage(
            "unsupported_array_type.sl",
            gpUnsupportedArrayTypeShaderSource,
            "Only float arrays are currently implemented",
            ppArgs[2],
            fileManager ) )
    {
        return 11;
    }

    if ( !compilerRejectsSourceWithMessage(
            "scalar_index.sl",
            gpScalarIndexShaderSource,
            "Variable 'value' is not an array",
            ppArgs[2],
            fileManager ) )
    {
        return 12;
    }

    if ( !compilerRejectsSourceWithMessage(
            "invalid_array_argument.sl",
            gpInvalidArrayArgumentShaderSource,
            "Could not find a matching function 'use_array'",
            ppArgs[2],
            fileManager ) )
    {
        return 13;
    }

    const std::string inlineSourceName =
        (outputDirectory / "inline_unary.sl").string();
    if ( !compileAndLoadShader(
            inlineSourceName,
            gpUnaryShaderSource,
            std::strlen( gpUnaryShaderSource ),
            outputDirectory / "inline_unary.rrasm",
            ppArgs[2],
            fileManager,
            { "neg.ss", "neg.vv" } ) )
    {
        return 14;
    }

    const std::string arraySourceName =
        (outputDirectory / "inline_array_declaration.sl").string();
    if ( !compileAndLoadShader(
            arraySourceName,
            gpArrayDeclarationShaderSource,
            std::strlen( gpArrayDeclarationShaderSource ),
            outputDirectory / "inline_array_declaration.rrasm",
            ppArgs[2],
            fileManager ) )
    {
        return 15;
    }

    const std::string floatArraySourceName =
        (outputDirectory / "inline_float_array.sl").string();
    if ( !compileAndLoadShader(
            floatArraySourceName,
            gpFloatArrayShaderSource,
            std::strlen( gpFloatArrayShaderSource ),
            outputDirectory / "inline_float_array.rrasm",
            ppArgs[2],
            fileManager,
            { "arrld.sss", "arrst.sss" } ) )
    {
        return 16;
    }

    const std::string chainedArraySourceName =
        (outputDirectory / "inline_array_chained_assignment.sl").string();
    if ( !compileAndLoadShader(
            chainedArraySourceName,
            gpArrayChainedAssignmentShaderSource,
            std::strlen( gpArrayChainedAssignmentShaderSource ),
            outputDirectory / "inline_array_chained_assignment.rrasm",
            ppArgs[2],
            fileManager,
            { "arrld.sss", "arrst.sss" },
            true ) )
    {
        return 17;
    }

    for (int sourceIndex = 3; sourceIndex < argc; ++sourceIndex)
    {
        const auto assemblyName =
            "corpus_" + std::to_string( sourceIndex - 3 ) + ".rrasm";
        if ( !compileAndLoadFile(
                ppArgs[sourceIndex],
                outputDirectory / assemblyName,
                ppArgs[2],
                fileManager ) )
        {
            return 18;
        }
    }

    return 0;
}
