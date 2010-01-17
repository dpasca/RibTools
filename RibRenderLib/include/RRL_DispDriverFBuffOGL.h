//==================================================================
/// RRL_DispDriverFBuffOGL.h
///
/// Created by Davide Pasca - 2009/02/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_DISPDRIVERFBUFFOGL_H
#define RRL_DISPDRIVERFBUFFOGL_H

#include "RI_System/include/RI_Framework.h"

//==================================================================
// RenderOutputOpenGL
//==================================================================
class DispDriverFBuffOGL : public RI::DispDriverBase
{
	u_char	*mpBuffer;
	u_int	mTexId;
	u_int	mWd;
	u_int	mHe;
	u_int	mTexWd;
	u_int	mTexHe;

private:
	bool	mSupportsNonPow2;

public:
	DispDriverFBuffOGL();	
	~DispDriverFBuffOGL();

		void SetSize( u_int w, u_int h );
		void UpdateRegion( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride );
		void Blit() const;
		u_int GetCurWd() const {	return mWd; }
		u_int GetCurHe() const {	return mHe;	}

private:
	void alloc( u_int w, u_int h );
	void convert( u_int x1, u_int y1, u_int w, u_int h, const float *pSrcData, u_int srcStride );
};

#endif
