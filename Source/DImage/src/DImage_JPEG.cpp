//==================================================================
/// DImage_JPEG.cpp
///
/// Created by Davide Pasca - 2010/1/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>

#define XMD_H	// shit !
extern "C" 
{
	#include "externals/libjpeg/jpeglib.h"
};
#undef XMD_H	// un-shit !

#include "DImage_JPEG.h"

//==================================================================
namespace DIMG
{

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
void SaveJPEG( const Image &img, const char *pFName )
{
	// make sure it's RGB
	if ( !img.IsSamplesNames( "rgb" ) || img.IsSamplesType( Image::ST_U8 ) )
	{
		Image	tmpImg( img.mWd, img.mHe, 3, Image::ST_U8, -1, "rgb" );
		DIMG::ConvertImages( tmpImg, img );

		write_JPEG_file( pFName, 100, (const JSAMPLE *)tmpImg.GetPixelPtrR(0,0), img.mWd, img.mHe );
	}
	else
	{
		write_JPEG_file( pFName, 100, (const JSAMPLE *)img.GetPixelPtrR(0,0), img.mWd, img.mHe );
	}
}

//==================================================================
}
