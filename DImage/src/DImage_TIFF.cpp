//==================================================================
/// DImage_TIFF.cpp
///
/// Created by Davide Pasca - 2010/1/20
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

#include <tiffio.h>

#include "DImage_TIFF.h"

namespace
{

constexpr toff_t InvalidTiffOffset = static_cast<toff_t>(-1);

DUT::MemFile &fileFromHandle( thandle_t handle )
{
    return *static_cast<DUT::MemFile *>(handle);
}

bool toSignedOffset( toff_t offset, ptrdiff_t &outOffset )
{
    const auto maxPositive = static_cast<toff_t>(std::numeric_limits<ptrdiff_t>::max());
    if ( offset <= maxPositive )
    {
        outOffset = static_cast<ptrdiff_t>(offset);
        return true;
    }

    const toff_t magnitude = ~offset + 1;
    if ( magnitude > maxPositive + 1 )
        return false;

    if ( magnitude == maxPositive + 1 )
        outOffset = std::numeric_limits<ptrdiff_t>::min();
    else
        outOffset = -static_cast<ptrdiff_t>(magnitude);

    return true;
}

toff_t seekProc( thandle_t handle, toff_t offset, int whence )
{
    try
    {
        auto &file = fileFromHandle( handle );

        if ( whence == SEEK_SET )
        {
            if ( offset > std::numeric_limits<size_t>::max() )
                return InvalidTiffOffset;

            file.SeekSet( static_cast<size_t>(offset) );
        }
        else
        {
            ptrdiff_t signedOffset = 0;
            if ( !toSignedOffset( offset, signedOffset ) )
                return InvalidTiffOffset;

            if ( whence == SEEK_CUR )
                file.SeekFromCur( signedOffset );
            else if ( whence == SEEK_END )
                file.SeekFromEnd( signedOffset );
            else
                return InvalidTiffOffset;
        }

        return static_cast<toff_t>(file.GetCurPos());
    }
    catch ( ... )
    {
        return InvalidTiffOffset;
    }
}

tmsize_t readProc( thandle_t handle, void *pData, tmsize_t size )
{
    if ( size < 0 )
        return -1;

    try
    {
        auto &file = fileFromHandle( handle );
        const size_t remaining = file.GetDataSize() - file.GetCurPos();
        const size_t requested = static_cast<size_t>(size);
        const size_t readSize = std::min( requested, remaining );

        if ( readSize > 0 )
            file.ReadData( pData, readSize );

        return static_cast<tmsize_t>(readSize);
    }
    catch ( ... )
    {
        return -1;
    }
}

tmsize_t writeProc( thandle_t, void *, tmsize_t )
{
    return 0;
}

toff_t sizeProc( thandle_t handle )
{
    return static_cast<toff_t>(fileFromHandle( handle ).GetDataSize());
}

int closeProc( thandle_t )
{
    return 0;
}

int mapProc( thandle_t, void **, toff_t * )
{
    return 0;
}

void unmapProc( thandle_t, void *, toff_t )
{
}

using TiffHandle = std::unique_ptr<TIFF, decltype(&TIFFClose)>;

} // namespace

namespace DIMG
{

//==================================================================
void LoadTIFF( Image &img, DUT::MemFile &readFile, const char *pFName )
{
    TiffHandle tiff(
        TIFFClientOpen(
            pFName,
            "rm",
            &readFile,
            readProc,
            writeProc,
            seekProc,
            closeProc,
            sizeProc,
            mapProc,
            unmapProc ),
        &TIFFClose );

    DASSTHROW( tiff != nullptr, "Could not open %s for read", pFName );

    uint32_t width = 0;
    uint32_t height = 0;
    uint16_t samplesPerPixel = 0;

    DASSTHROW(
        TIFFGetField( tiff.get(), TIFFTAG_IMAGEWIDTH, &width ) == 1 &&
        TIFFGetField( tiff.get(), TIFFTAG_IMAGELENGTH, &height ) == 1,
        "Missing image dimensions in %s",
        pFName );
    TIFFGetFieldDefaulted( tiff.get(), TIFFTAG_SAMPLESPERPIXEL, &samplesPerPixel );

    DASSTHROW(
        width > 0 && width <= Image::MAX_DIM_SIZE &&
        height > 0 && height <= Image::MAX_DIM_SIZE,
        "Unsupported image dimensions (%u, %u) in %s",
        width,
        height,
        pFName );
    DASSTHROW(
        static_cast<size_t>(width) <=
            std::numeric_limits<size_t>::max() / static_cast<size_t>(height),
        "Image dimensions overflow in %s",
        pFName );

    const size_t pixelCount = static_cast<size_t>(width) * height;
    std::vector<uint32_t> raster( pixelCount );

    DASSTHROW(
        TIFFReadRGBAImageOriented(
            tiff.get(),
            width,
            height,
            raster.data(),
            ORIENTATION_TOPLEFT,
            0 ) == 1,
        "Could not read %s",
        pFName );

    const char *pChannelNames = nullptr;
    switch ( samplesPerPixel )
    {
    case 1:
        pChannelNames = "y";
        break;

    case 3:
        pChannelNames = "rgb";
        break;

    case 4:
        pChannelNames = "rgba";
        break;

    default:
        DASSTHROW(
            0,
            "Unsupported # of samples per pixel (%u) in image %s",
            samplesPerPixel,
            pFName );
        return;
    }

    img.Init( width, height, samplesPerPixel, Image::ST_U8, -1, pChannelNames );

    size_t sourceIndex = 0;
    for (U32 y = 0; y < height; ++y)
    {
        U8 *pDest = img.GetPixelPtrRW( 0, y );

        for (U32 x = 0; x < width; ++x, ++sourceIndex)
        {
            const uint32_t pixel = raster[sourceIndex];
            pDest[0] = static_cast<U8>(TIFFGetR( pixel ));

            if ( samplesPerPixel >= 3 )
            {
                pDest[1] = static_cast<U8>(TIFFGetG( pixel ));
                pDest[2] = static_cast<U8>(TIFFGetB( pixel ));
            }

            if ( samplesPerPixel == 4 )
                pDest[3] = static_cast<U8>(TIFFGetA( pixel ));

            pDest += samplesPerPixel;
        }
    }
}

//==================================================================
void SaveTIFF( const Image &img, const char *pFName )
{
    const bool supportedSampleType =
        img.mSampType == Image::ST_U8 ||
        img.mSampType == Image::ST_U16 ||
        img.mSampType == Image::ST_U32 ||
        img.mSampType == Image::ST_F32;
    DASSTHROW( supportedSampleType, "Unsupported pixel format when writing %s", pFName );

    const bool isGray = img.mSampPerPix == 1;
    const bool isRGB = img.mSampPerPix == 3 && img.IsSamplesNames( "rgb" );
    const bool isRGBA = img.mSampPerPix == 4 && img.IsSamplesNames( "rgba" );
    DASSTHROW(
        isGray || isRGB || isRGBA,
        "Cannot write %s. Unsupported image format.",
        pFName );

    TiffHandle tiff( TIFFOpen( pFName, "w" ), &TIFFClose );
    DASSTHROW( tiff != nullptr, "Could not open %s for write", pFName );

    DASSTHROW(
        TIFFSetField( tiff.get(), TIFFTAG_IMAGEWIDTH, img.mWd ) == 1 &&
        TIFFSetField( tiff.get(), TIFFTAG_IMAGELENGTH, img.mHe ) == 1 &&
        TIFFSetField( tiff.get(), TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG ) == 1 &&
        TIFFSetField( tiff.get(), TIFFTAG_COMPRESSION, COMPRESSION_NONE ) == 1 &&
        TIFFSetField( tiff.get(), TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT ) == 1 &&
        TIFFSetField(
            tiff.get(),
            TIFFTAG_BITSPERSAMPLE,
            static_cast<uint16_t>(img.mBytesPerSamp * 8) ) == 1 &&
        TIFFSetField(
            tiff.get(),
            TIFFTAG_SAMPLEFORMAT,
            img.mSampType == Image::ST_F32 ? SAMPLEFORMAT_IEEEFP : SAMPLEFORMAT_UINT ) == 1 &&
        TIFFSetField(
            tiff.get(),
            TIFFTAG_SAMPLESPERPIXEL,
            static_cast<uint16_t>(img.mSampPerPix) ) == 1,
        "Could not configure TIFF output %s",
        pFName );

    if ( isGray )
    {
        DASSTHROW(
            TIFFSetField( tiff.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK ) == 1,
            "Could not configure TIFF output %s",
            pFName );
    }
    else
    {
        DASSTHROW(
            TIFFSetField( tiff.get(), TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB ) == 1,
            "Could not configure TIFF output %s",
            pFName );
    }

    uint16_t alphaType = EXTRASAMPLE_ASSOCALPHA;
    if ( isRGBA )
    {
        DASSTHROW(
            TIFFSetField( tiff.get(), TIFFTAG_EXTRASAMPLES, 1, &alphaType ) == 1,
            "Could not configure TIFF alpha output %s",
            pFName );
    }

    for (U32 y = 0; y < img.mHe; ++y)
    {
        auto *pRow = const_cast<U8 *>(img.GetPixelPtrR( 0, y ));
        DASSTHROW(
            TIFFWriteScanline( tiff.get(), pRow, y, 0 ) >= 0,
            "Could not write row %u to %s",
            y,
            pFName );
    }
}

//==================================================================
}
