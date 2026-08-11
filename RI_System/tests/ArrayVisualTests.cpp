#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <vector>

#include "DImage/include/DImage.h"
#include "DImage/include/DImage_TIFF.h"
#include "DSystem/include/DUtils_MemFile.h"

namespace
{

bool validateArrayRender( const std::filesystem::path &imagePath )
{
    std::ifstream inputStream( imagePath, std::ios::binary );
    const auto fileBegin = std::istreambuf_iterator<char>(inputStream);
    const auto fileEnd = std::istreambuf_iterator<char>();
    const std::vector<U8> fileData( fileBegin, fileEnd );
    if ( fileData.empty() )
    {
        std::fprintf(
            stderr,
            "Array visual regression image '%s' is missing or empty\n",
            imagePath.string().c_str() );
        return false;
    }

    DUT::MemFile inputFile( fileData.data(), fileData.size() );
    DIMG::Image image;
    DIMG::LoadTIFF( image, inputFile, imagePath.string().c_str() );
    if ( image.mWd < 32 || image.mHe < 16 || image.mSampPerPix < 3 )
    {
        std::fprintf( stderr, "Array visual regression image has an invalid format\n" );
        return false;
    }

    size_t redPixelCount = 0;
    size_t greenPixelCount = 0;
    size_t bluePixelCount = 0;
    size_t redXTotal = 0;
    size_t greenXTotal = 0;

    for (U32 y = 0; y < image.mHe; ++y)
    {
        for (U32 x = 0; x < image.mWd; ++x)
        {
            float samples[DIMG::Image::MAX_SAMP_PER_PIX] {};
            image.GetPixelAsFloat( x, y, samples );

            const bool isRed =
                samples[0] > 0.2f &&
                samples[0] > samples[1] * 2.0f &&
                samples[0] > samples[2] * 2.0f;
            const bool isGreen =
                samples[1] > 0.2f &&
                samples[1] > samples[0] * 2.0f &&
                samples[1] > samples[2] * 2.0f;
            const bool isBlue =
                samples[2] > 0.2f &&
                samples[2] > samples[0] * 2.0f &&
                samples[2] > samples[1] * 2.0f;

            if ( isRed )
            {
                ++redPixelCount;
                redXTotal += x;
            }
            if ( isGreen )
            {
                ++greenPixelCount;
                greenXTotal += x;
            }
            if ( isBlue )
                ++bluePixelCount;
        }
    }

    const size_t minimumColorPixels =
        static_cast<size_t>(image.mWd) * image.mHe / 40;
    if (
        redPixelCount < minimumColorPixels ||
        greenPixelCount < minimumColorPixels ||
        bluePixelCount != 0 )
    {
        std::fprintf(
            stderr,
            "Unexpected array render colors: red=%zu, green=%zu, blue=%zu\n",
            redPixelCount,
            greenPixelCount,
            bluePixelCount );
        return false;
    }

    const size_t redCenterX = redXTotal / redPixelCount;
    const size_t greenCenterX = greenXTotal / greenPixelCount;
    if ( redCenterX >= image.mWd / 2 || greenCenterX <= image.mWd / 2 )
    {
        std::fprintf(
            stderr,
            "Array render objects are in the wrong order: red=%zu, green=%zu\n",
            redCenterX,
            greenCenterX );
        return false;
    }

    std::printf(
        "Array visual regression: %zu red pixels at x=%zu, %zu green pixels at x=%zu\n",
        redPixelCount,
        redCenterX,
        greenPixelCount,
        greenCenterX );
    return true;
}

} // namespace

int main( int argc, char **ppArgs )
{
    if ( argc != 2 )
    {
        std::fprintf( stderr, "Expected the rendered TIFF path\n" );
        return 1;
    }

    try
    {
        return validateArrayRender( ppArgs[1] ) ? 0 : 2;
    }
    catch ( const std::exception &exception )
    {
        std::fprintf( stderr, "Could not validate array render: %s\n", exception.what() );
        return 3;
    }
}
