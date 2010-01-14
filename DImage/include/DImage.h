//==================================================================
/// DImage.h
///
/// Created by Davide Pasca - 2010/1/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DIMAGE_H
#define DIMAGE_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DUtils_Base.h"

//==================================================================
namespace DIMG
{

//==================================================================
class Image
{
public:
	static const U32 MAX_SAMP_PER_PIX	= 4;
	static const U32 MAX_DIM_SIZE		= 1 << 16;

	enum SampType
	{
		ST_UNKNOWN,
		ST_U8,
		ST_U16,
		ST_U32,
		ST_F16,
		ST_F32,
	};

	U8		*mpData;
	U32		mBytesPerRow;
	U32		mWd;
	U32		mHe;
	U32		mSampType;
	U32		mSampPerPix;
	char	mSampNames[MAX_SAMP_PER_PIX];

	// calculated for convenience
	U32		mBytesPerSamp;
	U32		mBytesPerPix;

private:
	void initBase();

public:
	Image()
	{
		initBase();
	}

	~Image();

	void Init(
			U32			wd,
			U32			he,
			U32			sampPerPix,
			SampType	sampType,
			int			bytesPerRow,
			const char	*pSampNames );

	U8 *GetPixelPtrRW( U32 x, U32 y )
	{
		DASSERT( x < mWd && y < mHe );
		return mpData + mBytesPerRow * y + x * mBytesPerPix;
	}
};


//==================================================================
}

#endif
