//==================================================================
/// DImage.cpp
///
/// Created by Davide Pasca - 2010/1/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <cmath>
#include <limits>
#include <string.h>
#include "DSystem/include/DMemory.h"
#include "DSystem/include/DUtils.h"
#include "DImage.h"

//==================================================================
namespace DIMG
{

//==================================================================
static double clampUnitSample( float sample )
{
    // Integer images cannot represent NaN. Use black/transparent for NaN and
    // saturate infinities just like other out-of-range floating-point values.
    if ( std::isnan( sample ) )
        return 0.0;

    return D::Clamp( static_cast<double>(sample), 0.0, 1.0 );
}

//==================================================================
void Image::initBase()
{
    mOwnData		= false;
    mpData			= NULL;
    mBytesPerRow	= 0;
    mWd				= 0;
    mHe				= 0;
    mSampType		= ST_UNKNOWN;
    mSampPerPix		= 0;

    for (U32 i=0; i < MAX_SAMP_PER_PIX; ++i)
        mSampNames[i] = 0;

    mBytesPerSamp	= 0;
    mBytesPerPix	= 0;
}

//==================================================================
Image::Image(
        U32			wd,
        U32			he,
        U32			sampPerPix,
        SampType	sampType,
        int			bytesPerRow,
        const char	*pSampNames,
        const U8	*pSrcDataBorrow )
{
    initBase();

    Init(
        wd,
        he,
        sampPerPix,
        sampType,
        bytesPerRow,
        pSampNames,
        pSrcDataBorrow );
}

//==================================================================
Image::~Image()
{
    if ( mOwnData )
    {
        DSAFE_DELETE_ARRAY( mpData );
    }
}

//==================================================================
static U32 getSampTypeSize( Image::SampType sampType )
{
    switch ( sampType )
    {
    case Image::ST_U8	:	return sizeof(U8);
    case Image::ST_U16	:	return sizeof(U16);
    case Image::ST_U32	:	return sizeof(U32);
    case Image::ST_F16	:	return sizeof(U16);
    case Image::ST_F32	:	return sizeof(float);

    default:
        DASSTHROW( 0, ("Not good sample type !") );
        break;
    }

    return 0;
}

//==================================================================
void Image::Init(
            U32			wd,
            U32			he,
            U32			sampPerPix,
            SampType	sampType,
            int			bytesPerRow,
            const char	*pSampNames,
            const U8	*pSrcDataBorrow )
{
    DASSERT( mSampType == ST_UNKNOWN );

    mWd			= wd;
    mHe			= he;
    mSampPerPix	= sampPerPix;
    mSampType	= sampType;

    DASSTHROW( pSampNames != nullptr, "Missing samples definition" );
    const size_t len = strlen( pSampNames );

    DASSTHROW(
        wd > 0 && wd <= MAX_DIM_SIZE &&
        he > 0 && he <= MAX_DIM_SIZE,
            "Unholy image dimensions ! (%ui, %ui)", wd, he );

    DASSTHROW(
        mSampPerPix > 0 &&
        len == (size_t)mSampPerPix &&
        mSampPerPix <= MAX_SAMP_PER_PIX,
            ("Bad samples definition !") );

    for (size_t i=0; i < len; ++i)
        mSampNames[i] = pSampNames[i];

    mBytesPerSamp = getSampTypeSize( mSampType );

    mBytesPerPix	= mBytesPerSamp * mSampPerPix;

    if ( bytesPerRow <= 0 )
    {
        const size_t tightBytesPerRow = static_cast<size_t>(mBytesPerPix) * mWd;
        const size_t alignedBytesPerRow = (tightBytesPerRow + 63) & ~size_t(63);
        DASSTHROW(
            alignedBytesPerRow <= std::numeric_limits<U32>::max(),
            "Image row size is too large" );
        mBytesPerRow = static_cast<U32>(alignedBytesPerRow);
    }
    else
    {
        DASSTHROW(
            static_cast<size_t>(bytesPerRow) >= static_cast<size_t>(mBytesPerPix) * mWd,
            "Image row stride is too small" );
        mBytesPerRow = (U32)bytesPerRow;
    }

    DASSTHROW(
        static_cast<size_t>(mHe) <= std::numeric_limits<size_t>::max() / mBytesPerRow,
        "Image allocation size is too large" );

    if ( pSrcDataBorrow )
    {
        mOwnData = false;
        mpData = (U8 *)pSrcDataBorrow;
    }
    else
    {
        mOwnData = true;
        mpData = DNEW U8 [ static_cast<size_t>(mBytesPerRow) * mHe ];
    }
}

//==================================================================
bool Image::IsSamplesNames( const char *pMatchSampNames ) const
{
    if ( pMatchSampNames == nullptr )
        return false;

    const size_t len = strlen( pMatchSampNames );
    if ( len != mSampPerPix || len > MAX_SAMP_PER_PIX )
        return false;

    for (size_t i=0; i < len; ++i)
    {
        if ( mSampNames[i] != pMatchSampNames[i] )
            return false;
    }

    return true;
}

//==================================================================
void Image::GetPixelAsFloat( U32 x, U32 y, float out_samps[MAX_SAMP_PER_PIX] ) const
{
    const void *pPixData = GetPixelPtrR( x, y );
    const double maxU32 = static_cast<double>(std::numeric_limits<U32>::max());

    switch ( mSampType )
    {
    case Image::ST_U8	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = (1.0f/255) * ((const U8 *)pPixData)[i]; break;
    case Image::ST_U16	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = (1.0f/65535) * ((const U16 *)pPixData)[i]; break;
    case Image::ST_U32:
    {
        const auto *pSamples = static_cast<const U32 *>(pPixData);
        for (size_t i = 0; i < mSampPerPix; ++i)
        {
            out_samps[i] = static_cast<float>(pSamples[i] / maxU32);
        }
        break;
    }
        //case Image::ST_F16	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = ((const U16 *)pPixData)[i]; break;
    case Image::ST_F32	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = ((const float *)pPixData)[i]; break;

    default:
        DASSERT( 0 );
        return;
    }
}

//==================================================================
void Image::SetPixelFromFloat( U32 x, U32 y, const float in_samps[MAX_SAMP_PER_PIX] )
{
    void *pPixData = GetPixelPtrRW( x, y );
    const double maxU32 = static_cast<double>(std::numeric_limits<U32>::max());

    switch ( mSampType )
    {
    case Image::ST_U8:
    {
        auto *pSamples = static_cast<U8 *>(pPixData);
        for (size_t i = 0; i < mSampPerPix; ++i)
            pSamples[i] = static_cast<U8>(clampUnitSample( in_samps[i] ) * 255.0);
        break;
    }
    case Image::ST_U16:
    {
        auto *pSamples = static_cast<U16 *>(pPixData);
        for (size_t i = 0; i < mSampPerPix; ++i)
            pSamples[i] = static_cast<U16>(clampUnitSample( in_samps[i] ) * 65535.0);
        break;
    }
    case Image::ST_U32:
    {
        auto *pSamples = static_cast<U32 *>(pPixData);
        for (size_t i = 0; i < mSampPerPix; ++i)
        {
            pSamples[i] = static_cast<U32>(clampUnitSample( in_samps[i] ) * maxU32);
        }
        break;
    }
        //case Image::ST_F16	:	for (size_t i=0; i < mSampPerPix; ++i) ((U16 *)pPixData)[i] = in_samps[i]; break;
    case Image::ST_F32	:	for (size_t i=0; i < mSampPerPix; ++i) ((float *)pPixData)[i] = in_samps[i]; break;

    default:
        DASSERT( 0 );
        return;
    }
}

//==================================================================
void ConvertImages( Image &des, const Image &src, U32 dx, U32 dy, U32 sx, U32 sy, int wd, int he )
{
    if ( wd < 0 )
        wd = (int)D::Min( des.mWd, src.mWd );

    if ( he < 0 )
        he = (int)D::Min( des.mHe, src.mHe );

    float	tmp[ Image::MAX_SAMP_PER_PIX ];

    for (int y=0; y < he; ++y)
    {
        for (int x=0; x < wd; ++x)
        {
            memset( tmp, 0, sizeof(tmp) );
            src.GetPixelAsFloat( x, y, tmp );
            des.SetPixelFromFloat( x, y, tmp );
        }
    }
}

//==================================================================
}
