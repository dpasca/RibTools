#include <cctype>
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

constexpr auto gpUnimplementedOpcodeSource = R"RRASM(
.code
surface
__main:
    seteq.bmm      $bu1 leftMatrix rightMatrix
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
        RI::RRASM::OPC_FLG_UNIMPLEMENTED;

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
                    std::initializer_list<const char *> requiredOpcodes = {} )
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
        return 5;
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
        return 6;
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
            return 7;
        }
    }

    return 0;
}
