//==================================================================
/// RenderOutputFile.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RENDEROUTPUTFILE_H
#define RENDEROUTPUTFILE_H

#include "RI_System/include/RI_FrameworkREYES.h"

//==================================================================
// RenderOutputFile
//==================================================================
class RenderOutputFile : public RI::RenderOutputBase
{
	u_char		*mpBuffer;
	u_int		mWd;
	u_int		mHe;
	const char	*mpFileName;

public:
	RenderOutputFile( const char *pFileName );	
	~RenderOutputFile();

		void Update( u_int w, u_int h, const float *pSrcData );	
		void Blit() const;
		u_int GetCurWd() const {	return mWd; }
		u_int GetCurHe() const {	return mHe;	}

private:
	void alloc( u_int w, u_int h );
	void convert( u_int w, u_int h, const float *pSrcData );
};

#endif