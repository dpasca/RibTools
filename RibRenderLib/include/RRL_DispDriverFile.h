//==================================================================
/// RRL_DispDriverFile.h
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_DISPDRIVERFILE_H
#define RRL_DISPDRIVERFILE_H

#include "RI_System/include/RI_Framework.h"

//==================================================================
// DispDriverFile
//==================================================================
class DispDriverFile : public RI::DispDriverBase
{
	u_char		*mpBuffer;
	u_int		mWd;
	u_int		mHe;
	const char	*mpFileName;

public:
	DispDriverFile( const char *pFileName );	
	~DispDriverFile();

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
