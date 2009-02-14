/*
 *  RenderOutputOpenGL.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/12.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RENDEROUTPUTOPENGL_H
#define RENDEROUTPUTOPENGL_H

#include "RI_FrameworkBase.h"
#include "RenderOutputOpenGL.h"
#include <GLUT/glut.h>

//==================================================================
// RenderOutputOpenGL
//==================================================================
class RenderOutputOpenGL : public RI::RenderOutputBase
{
	u_char	*mpBuffer;
	u_int	mTexId;
	u_int	mWd;
	u_int	mHe;

public:
	RenderOutputOpenGL();	
	~RenderOutputOpenGL();

		void Update( u_int w, u_int h, const float *pSrcData );	
		void Blit() const;
		u_int GetCurWd() const {	return mWd; }
		u_int GetCurHe() const {	return mHe;	}

private:
	void alloc( u_int w, u_int h );
	void convert( u_int w, u_int h, const float *pSrcData );
};

#endif
