//==================================================================
/// DImage_TIFF.cpp
///
/// Created by Davide Pasca - 2010/1/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

extern "C"
{
	#include "externals/local/libtiff_pc/tif_config.h"
	#include "externals/local/libtiff_pc/tiffconf.h"
	#include "tiff.h"
	#include "tiffio.h"
	#include "tiffiop.h"
}

#include "DImage_TIFF.h"

//==================================================================
static toff_t seekProc( thandle_t pFile, toff_t offset, int whence )
{
	try 
	{
		if ( whence == SEEK_SET )	((DUT::MemFile *)pFile)->SeekSet( offset );	else
		if ( whence == SEEK_CUR )	((DUT::MemFile *)pFile)->SeekFromCur( offset );	else
		if ( whence == SEEK_END )	((DUT::MemFile *)pFile)->SeekFromEnd( offset );
		else
			return -1;

	} catch ( ... )
	{
		return -1;
	}

	return (toff_t)((DUT::MemFile *)pFile)->GetCurPos();
}   

//==================================================================
static tsize_t readProc( thandle_t pFile, tdata_t pData, tsize_t size )
{
	try 
	{
		((DUT::MemFile *)pFile)->ReadData( pData, size );
	} catch ( ... )
	{
		return (tsize_t)-1;	// $$$ ummm ??!!
	}

	return size;
}

//==================================================================
static tsize_t writeProc( thandle_t pFile, tdata_t pData, tsize_t size )
{
	DASSERT( 0 );
	return 0;
}

//==================================================================
static toff_t sizeProc( thandle_t pFile )
{
	return (toff_t)((DUT::MemFile *)pFile)->GetDataSize();
}

//==================================================================
static int closeProc( thandle_t pFile )
{
	return 0;
}

//==================================================================
static int mapProc( thandle_t pFile, tdata_t *ppData, toff_t *pOff )
{
	DASSERT( 0 );
	return 0;
}

//==================================================================
static void unmapProc( thandle_t pFile, tdata_t pData, toff_t off )
{
	DASSERT( 0 );
}

//==================================================================
namespace DIMG
{

//==================================================================
void LoadTIFF( Image &img, DUT::MemFile &readFile, const char *pFName )
{
	TIFF *pTiff = TIFFClientOpen(
							pFName,
							"rbm",
							&readFile,
							readProc,
							writeProc,
							seekProc,
							closeProc,
							sizeProc,
							mapProc,
							unmapProc );

	if NOT( pTiff )
	{
		DASSTHROW( 0, ("Could not open %s", pFName) );
	}

	U32	w, h;
	TIFFGetField( pTiff, TIFFTAG_IMAGEWIDTH, &w );
	TIFFGetField( pTiff, TIFFTAG_IMAGELENGTH, &h );

	U16	photometric;
	TIFFGetField( pTiff, TIFFTAG_PHOTOMETRIC, &photometric );

	U16	bps, spp;
	TIFFGetField( pTiff, TIFFTAG_BITSPERSAMPLE, &bps );
	TIFFGetField( pTiff, TIFFTAG_SAMPLESPERPIXEL, &spp );

	Image::SampType	imgSampType = Image::ST_UNKNOWN;

	U32 *pTmpBuff = (U32 *)_TIFFCheckMalloc( pTiff, w * h, sizeof(U32), "raster buffer" );

	if NOT( pTmpBuff )
	{
		TIFFClose( pTiff );
		throw std::bad_alloc();
	}

	// Read the image in one chunk into an RGBA array
	if NOT( TIFFReadRGBAImageOriented( pTiff, w, h, pTmpBuff, ORIENTATION_TOPLEFT, 0 ) )
	{
		TIFFClose( pTiff );
		_TIFFfree( pTmpBuff );
		DASSTHROW( 0, ("Could not open %s", pFName) );
	}

	const char *pChansStr = 0;
	switch ( spp )
	{
	case 1:	pChansStr = "y"; break;
	case 3:	pChansStr = "rgb"; break;
	case 4:	pChansStr = "rgba"; break;
	default:
		DASSTHROW( 0, ("Unsupported # of samples per pixels (%i) in image %s", spp, pFName) );
		return;
	}

	img.Init( w, h, spp, Image::ST_U8, -1, pChansStr );

	size_t	srcIdx = 0;
	for (U32 y=0; y < h; ++y)
	{
		if ( spp == 1 )
		{
			U8 *pDest = img.GetPixelPtrRW( 0, y );

			switch ( spp )
			{
			case 1:
				for (U32 x=0; x < w; ++x, ++srcIdx)
				{
					pDest[0] = TIFFGetR( pTmpBuff[srcIdx] );
					pDest += 1;
				}
				break;

			case 3:
				for (U32 x=0; x < w; ++x, ++srcIdx)
				{
					pDest[0] = TIFFGetR( pTmpBuff[srcIdx] );
					pDest[1] = TIFFGetG( pTmpBuff[srcIdx] );
					pDest[2] = TIFFGetB( pTmpBuff[srcIdx] );
					pDest += 3;
				}
				break;

			case 4:
				for (U32 x=0; x < w; ++x, ++srcIdx)
				{
					pDest[0] = TIFFGetR( pTmpBuff[srcIdx] );
					pDest[1] = TIFFGetG( pTmpBuff[srcIdx] );
					pDest[2] = TIFFGetB( pTmpBuff[srcIdx] );
					pDest[3] = TIFFGetA( pTmpBuff[srcIdx] );
					pDest += 4;
				}
				break;
			}
		}
	}

	// XXX: raster array has 4-byte unsigned integer type, that is why
	// we should rearrange it here.
#if HOST_BIGENDIAN
    TIFFSwabArrayOfLong( pTmpBuff, w * h );
#endif

	TIFFClose( pTiff );
}

//==================================================================
}
