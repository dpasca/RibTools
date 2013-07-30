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

	bool		mOwnData;
	U8			*mpData;
	U32			mBytesPerRow;
	U32			mWd;
	U32			mHe;
	SampType	mSampType;
	U32			mSampPerPix;
	char		mSampNames[MAX_SAMP_PER_PIX];

	// calculated for convenience
	U32			mBytesPerSamp;
	U32			mBytesPerPix;

private:
	void initBase();

public:
	Image()
	{
		initBase();
	}

	Image(
		U32			wd,
		U32			he,
		U32			sampPerPix,
		SampType	sampType,
		int			bytesPerRow,
		const char	*pSampNames,
		const U8	*pSrcDataBorrow=NULL );

	~Image();

	void Init(
			U32			wd,
			U32			he,
			U32			sampPerPix,
			SampType	sampType,
			int			bytesPerRow,
			const char	*pSampNames,
			const U8	*pSrcDataBorrow=NULL );

	bool IsSamplesNames( const char *pMatchSampNames ) const;

	bool IsSamplesType( SampType type ) const	{	return type == mSampType;	}

	void GetPixelAsFloat( U32 x, U32 y, float out_samps[MAX_SAMP_PER_PIX] ) const;

	void SetPixelFromFloat( U32 x, U32 y, const float in_samps[MAX_SAMP_PER_PIX] );

	U8 *GetPixelPtrRW( U32 x, U32 y )
	{
		DASSERT( x < mWd && y < mHe );
		return mpData + mBytesPerRow * y + x * mBytesPerPix;
	}

	const U8 *GetPixelPtrR( U32 x, U32 y ) const
	{
		DASSERT( x < mWd && y < mHe );
		return mpData + mBytesPerRow * y + x * mBytesPerPix;
	}

	void GetPixelQuadWrapR( int x, int y, U8 *pDest4[4] ) const
	{
		DASSERT( mWd >= 1 && mHe >= 1 );

		int	x1 = x % mWd; x1 += mWd & (x1 >> (sizeof(int)*8-1));
		int	y1 = y % mHe; x1 += mHe & (y1 >> (sizeof(int)*8-1));

		int x2 = x1 + 1; if ( x2 >= (int)mWd ) x2 -= mWd;
		int y2 = y1 + 1; if ( y2 >= (int)mHe ) y2 -= mHe;

		pDest4[0] = mpData + mBytesPerRow * y1 + x1 * mBytesPerPix;
		pDest4[1] = mpData + mBytesPerRow * y1 + x2 * mBytesPerPix;
		pDest4[2] = mpData + mBytesPerRow * y2 + x1 * mBytesPerPix;
		pDest4[3] = mpData + mBytesPerRow * y2 + x2 * mBytesPerPix;
	}

	void GetPixelQuadClampR( int x, int y, U8 *pDest4[4] ) const
	{
		DASSERT( mWd >= 1 && mHe >= 1 );
		int	x1 = D::Clamp( x+0, 0, (int)mWd-1 );
		int	y1 = D::Clamp( y+0, 0, (int)mHe-1 );
		int	x2 = D::Clamp( x+1, 0, (int)mWd-1 );
		int	y2 = D::Clamp( y+1, 0, (int)mHe-1 );

		pDest4[0] = mpData + mBytesPerRow * y1 + x1 * mBytesPerPix;
		pDest4[1] = mpData + mBytesPerRow * y1 + x2 * mBytesPerPix;
		pDest4[2] = mpData + mBytesPerRow * y2 + x1 * mBytesPerPix;
		pDest4[3] = mpData + mBytesPerRow * y2 + x2 * mBytesPerPix;
	}
};

//==================================================================
void ConvertImages( Image &des, const Image &src, U32 dx=0, U32 dy=0, U32 sx=0, U32 sy=0, int wd=-1, int he=-1 );

//==================================================================
}

#endif
