//==================================================================
/// DImage_JPEG.cpp
///
/// Created by Davide Pasca - 2010/1/29
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <cstdio>
#include <limits>
#include <memory>

#include <jpeglib.h>

#include "DImage_JPEG.h"

namespace
{

class JpegCompressor
{
public:
    JpegCompressor()
    {
        mInfo.err = jpeg_std_error( &mError );
        jpeg_create_compress( &mInfo );
    }

    ~JpegCompressor()
    {
        jpeg_destroy_compress( &mInfo );
    }

    jpeg_compress_struct mInfo {};
    jpeg_error_mgr mError {};
};

void writeJPEGFile(
    const char *pFileName,
    int quality,
    const JSAMPLE *pImageBuffer,
    size_t rowStride,
    U32 imageWidth,
    U32 imageHeight )
{
    DASSTHROW(
        imageWidth <= std::numeric_limits<JDIMENSION>::max() &&
        imageHeight <= std::numeric_limits<JDIMENSION>::max(),
        "Image dimensions are too large for JPEG output %s",
        pFileName );

    FILE *pRawFile = nullptr;
    DASSTHROW(
        fopen_s( &pRawFile, pFileName, "wb" ) == 0,
        "Could not open %s for write",
        pFileName );
    std::unique_ptr<FILE, decltype(&fclose)> file( pRawFile, &fclose );

    JpegCompressor compressor;
    auto &info = compressor.mInfo;

    jpeg_stdio_dest( &info, file.get() );

    info.image_width = static_cast<JDIMENSION>(imageWidth);
    info.image_height = static_cast<JDIMENSION>(imageHeight);
    info.input_components = 3;
    info.in_color_space = JCS_RGB;

    jpeg_set_defaults( &info );
    jpeg_set_quality( &info, quality, TRUE );
    jpeg_start_compress( &info, TRUE );

    while ( info.next_scanline < info.image_height )
    {
        auto *pRow = const_cast<JSAMPLE *>(
            pImageBuffer + static_cast<size_t>(info.next_scanline) * rowStride );
        jpeg_write_scanlines( &info, &pRow, 1 );
    }

    jpeg_finish_compress( &info );
}

} // namespace

namespace DIMG
{

//==================================================================
void SaveJPEG( const Image &img, const char *pFName )
{
    const bool needsConversion =
        !img.IsSamplesNames( "rgb" ) ||
        !img.IsSamplesType( Image::ST_U8 );

    if ( needsConversion )
    {
        Image convertedImage( img.mWd, img.mHe, 3, Image::ST_U8, -1, "rgb" );
        ConvertImages( convertedImage, img );
        writeJPEGFile(
            pFName,
            100,
            convertedImage.GetPixelPtrR( 0, 0 ),
            convertedImage.mBytesPerRow,
            convertedImage.mWd,
            convertedImage.mHe );
    }
    else
    {
        writeJPEGFile(
            pFName,
            100,
            img.GetPixelPtrR( 0, 0 ),
            img.mBytesPerRow,
            img.mWd,
            img.mHe );
    }
}

//==================================================================
}
