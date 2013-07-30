//==================================================================
/// DImage_TIFF.cpp
///
/// Created by Davide Pasca - 2010/1/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

extern "C"
{
	#include "externals/local/libtiff/tif_config.h"
	#include "externals/local/libtiff/tiffconf.h"
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
							"rm",
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
		DASSTHROW( 0, ("Could not open %s for read", pFName) );
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

	// XXX: raster array has 4-byte unsigned integer type, that is why
	// we should rearrange it here.
#if HOST_BIGENDIAN
    TIFFSwabArrayOfLong( pTmpBuff, w * h );
#endif

	TIFFClose( pTiff );

	// the following is for debugging, to dump out the just read TIF
#if 0
	char	fixname[2048];
	char	ppmName[2048];

	for (size_t i=0; ; ++i)
	{
		fixname[i] = pFName[i];

		if ( fixname[i] == '/' || fixname[i] == '\\' )
			fixname[i] = '_';

		if NOT( pFName[i] )
			break;
	}

	sprintf_s( ppmName, "C:\\Users\\xxxxxx\\tmp\\%s.ppm", fixname );

	SavePPM( img, ppmName );
#endif

}

//==================================================================
void SaveTIFF( const Image &img, const char *pFName )
{
	TIFF *pTiff = TIFFOpen( pFName, "w" );

	if NOT( pTiff )
	{
		DASSTHROW( 0, ("Could not open %s for write", pFName) );
	}

	// Write the tiff tags to the file
	TIFFSetField( pTiff, TIFFTAG_IMAGEWIDTH, img.mWd );
	TIFFSetField( pTiff, TIFFTAG_IMAGELENGTH, img.mHe );
	TIFFSetField( pTiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG );
	//TIFFSetField( pTiff, TIFFTAG_COMPRESSION, COMPRESSION_LZW );
    TIFFSetField( pTiff, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT );

	DASSTHROW( img.mSampType != Image::ST_F16, ("Unsupported pixel format when writing %s", pFName) );

	if ( img.mSampType == Image::ST_F32 )
		TIFFSetField( pTiff, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP );

	TIFFSetField( pTiff, TIFFTAG_BITSPERSAMPLE, img.mBytesPerSamp * 8 );

	if ( img.mSampPerPix == 1 )
	{
		TIFFSetField( pTiff, TIFFTAG_SAMPLESPERPIXEL, 1 );
	}
	else
	if ( img.mSampPerPix == 3 && img.IsSamplesNames( "rgb" ) )
	{
		TIFFSetField( pTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
		TIFFSetField( pTiff, TIFFTAG_SAMPLESPERPIXEL, 3 );
	}
	else
	if ( img.mSampPerPix == 4 && img.IsSamplesNames( "rgba" ) )
	{
		TIFFSetField( pTiff, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB );
		TIFFSetField( pTiff, TIFFTAG_SAMPLESPERPIXEL, 4 );
		TIFFSetField( pTiff, TIFFTAG_MATTEING, 1 );
	}
	else
	{
		DASSTHROW( 0, ("Cannot write %s. Unsupported image format.", pFName) );
	}

	//TIFFWriteEncodedStrip( pTiff, 0, (tdata_t)img.GetPixelPtrR(0,0) );

	for (U32 i=0; i < img.mHe; ++i)
	{
		TIFFWriteScanline( pTiff, (tdata_t)img.GetPixelPtrR(0,i), i );
	}

	TIFFClose( pTiff );
}

//==================================================================
}
