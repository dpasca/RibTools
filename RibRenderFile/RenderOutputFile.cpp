//==================================================================
/// RenderOutputFile.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RenderOutputFile.h"

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
void RenderOutputFile::Update( u_int w, u_int h, const float *pSrcData )
{
	alloc( w, h );
	convert( w, h, pSrcData );

	FILE	*pFile;
	if ( 0 != fopen_s( &pFile, mpFileName, "wb" ) )
	{
		DASSTHROW( 0, ("Failed to open %s for writing", mpFileName) );
	}

	// mpBuffer

	fclose( pFile );
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

	mpBuffer = new u_char [ w * h * 3 ];
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
