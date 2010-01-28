//==================================================================
/// DispDriverFBuffOGL.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

//#include "stdafx.h"

#if defined(WIN32)
	#include <Windows.h>
	#pragma comment (lib, "opengl32.lib")
#endif

#include <GL/GL.h>

#include "DSystem/include/DTypes.h"
#include "RI_System/include/RI_State.h"
#include "DispDrivers/include/DispDriverFBuffOGL.h"

//==================================================================
static u_int getNextPow2( u_int val )
{
	if ( val == 0 )
		return 0;

	for (u_int i=1; i < 32; ++i)
	{
		u_int	l = 1 << (i-1);
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
// DispDriverFramebuffOGL
//==================================================================
DispDriverFBuffOGL::DispDriverFBuffOGL( const char *pFBuffOGLName, const DIMG::Image &srcImg ) :
	mTexId(0), mTexWd(0), mTexHe(0)
{
	const char *pExtensionsStr = (const char *)glGetString( GL_EXTENSIONS );

	DASSTHROW( pExtensionsStr != NULL, ("OpenGL not initialized ?") );

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

	mImage.Init( srcImg.mWd, srcImg.mHe, 4, DIMG::Image::ST_U8, -1, "rgba" );
	DIMG::ConvertImages( mImage, srcImg );

	glGenTextures( 1, &mTexId );
	glBindTexture( GL_TEXTURE_2D, mTexId );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	if ( mSupportsNonPow2 )
	{
		mTexWd = mImage.mWd;
		mTexHe = mImage.mHe;
	}
	else
	{
		mTexWd = getNextPow2( mImage.mWd );
		mTexHe = getNextPow2( mImage.mHe );
	}
	glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RGBA,
			mTexWd,
			mTexHe,
			0,
			GL_RGBA,
			GL_UNSIGNED_BYTE,
			mImage.GetPixelPtrR(0,0) );
}

//==================================================================
DispDriverFBuffOGL::~DispDriverFBuffOGL()
{
	glDeleteTextures( 1, &mTexId );
}

//==================================================================
void DispDriverFBuffOGL::Blit() const
{
	//glViewport( 0, 0, w, h );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	glOrtho( 0, (float)mImage.mWd, (float)mImage.mHe, 0, 0, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();

	glDisable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glEnable( GL_TEXTURE_2D );

	glBindTexture( GL_TEXTURE_2D, mTexId );

	float w = (float)mImage.mWd;
	float h = (float)mImage.mHe;

	float s2 = (float)mImage.mWd / mTexWd;
	float t2 = (float)mImage.mHe / mTexHe;

	glColor3f( 1, 1, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f( 0,  0 );  glVertex2f( w*0, h*0 );
		glTexCoord2f( s2, 0 );  glVertex2f( w*1, h*0 );
		glTexCoord2f( s2, t2 ); glVertex2f( w*1, h*1 );
		glTexCoord2f( 0,  t2 ); glVertex2f( w*0, h*1 );
	glEnd();
}
