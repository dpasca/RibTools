#include <cmath>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <limits>
#include <memory>
#include <string>
#include <system_error>
#include <vector>

#include <jpeglib.h>

#include "DImage/include/DImage.h"
#include "DImage/include/DImage_JPEG.h"
#include "DImage/include/DImage_TIFF.h"

namespace
{

class JpegDecompressor
{
public:
    JpegDecompressor()
    {
        mInfo.err = jpeg_std_error( &mError );
        jpeg_create_decompress( &mInfo );
    }

    ~JpegDecompressor()
    {
        jpeg_destroy_decompress( &mInfo );
    }

    jpeg_decompress_struct mInfo {};
    jpeg_error_mgr mError {};
};

bool testWrappedPixels()
{
    DIMG::Image image( 2, 2, 1, DIMG::Image::ST_U8, -1, "y" );
    image.GetPixelPtrRW( 0, 0 )[0] = 1;
    image.GetPixelPtrRW( 1, 0 )[0] = 2;
    image.GetPixelPtrRW( 0, 1 )[0] = 3;
    image.GetPixelPtrRW( 1, 1 )[0] = 4;

    U8 *pixels[4] {};
    image.GetPixelQuadWrapR( -1, -1, pixels );

    return
        pixels[0][0] == 4 &&
        pixels[1][0] == 3 &&
        pixels[2][0] == 2 &&
        pixels[3][0] == 1;
}

bool testU32Conversion()
{
    DIMG::Image image( 1, 1, 1, DIMG::Image::ST_U32, -1, "y" );
    const float input[DIMG::Image::MAX_SAMP_PER_PIX] { 1.0f };
    image.SetPixelFromFloat( 0, 0, input );

    const auto stored = *reinterpret_cast<const U32 *>(image.GetPixelPtrR( 0, 0 ));
    if ( stored != std::numeric_limits<U32>::max() )
        return false;

    float output[DIMG::Image::MAX_SAMP_PER_PIX] {};
    image.GetPixelAsFloat( 0, 0, output );
    return std::fabs( output[0] - 1.0f ) < 0.000001f;
}

bool testNonFiniteIntegerConversion()
{
    const float input[DIMG::Image::MAX_SAMP_PER_PIX] {
        std::numeric_limits<float>::quiet_NaN(),
        std::numeric_limits<float>::infinity(),
        -std::numeric_limits<float>::infinity(),
        0.5f
    };

    DIMG::Image imageU8( 1, 1, 4, DIMG::Image::ST_U8, -1, "rgba" );
    imageU8.SetPixelFromFloat( 0, 0, input );
    const auto *pU8 = reinterpret_cast<const U8 *>(imageU8.GetPixelPtrR( 0, 0 ));
    if ( pU8[0] != 0 || pU8[1] != 255 || pU8[2] != 0 || pU8[3] != 127 )
        return false;

    DIMG::Image imageU16( 1, 1, 4, DIMG::Image::ST_U16, -1, "rgba" );
    imageU16.SetPixelFromFloat( 0, 0, input );
    const auto *pU16 = reinterpret_cast<const U16 *>(imageU16.GetPixelPtrR( 0, 0 ));
    if (
        pU16[0] != 0 ||
        pU16[1] != std::numeric_limits<U16>::max() ||
        pU16[2] != 0 ||
        pU16[3] != 32767 )
    {
        return false;
    }

    DIMG::Image imageU32( 1, 1, 4, DIMG::Image::ST_U32, -1, "rgba" );
    imageU32.SetPixelFromFloat( 0, 0, input );
    const auto *pU32 = reinterpret_cast<const U32 *>(imageU32.GetPixelPtrR( 0, 0 ));
    return
        pU32[0] == 0 &&
        pU32[1] == std::numeric_limits<U32>::max() &&
        pU32[2] == 0 &&
        pU32[3] == 2147483647U;
}

bool testSampleNames()
{
    DIMG::Image image( 1, 1, 4, DIMG::Image::ST_U8, -1, "rgba" );
    return image.IsSamplesNames( "rgba" ) && !image.IsSamplesNames( "rgb" );
}

bool testTIFFRoundTrip( const std::filesystem::path &outputDirectory )
{
    const auto outputPath = outputDirectory / "DImageTests-roundtrip.tiff";
    const std::string outputName = outputPath.string();

    DIMG::Image source( 3, 2, 3, DIMG::Image::ST_U8, -1, "rgb" );
    for (U32 y = 0; y < source.mHe; ++y)
    {
        for (U32 x = 0; x < source.mWd; ++x)
        {
            U8 *pPixel = source.GetPixelPtrRW( x, y );
            pPixel[0] = static_cast<U8>(20 + x * 30);
            pPixel[1] = static_cast<U8>(40 + y * 50);
            pPixel[2] = static_cast<U8>(60 + x * 10 + y * 20);
        }
    }

    DIMG::SaveTIFF( source, outputName.c_str() );

    std::ifstream inputStream( outputPath, std::ios::binary );
    const auto fileBegin = std::istreambuf_iterator<char>(inputStream);
    const auto fileEnd = std::istreambuf_iterator<char>();
    const std::vector<U8> fileData( fileBegin, fileEnd );
    if ( fileData.empty() )
        return false;

    DUT::MemFile inputFile( fileData.data(), fileData.size() );
    DIMG::Image loaded;
    DIMG::LoadTIFF( loaded, inputFile, outputName.c_str() );

    if (
        loaded.mWd != source.mWd ||
        loaded.mHe != source.mHe ||
        loaded.mSampPerPix != source.mSampPerPix ||
        !loaded.IsSamplesNames( "rgb" ) )
    {
        return false;
    }

    for (U32 y = 0; y < source.mHe; ++y)
    {
        for (U32 x = 0; x < source.mWd; ++x)
        {
            if ( std::memcmp(
                    source.GetPixelPtrR( x, y ),
                    loaded.GetPixelPtrR( x, y ),
                    source.mBytesPerPix ) != 0 )
            {
                return false;
            }
        }
    }

    std::error_code error;
    std::filesystem::remove( outputPath, error );
    return true;
}

bool testJPEGConversionAndStride( const std::filesystem::path &outputDirectory )
{
    const auto outputPath = outputDirectory / "DImageTests-conversion.jpg";
    const std::string outputName = outputPath.string();

    constexpr U32 Width = 15;
    constexpr U32 Height = 8;
    DIMG::Image source( Width, Height, 3, DIMG::Image::ST_F32, -1, "rgb" );

    for (U32 y = 0; y < Height; ++y)
    {
        for (U32 x = 0; x < Width; ++x)
        {
            const bool isLeft = x < Width / 2;
            const float samples[DIMG::Image::MAX_SAMP_PER_PIX] {
                isLeft ? 1.0f : 0.0f,
                isLeft ? 0.0f : 1.0f,
                0.0f,
                0.0f
            };
            source.SetPixelFromFloat( x, y, samples );
        }
    }

    DIMG::SaveJPEG( source, outputName.c_str() );

    FILE *pRawFile = std::fopen( outputName.c_str(), "rb" );
    if ( pRawFile == nullptr )
        return false;
    std::unique_ptr<FILE, decltype(&std::fclose)> file( pRawFile, &std::fclose );

    JpegDecompressor decompressor;
    auto &info = decompressor.mInfo;
    jpeg_stdio_src( &info, file.get() );

    if ( jpeg_read_header( &info, TRUE ) != JPEG_HEADER_OK )
        return false;

    info.out_color_space = JCS_RGB;
    if ( jpeg_start_decompress( &info ) != TRUE )
        return false;

    if (
        info.output_width != Width ||
        info.output_height != Height ||
        info.output_components != 3 )
    {
        return false;
    }

    const size_t rowSize =
        static_cast<size_t>(info.output_width) * info.output_components;
    std::vector<JSAMPLE> pixels( rowSize * info.output_height );

    while ( info.output_scanline < info.output_height )
    {
        JSAMPLE *pRow =
            pixels.data() + static_cast<size_t>(info.output_scanline) * rowSize;
        if ( jpeg_read_scanlines( &info, &pRow, 1 ) != 1 )
            return false;
    }

    if ( jpeg_finish_decompress( &info ) != TRUE )
        return false;

    const auto pixelAt = [&]( U32 x, U32 y )
    {
        return pixels.data() + static_cast<size_t>(y) * rowSize + x * 3;
    };

    const JSAMPLE *pRed = pixelAt( 2, Height / 2 );
    const JSAMPLE *pGreen = pixelAt( Width - 3, Height / 2 );
    const bool colorsAreCorrect =
        pRed[0] > 180 && pRed[1] < 80 && pRed[2] < 80 &&
        pGreen[0] < 80 && pGreen[1] > 180 && pGreen[2] < 80;

    std::error_code error;
    std::filesystem::remove( outputPath, error );
    return colorsAreCorrect;
}

} // namespace

int main( int argc, char **argv )
{
    if ( argc != 2 )
        return 1;

    if ( !testWrappedPixels() )
        return 2;

    if ( !testU32Conversion() )
        return 3;

    if ( !testNonFiniteIntegerConversion() )
        return 4;

    if ( !testSampleNames() )
        return 5;

    const std::filesystem::path outputDirectory( argv[1] );
    if ( !testTIFFRoundTrip( outputDirectory ) )
        return 6;

    if ( !testJPEGConversionAndStride( outputDirectory ) )
        return 7;

    return 0;
}
