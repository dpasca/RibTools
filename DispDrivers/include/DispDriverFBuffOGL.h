//==================================================================
/// DispDriverFBuffOGL.h
///
/// Created by Davide Pasca - 2009/02/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DISPDRIVERFBUFFOGL_H
#define DISPDRIVERFBUFFOGL_H

#include "DImage/include/DImage.h"

//==================================================================
// RenderOutputOpenGL
//==================================================================
class DispDriverFBuffOGL
{
	DIMG::Image	mImage;
	u_int		mTexId;
	u_int		mTexWd;
	u_int		mTexHe;

private:
	bool	mSupportsNonPow2;

public:
	DispDriverFBuffOGL( const char *pFBuffOGLName, const DIMG::Image &srcImg );	
	~DispDriverFBuffOGL();

	void Blit() const;

	u_int GetCurWd() const {	return mImage.mWd; }
	u_int GetCurHe() const {	return mImage.mHe; }
};

#endif
