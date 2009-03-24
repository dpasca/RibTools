/*
 *  RenderOutputOpenGL.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/12.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RenderOutputOpenGL.h"
#ifdef _MSC_VER
	#include <GL/glut.h>
#else
	#include <GLUT/glut.h>
#endif

//==================================================================
// RenderOutputOpenGL
//==================================================================
RenderOutputOpenGL::RenderOutputOpenGL() :
	mpBuffer(NULL), mTexId(0), mWd(0), mHe(0), mTexWd(0), mTexHe(0)
{
	const char *pExtensionsStr = (const char *)glGetString( GL_EXTENSIONS );
	
	if (
		0 == strstr( pExtensionsStr, "GL_ARB_texture_non_power_of_two" ) &&
		0 == strstr( pExtensionsStr, "GL_ARB_texture_rectangle" ) &&
		0 == strstr( pExtensionsStr, "GL_EXT_texture_rectangle" )
	)
	{
		mSupportsNonPow2 = false;
		//DASSTHROW( false, ("Non power of two texture not supported !!") );
	}
	else
	{
		mSupportsNonPow2 = true;
	}

	glGenTextures( 1, &mTexId );
	glBindTexture( GL_TEXTURE_2D, mTexId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}

//==================================================================
RenderOutputOpenGL::~RenderOutputOpenGL()
{
	glDeleteTextures( 1, &mTexId );
	DSAFE_DELETE_ARRAY( mpBuffer );
}

//==================================================================
void RenderOutputOpenGL::Update( u_int w, u_int h, const float *pSrcData )
{
	glBindTexture( GL_TEXTURE_2D, mTexId );

	//glPixelStorei( GL_PACK_ALIGNMENT, 1 );
	alloc( w, h );
	convert( w, h, pSrcData );
	glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0,
					0,
					w,
					h,
					GL_RGBA,
					GL_UNSIGNED_BYTE,
					mpBuffer );

	//glPixelStorei( GL_PACK_ALIGNMENT, 0 );
}

//==================================================================
void RenderOutputOpenGL::Blit() const
{
	//glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	gluOrtho2D( 0, (float)mWd, (float)mHe, 0 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, mTexId );
	
	float w = (float)mWd;
	float h = (float)mHe;

	float s2 = (float)mWd / mTexWd;
	float t2 = (float)mHe / mTexHe;

	glColor3f( 1, 1, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f( 0,  0 );  glVertex2f( w*0, h*0 );
		glTexCoord2f( s2, 0 );  glVertex2f( w*1, h*0 );
		glTexCoord2f( s2, t2 ); glVertex2f( w*1, h*1 );
		glTexCoord2f( 0,  t2 ); glVertex2f( w*0, h*1 );
	glEnd();
}

//==================================================================
static u_int getNextPow2( u_int val )
{
	if ( val == 0 )
		return 0;

	for (u_int i=1; i < 32; ++i)
	{
		u_int	l = 1 << i-1;
		u_int	r = 1 << i;
		if ( val >= l && val <= r )
		{
			return r;
		}
	}

	DASSERT( 0 );
	return 0;
}

//==================================================================
void RenderOutputOpenGL::alloc( u_int w, u_int h )
{
	if ( w == mWd && h == mHe )
		return;

	mWd = w;
	mHe = h;

	if ( mSupportsNonPow2 )
	{
		mTexWd = mWd;
		mTexHe = mHe;
	}
	else
	{
		mTexWd = getNextPow2( mWd );
		mTexHe = getNextPow2( mHe );
	}

	glTexImage2D( GL_TEXTURE_2D, 0, 3, mTexWd, mTexHe, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	mpBuffer = new u_char [ w * h * 4 ];
	
	glutReshapeWindow( w, h );
}

//==================================================================
void RenderOutputOpenGL::convert( u_int w, u_int h, const float *pSrcData )
{
	u_char	*pDest = mpBuffer;

	for (u_int j=0; j < h; ++j)
		for (u_int i=0; i < w; ++i)
		{
			float	r = pSrcData[0] * 255.0f;
			float	g = pSrcData[1] * 255.0f;
			float	b = pSrcData[2] * 255.0f;
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
			pDest[3] = 255;
			pDest += 4;
		}
}
