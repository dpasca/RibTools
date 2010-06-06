//==================================================================
/// DImage_BMP.cpp
///
/// Created by Davide Pasca - 2010/1/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DImage_BMP.h"

//==================================================================
#pragma pack(push,1)

typedef unsigned int    U32;
typedef unsigned short  U16;
typedef unsigned int    U32;

typedef struct  { // bmfh
	U16   bfType;
	U32   bfSize;
	U16   bfReserved1;
	U16   bfReserved2;
	U32   bfOffBits;
} myBITMAPFILEHEADER;

typedef struct {
	U32		biSize;
	U32		biWidth;
	U32		biHeight;
	U16		biPlanes;
	U16		biBitCount;
	U32		biCompression;
	U32		biSizeImage;
	U32		biXPelsPerMeter;
	U32		biYPelsPerMeter;
	U32		biClrUsed;
	U32		biClrImportant;
} myBITMAPINFOHEADER;

typedef struct  { // rgbq
	U8  rgbBlue;
	U8  rgbGreen;
	U8  rgbRed;
	U8  rgbReserved;
} myRGBQUAD;

#pragma pack(pop)

#define myBI_RGB        0L
#define myBI_RLE8       1L
#define myBI_RLE4       2L
#define myBI_BITFIELDS  3L
#define myBI_JPEG       4L
#define myBI_PNG        5L

//==================================================================
namespace DIMG
{

//==================================================================
static bool isPaletteGrayscale( const myRGBQUAD *pPal, size_t cnt )
{
	for (size_t i=0; i < cnt; ++i)
	{
		if (pPal->rgbRed != pPal->rgbGreen ||
			pPal->rgbRed != pPal->rgbBlue )
			return false;
	}

	return true;
}

//==================================================================
void LoadBMP( Image &img, DUT::MemFile &readFile, const char *pFName )
{
	const myBITMAPFILEHEADER *pFileHead =
			(const myBITMAPFILEHEADER *)readFile.ReadDataPtr( sizeof(myBITMAPFILEHEADER) );

	DASSTHROW(
		pFileHead->bfSize != sizeof(myBITMAPFILEHEADER),
			("Unknown BMP format") );

	const myBITMAPINFOHEADER *pInfoHead =
			(const myBITMAPINFOHEADER *)readFile.ReadDataPtr( sizeof(myBITMAPINFOHEADER) );

	// see how much more there is of this structure to read
	ptrdiff_t	sizeDiff = pInfoHead->biSize - sizeof(myBITMAPINFOHEADER);
	// did we expect a larger structure ? Then it's a problem
	DASSTHROW( sizeDiff < 0, ("Unknown BMP format") );

	// we only read uncompressed stuff
	DASSTHROW( pInfoHead->biCompression != myBI_RGB, ("Unsupported BMP format") );

	// skip to the end of myBITMAPINFOHEADER structure
	readFile.SeekFromCur( sizeDiff );
	
	size_t			clutSize = 0;
	const myRGBQUAD	*pClutData = NULL;

	Image::SampType	sampType = Image::ST_UNKNOWN;
	U32				sampPerPix = 0;
	char			*pSampNames = NULL;

	bool			convertingToTrueCol = false;

	switch ( pInfoHead->biBitCount )
	{
	case 8:
		if ( pInfoHead->biClrUsed )
			clutSize = pInfoHead->biClrUsed;
		else
			clutSize = (size_t)(1 << pInfoHead->biBitCount);	// 1 << 8 actually..

		pClutData = (const myRGBQUAD *)readFile.ReadDataPtr( clutSize * sizeof(myRGBQUAD) );

		sampType	= Image::ST_U8;

		if ( isPaletteGrayscale( pClutData, clutSize ) )
		{
			sampPerPix	= 1;
			pSampNames	= "a";
		}
		else
		{
			sampPerPix	= 3;
			pSampNames	= "rgb";
			convertingToTrueCol = true;
		}
		break;

	case 24:
		sampType	= Image::ST_U8;
		sampPerPix	= 3;
		pSampNames	= "rgb";
		break;

	case 32:
		sampType	= Image::ST_U8;
		sampPerPix	= 4;
		pSampNames	= "rgba";
		break;

	default:
		DASSTHROW( 0, ("Cannot read BMP files with %i bits per pixel", pInfoHead->biBitCount) );
		return;
	}

	img.Init(
			pInfoHead->biWidth,
			pInfoHead->biHeight,
			sampPerPix,
			sampType,
			-1,
			pSampNames );

	int	srcPitch = ((pInfoHead->biWidth * pInfoHead->biBitCount / 8) + 3) & ~3;

	for (int y=(int)pInfoHead->biHeight-1; y >= 0; --y)
	{
		const U8 *pSrcRow = (const U8 *)readFile.ReadDataPtr( srcPitch );
		
		U8	*pDestRow = img.GetPixelPtrRW( 0, y );

		if ( sampPerPix == 1 )
		{
			if ( convertingToTrueCol )
			{
				for (size_t i=0; i < (size_t)img.mWd; ++i)
				{
					// expand to true color and from bgrx to rgb
					const myRGBQUAD &col = pClutData[ pSrcRow[i] ];
					pDestRow[0] = col.rgbRed;
					pDestRow[1] = col.rgbGreen;
					pDestRow[2] = col.rgbBlue;
					pDestRow += img.mBytesPerPix;
				}
			}
			else
			{
				memcpy( pDestRow, pSrcRow, img.mWd * sizeof(U8)  );
			}
		}
		else
		if ( sampPerPix == 3 )
		{
			for (size_t i=0; i < (size_t)img.mWd; ++i)
			{
				pDestRow[0] = pSrcRow[2];
				pDestRow[1] = pSrcRow[1];
				pDestRow[2] = pSrcRow[0];
				pSrcRow += 3;
				pDestRow += img.mBytesPerPix;
			}
		}
		else
		if ( sampPerPix == 4 )
		{
			for (size_t i=0; i < (size_t)img.mWd; ++i)
			{
				pDestRow[0] = pSrcRow[2];
				pDestRow[1] = pSrcRow[1];
				pDestRow[2] = pSrcRow[0];
				pDestRow[3] = pSrcRow[3];
				pSrcRow += 4;
				pDestRow += img.mBytesPerPix;
			}
		}
	}
}

//==================================================================
}
