//==================================================================
/// DImage.cpp
///
/// Created by Davide Pasca - 2010/1/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DImage.h"
#include "DSystem/include/DMemory.h"
#include "DSystem/include/DUtils.h"

//==================================================================
namespace DIMG
{

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

	size_t	len = strlen( pSampNames );

	DASSTHROW(
		wd > 0 && wd <= MAX_DIM_SIZE &&
		he > 0 && he <= MAX_DIM_SIZE,
			("Unholy image dimensions ! (%ui, %ui)", wd, he) );

	DASSTHROW(
		len == (size_t)mSampPerPix &&
		mSampPerPix <= MAX_DIM_SIZE,
			("Bad samples definition !") );

	for (size_t i=0; i < len; ++i)
		mSampNames[i] = pSampNames[i];

	mBytesPerSamp = getSampTypeSize( mSampType );

	mBytesPerPix	= mBytesPerSamp * mSampPerPix;

	if ( bytesPerRow <= 0 )
	{
		mBytesPerRow = mBytesPerPix * mWd;
		mBytesPerRow = (mBytesPerRow + 63) & ~63;
	}
	else
	{
		mBytesPerRow = (U32)bytesPerRow;
	}

	if ( pSrcDataBorrow )
	{
		mOwnData = false;
		mpData = (U8 *)pSrcDataBorrow;
	}
	else
	{
		mOwnData = true;
		mpData = DNEW U8 [ mBytesPerRow * mHe ];
	}
}

//==================================================================
bool Image::IsSamplesNames( const char *pMatchSampNames ) const
{
	size_t	len = strlen( pMatchSampNames );
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

	switch ( mSampType )
	{
	case Image::ST_U8	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = (1.0f/255) * ((const U8 *)pPixData)[i]; break;
	case Image::ST_U16	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = (1.0f/65535) * ((const U16 *)pPixData)[i]; break;
	case Image::ST_U32	:	for (size_t i=0; i < mSampPerPix; ++i) out_samps[i] = (1.0f/0xffffffff) * ((const U32 *)pPixData)[i]; break;
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

	switch ( mSampType )
	{
	case Image::ST_U8	:	for (size_t i=0; i < mSampPerPix; ++i) ((U8 *)pPixData)[i] = (U8)D::Clamp( 255 * in_samps[i], 0.f, 255.f ); break;
	case Image::ST_U16	:	for (size_t i=0; i < mSampPerPix; ++i) ((U16 *)pPixData)[i] = (U16)D::Clamp( 65535 * in_samps[i], 0.f, 65535.f ); break;
	case Image::ST_U32	:	for (size_t i=0; i < mSampPerPix; ++i) ((U32 *)pPixData)[i] = (U32)D::Clamp( 0xffffffff * in_samps[i], 0.f, (float)0xffffffff ); break;
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
