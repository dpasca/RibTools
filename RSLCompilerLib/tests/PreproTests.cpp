#include <cstdio>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>

#include "DSystem/include/DIO_FileManager.h"
#include "RSLCompilerLib/include/RSLC_Prepro.h"

namespace
{

class MemoryFileManager final : public DIO::FileManagerBase
{
public:
    void AddFile( std::string name, std::string contents )
    {
        mFiles.insert_or_assign( std::move( name ), std::move( contents ) );
    }

    void GrabFile( const char *pFileName, DVec<U8> &outData ) override
    {
        const auto foundIt = mFiles.find( pFileName );
        if ( foundIt == mFiles.end() )
            throw std::runtime_error( "Test file not found" );

        outData.assign( foundIt->second.begin(), foundIt->second.end() );
    }

    bool FileExists( const char *pFileName ) override
    {
        return mFiles.contains( pFileName );
    }

private:
    std::unordered_map<std::string, std::string> mFiles;
};

std::string preprocess( const std::string &sourceText )
{
    MemoryFileManager fileManager;
    fileManager.AddFile(
        "builtins.sl",
        R"RSL(#define BUILTIN_VALUE 99
)RSL" );
    fileManager.AddFile(
        "included.sl",
        R"RSL(#define INCLUDED_VALUE 23
float included = INCLUDED_VALUE;
)RSL" );

    RSLC::FatBase fatBase;
    DVec<RSLC::Fat8> source;
    fatBase.AppendNewFile(
        source,
        "prepro_test.sl",
        reinterpret_cast<const U8 *>( sourceText.data() ),
        sourceText.size() );

    DVec<RSLC::Fat8> processedSource;
    RSLC::PREPRO::Prepro preprocessor(
        fileManager,
        fatBase,
        source,
        "builtins.sl",
        ".",
        processedSource );

    std::string result;
    result.reserve( processedSource.size() );
    for (const auto &character : processedSource)
        result += static_cast<char>( character.Ch );

    return result;
}

bool expectContains(
    const std::string &text,
    const std::string &expected,
    const char *pDescription )
{
    if ( text.find( expected ) != std::string::npos )
        return true;

    std::fprintf( stderr, "Missing %s: %s\n", pDescription, expected.c_str() );
    return false;
}

bool expectNotContains(
    const std::string &text,
    const std::string &unexpected,
    const char *pDescription )
{
    if ( text.find( unexpected ) == std::string::npos )
        return true;

    std::fprintf( stderr, "Unexpected %s: %s\n", pDescription, unexpected.c_str() );
    return false;
}

} // namespace

int main()
{
    const auto result = preprocess(
        R"RSL(#include "included.sl"
#define SHORT 1
#define LONG_NAME 123456
#define EMPTY
float shortValue = SHORT;
float longValue = LONG_NAME;
float LONG_NAME_SUFFIX = 7;
string label = "LONG_NAME";
#ifdef EMPTY
float active = INCLUDED_VALUE;
#ifdef MISSING
float nestedHidden = 88;
#endif
#endif
#ifdef MISSING
#ifdef EMPTY
float nestedParentHidden = 66;
#else
float nestedElseHidden = 55;
#endif
#endif
#ifndef EMPTY
float hidden = 77;
#else
float alternative = BUILTIN_VALUE;
#endif
EMPTY;
)RSL" );

    bool success = true;
    success &= expectContains( result, "float included = 23;", "included macro expansion" );
    success &= expectContains( result, "float shortValue = 1;", "short macro expansion" );
    success &= expectContains( result, "float longValue = 123456;", "long macro expansion" );
    success &= expectContains( result, "float LONG_NAME_SUFFIX = 7;", "whole-token matching" );
    success &= expectContains( result, "string label = \"LONG_NAME\";", "string preservation" );
    success &= expectContains( result, "float active = 23;", "active conditional" );
    success &= expectContains( result, "float alternative = 99;", "else branch" );
    success &= expectNotContains( result, "nestedHidden", "nested inactive branch" );
    success &= expectNotContains( result, "nestedParentHidden", "inactive parent branch" );
    success &= expectNotContains( result, "nestedElseHidden", "else under inactive parent" );
    success &= expectNotContains( result, "float hidden = 77;", "inactive branch" );
    success &= expectNotContains( result, "EMPTY", "empty macro" );

    if ( !success )
        std::fprintf( stderr, "\nPreprocessed output:\n%s\n", result.c_str() );

    return success ? 0 : 1;
}
