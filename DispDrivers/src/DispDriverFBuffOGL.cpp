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

#if defined(__APPLE__)
    #include <OpenGL/gl.h>
#else
    #include <GL/gl.h>
#endif

#include "DSystem/include/DTypes.h"
#include "RI_System/include/RI_State.h"
#include "DispDrivers/include/DispDriverFBuffOGL.h"

//==================================================================
static unsigned int getNextPow2(unsigned int val)
{
    if (val == 0)
        return 0;

    for (unsigned int i = 1; i < 32; ++i)
    {
        const auto left = 1u << (i - 1);
        const auto right = 1u << i;
        if (val >= left && val <= right)
        {
            return right;
        }
    }

    DASSERT( 0 );
    return 0;
}

//==================================================================
// DispDriverFramebuffOGL
//==================================================================
DispDriverFBuffOGL::DispDriverFBuffOGL(const char* pFBuffOGLName, const DIMG::Image& srcImg)
{
    static_cast<void>(pFBuffOGLName);

    const auto* pExtensionsStr =
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));

    DASSTHROW(pExtensionsStr != nullptr, "OpenGL is not initialized");

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

    mImage.Init(
            srcImg.mWd,
            srcImg.mHe,
            4,
            DIMG::Image::ST_U8,
            (size_t)srcImg.mWd * 4 * sizeof(U8),
            "rgba" );

    DIMG::ConvertImages( mImage, srcImg );

    glGenTextures( 1, &mTexId );
    glBindTexture( GL_TEXTURE_2D, mTexId );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    if ( mSupportsNonPow2 )
    {
        mTexWd = mImage.mWd;
        mTexHe = mImage.mHe;

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
    else
    {
        mTexWd = getNextPow2( mImage.mWd );
        mTexHe = getNextPow2( mImage.mHe );
    
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RGBA,
            mTexWd,
            mTexHe,
            0,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            NULL );

        glTexSubImage2D(
            GL_TEXTURE_2D,
            0,
            0,
            0,
            mImage.mWd,
            mImage.mHe,
            GL_RGBA,
            GL_UNSIGNED_BYTE,
            mImage.GetPixelPtrR(0,0) );
    }
}

//==================================================================
float DispDriverFBuffOGL::GetTextureUMax() const
{
    return mTexWd ? static_cast<float>(mImage.mWd) / mTexWd : 0.0f;
}

//==================================================================
float DispDriverFBuffOGL::GetTextureVMax() const
{
    return mTexHe ? static_cast<float>(mImage.mHe) / mTexHe : 0.0f;
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
