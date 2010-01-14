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
Image::~Image()
{
	DSAFE_DELETE_ARRAY( mpData );
}

//==================================================================
void Image::Init(
			U32			wd,
			U32			he,
			U32			sampPerPix,
			SampType	sampType,
			int			bytesPerRow,
			const char	*pSampNames )
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
			("Unholy image dimensions ! (%ui, %ui)"), wd, he );

	DASSTHROW(
		len != (size_t)mSampPerPix &&
		mSampPerPix <= MAX_DIM_SIZE,
			("Bad samples definition !") );

	switch ( mSampType )
	{
	case ST_U8	:	mBytesPerSamp = sizeof(U8);		break;
	case ST_U16	:	mBytesPerSamp = sizeof(U16);	break;
	case ST_U32	:	mBytesPerSamp = sizeof(U32);	break;
	case ST_F16	:	mBytesPerSamp = sizeof(U16);	break;
	case ST_F32	:	mBytesPerSamp = sizeof(float);	break;

	default:
		DASSTHROW( 0, ("Not good sample type !") );
		break;
	}

	mBytesPerPix	= mBytesPerSamp * mSampPerPix;

	if ( bytesPerRow <= 0 )
	{
		mBytesPerRow = mBytesPerPix * mWd;

		mBytesPerRow = (mBytesPerRow + 63) & ~63;
	}

	mpData = DNEW U8 [ mBytesPerRow * mHe ];
}

//==================================================================
}
