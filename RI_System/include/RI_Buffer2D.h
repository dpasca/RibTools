//==================================================================
/// RI_Buffer2D.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_BUFFER2D_H
#define RI_BUFFER2D_H

#include "DSystem/include/DTypes.h"

//==================================================================
namespace RI
{

//==================================================================
/// Buffer2D
//==================================================================
template <u_int NCHANS>
class Buffer2D
{
	float				*mpData;

public:
	u_int				mWd;
	u_int				mHe;

public:
	Buffer2D() :
		mpData(NULL),
		mWd(0),
		mHe(0)
	{
	}

	~Buffer2D()
	{
		DSAFE_DELETE_ARRAY( mpData );
	}

	void Setup( u_int wd, u_int he )
	{
		mWd	= wd;
		mHe	= he;
		mpData = new float [ mWd * NCHANS * mHe ];
	}
	
	void Clear()
	{
		memset( mpData, 0, sizeof(float) * mWd * NCHANS * mHe );
	}

	void Fill( float val )
	{
		size_t	size = mWd * NCHANS * mHe;
		for(size_t i=0; i < size; ++i)
			mpData[i] = val;
	}
	
	void SetSample( int x, int y, const float *pVal )
	{
		if ( x >= 0 && y >= 0 && x < (int)mWd && y < (int)mHe )
		{
			float	*pDest = &mpData[ (x + mWd * y) * NCHANS ];
			for (u_int ci=0; ci < NCHANS; ++ci)
				pDest[ci] = pVal[ci];
		}
	}

	float *GetSamplePtr( int x, int y )
	{
		if ( x >= 0 && y >= 0 && x < (int)mWd && y < (int)mHe )
		{
			return &mpData[ (x + mWd * y) * NCHANS + 0 ];
		}
		else
			return NULL;
	}

	void DrawHLine( int x1, int y, int x2, const float *pVal )
	{
		for (int x=x1; x <= x2; ++x)
			SetSample( x, y, pVal );
	}

	void DrawVLine( int x, int y1, int y2, const float *pVal )
	{
		for (int y=y1; y <= y2; ++y)
			SetSample( x, y, pVal );
	}

	const float *GetData() const { return mpData;	}
	u_int		GetWd() const	 { return mWd;	}
	u_int		GetHe() const	 { return mHe;	}
};

//==================================================================
}

#endif