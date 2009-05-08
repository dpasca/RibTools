//==================================================================
/// RenderOutputFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RenderOutputFile.h"

extern "C" 
{
	#include "jpeglib.h"
};

//==================================================================
// RenderOutputFile
//==================================================================
RenderOutputFile::RenderOutputFile( const char *pFileName ) :
	mpFileName(pFileName),
	mpBuffer(NULL),
	mWd(0),
	mHe(0)
{
}

//==================================================================
RenderOutputFile::~RenderOutputFile()
{
	DSAFE_DELETE_ARRAY( mpBuffer );
}

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
void RenderOutputFile::Update( u_int w, u_int h, const float *pSrcData )
{
	alloc( w, h );
	convert( w, h, pSrcData );

	write_JPEG_file( mpFileName, 100, mpBuffer, w, h );
}

//==================================================================
void RenderOutputFile::Blit() const
{
}

//==================================================================
void RenderOutputFile::alloc( u_int w, u_int h )
{
	if ( w == mWd && h == mHe )
		return;

	mWd = w;
	mHe = h;

	mpBuffer = DNEW u_char [ w * h * 3 ];
}

//==================================================================
void RenderOutputFile::convert( u_int w, u_int h, const float *pSrcData )
{
	u_char	*pDest = mpBuffer;

	for (u_int j=0; j < h; ++j)
		for (u_int i=0; i < w; ++i)
		{
			int	r = (int)(pSrcData[0] * 255.0f);
			int	g = (int)(pSrcData[1] * 255.0f);
			int	b = (int)(pSrcData[2] * 255.0f);
			pSrcData += 3;

#if 1
			if ( r < 0 ) r = 0; else if ( r > 255 ) r = 255;
			if ( g < 0 ) g = 0; else if ( g > 255 ) g = 255;
			if ( b < 0 ) b = 0; else if ( b > 255 ) b = 255;
#else
			if ( r < 0 ) r = -r; if ( r > 255 ) r = 255;
			if ( g < 0 ) g = -g; if ( g > 255 ) g = 255;
			if ( b < 0 ) b = -b; if ( b > 255 ) b = 255;
#endif
			pDest[0] = (u_char)r;
			pDest[1] = (u_char)g;
			pDest[2] = (u_char)b;
			pDest += 3;
		}
}
