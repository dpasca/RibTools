//==================================================================
/// DispDriverFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

//#include "stdafx.h"

#include <stdio.h>

#define XMD_H	// shit !
extern "C" 
{
	#include "externals/libjpeg/jpeglib.h"
};
#undef XMD_H	// un-shit !

#include "DispDrivers/include/DispDriverFile.h"

//==================================================================
static void write_JPEG_file(
			const char * filename,
			int quality,
			const JSAMPLE * image_buffer,
			int image_width,
			int image_height
			)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* More stuff */
	FILE * outfile;		/* target file */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	if ( 0 != fopen_s( &outfile, filename, "wb" ) )
	{
		DASSTHROW( 0, ("can't open %s\n", filename) );
	}
	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	jpeg_start_compress(&cinfo, TRUE);
	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

	const JSAMPLE *row_pointer[1];	/* pointer to JSAMPLE row[s] */

	while (cinfo.next_scanline < cinfo.image_height)
	{
		row_pointer[0] = & image_buffer[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, (JSAMPARRAY)row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);

	jpeg_destroy_compress(&cinfo);
}

//==================================================================
// DispDriverFile
//==================================================================
DispDriverFile::DispDriverFile( const char *pFileName ) :
	mpFileName(pFileName),
	mpBuffer(NULL),
	mWd(0),
	mHe(0)
{
}

//==================================================================
DispDriverFile::~DispDriverFile()
{
	if ( mWd && mHe )
		write_JPEG_file( mpFileName, 100, mpBuffer, mWd, mHe );

	DSAFE_DELETE_ARRAY( mpBuffer );
}

//==================================================================
void DispDriverFile::SetSize( u_int w, u_int h )
{
	alloc( w, h );
}

//==================================================================
void DispDriverFile::UpdateRegion( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride )
{
	convert( x1, y1, w, h, pSrcData, srcStride );
}

//==================================================================
void DispDriverFile::Blit() const
{
}

//==================================================================
void DispDriverFile::alloc( u_int w, u_int h )
{
	if ( w == mWd && h == mHe )
		return;

	mWd = w;
	mHe = h;

	mpBuffer = DNEW u_char [ w * h * RI::NCOLS ];
}

//==================================================================
void DispDriverFile::convert( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride )
{
	u_char	*pDest = mpBuffer + (x1 + y1 * mWd) * RI::NCOLS;

	for (u_int j=0; j < h; ++j)
	{
		const float *pSrcData2 = pSrcData;
		pSrcData += srcStride;

		u_char	*pDest2 = pDest;
		pDest += mWd * RI::NCOLS;

		for (u_int i=0; i < w; ++i)
		{
			int	r = (int)(pSrcData2[0] * 255.0f);
			int	g = (int)(pSrcData2[1] * 255.0f);
			int	b = (int)(pSrcData2[2] * 255.0f);
			pSrcData2 += RI::NCOLS;

#if 1
			if ( r < 0 ) r = 0; else if ( r > 255 ) r = 255;
			if ( g < 0 ) g = 0; else if ( g > 255 ) g = 255;
			if ( b < 0 ) b = 0; else if ( b > 255 ) b = 255;
#else
			if ( r < 0 ) r = -r; if ( r > 255 ) r = 255;
			if ( g < 0 ) g = -g; if ( g > 255 ) g = 255;
			if ( b < 0 ) b = -b; if ( b > 255 ) b = 255;
#endif
			pDest2[0] = (u_char)r;
			pDest2[1] = (u_char)g;
			pDest2[2] = (u_char)b;
			pDest2 += RI::NCOLS;
		}
	}
}
