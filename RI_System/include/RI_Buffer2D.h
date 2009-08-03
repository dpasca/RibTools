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
	float	*mpData;

public:
	u_int	mWd;
	u_int	mHe;

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
		mpData = DNEW float [ mWd * NCHANS * mHe ];
	}

	void Free()
	{
		DSAFE_DELETE_ARRAY( mpData );
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

	const float *GetSampleCPtr( int x, int y ) const
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

	void CopyRect( int dx1, int dy1, const Buffer2D<NCHANS> &fromBuff )
	{
		int	w = (int)fromBuff.mWd;
		int	h = (int)fromBuff.mHe;

		int	sx1 = 0;
		int	sy1 = 0;

		if ( dx1 < 0 )				{ sx1 -= dx1; w += dx1; dx1 = 0; }
		if ( dy1 < 0 )				{ sy1 -= dy1; h += dy1; dy1 = 0; }

		if ( dx1 + w > (int)mWd )	{ w = (int)mWd - dx1; }
		if ( dy1 + h > (int)mHe )	{ h = (int)mHe - dy1; }

		const	float *pSrc = fromBuff.mpData;
				float *pDes = mpData;

		int	sPitch = fromBuff.mWd * NCHANS;
		int	dPitch = mWd * NCHANS;

		pSrc += sx1 * NCHANS + sy1 * sPitch;
		pDes += dx1 * NCHANS + dy1 * dPitch;

		float	*pDesEnd = pDes + mWd * mHe * NCHANS;

		w *= NCHANS;
		for (int i=0; i < h; ++i)
		{
			for (int j=0; j < w; ++j)
			{
				DASSERT( (pDes + j) < pDesEnd );
				pDes[j] = pSrc[j];
			}
			pSrc += sPitch;
			pDes += dPitch;
		}
	}

	const float *GetData() const { return mpData;	}
	u_int		GetWd() const	 { return mWd;	}
	u_int		GetHe() const	 { return mHe;	}
};

//==================================================================
}

#endif