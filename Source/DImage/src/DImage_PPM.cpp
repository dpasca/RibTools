//==================================================================
/// DImage_PPM.cpp
///
/// Created by Davide Pasca - 2010/1/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <stdio.h>
#include "DImage_PPM.h"

//==================================================================
namespace DIMG
{

//==================================================================
void SavePPM( const Image &img, const char *pFName )
{
	FILE *pFile;

	int err = fopen_s( &pFile, pFName, "wb" );
	DASSTHROW( err == 0, ("Could not write %s", pFName) );

	DASSTHROW(
		img.mSampPerPix <= 3 &&
		img.mBytesPerSamp == 1,
			("Unsupported image format. Cannot write %s", pFName) );

	fprintf_s( pFile,
		"P6\n"
		"# %s\n"
		"%i %i\n"
		"%i\n",
			pFName,
			img.mWd, img.mHe,
			(int)(1 << (img.mBytesPerSamp<<3)) - 1 );

	for (u_int y=0; y < img.mHe; ++y)
	{
		for (u_int x=0; x < img.mWd; ++x)
		{
			const U8	*pPix = img.GetPixelPtrR( x, y );

			U8	col[3];

			if ( img.mSampPerPix == 1 )
				col[0] = col[1] = col[2] = pPix[0];
			else
			if ( img.mSampPerPix == 2 )
			{
				col[0] = pPix[0];
				col[1] = pPix[1];
				col[2] = 0;
			}
			else
			if ( img.mSampPerPix == 3 )
			{
				col[0] = pPix[0];
				col[1] = pPix[1];
				col[2] = pPix[2];
			}

			fwrite( col, 1, 3, pFile );
		}
	}

	fclose( pFile );
}

//==================================================================
}
