/*
 *  RI_HiderREYES.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_HIDERREYES_H
#define RI_HIDERREYES_H

#include "RI_Options.h"
#include "RI_HiderBase.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class Primitive;

//==================================================================
/// DestBuffer
//==================================================================
template <u_int NCHANS>
class DestBuffer
{
	float				*mpData;

public:
	u_int				mWd;
	u_int				mHe;

public:
	DestBuffer() :
		mpData(NULL),
		mWd(0),
		mHe(0)
	{
	}

	~DestBuffer()
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
	
	const float *GetData() const { return mpData;	}
	u_int		GetWd() const	 { return mWd;	}
	u_int		GetHe() const	 { return mHe;	}
};

//==================================================================
/// HiderREYES
//==================================================================
class HiderREYES : public HiderBase
{
	Options				mOptions;
	DVec<Primitive *>	mpPrims;
	DestBuffer<3>		mDestBuff;
	DestBuffer<1>		mZBuff;

public:
	HiderREYES();
	~HiderREYES();

	void WorldBegin(
			const Options &opt,
			const Matrix44 &mtxWorldCamera );

	void Insert(
				Primitive			*pPrim,
				 const Attributes	&attr,
				 const Transform	&xform );

	void InsertSplitted(	
						Primitive	*pSplitPrim,
						Primitive	&srcPrim
						);

	void Remove( Primitive *pPrim );

	void WorldEnd();
	
	float RasterEstimate( const Bound &b, Matrix44 &mtxLocalCamera ) const;

	void Hide( MicroPolygonGrid &g );
	
	u_int		GetOutputDataWd() const	{ return mDestBuff.GetWd();	}
	u_int		GetOutputDataHe() const	{ return mDestBuff.GetHe();	}
	const float *GetOutputData() const { return mDestBuff.GetData(); }

	DVec<Primitive *>	&GetPrimList()	{ return mpPrims;	}
	
private:
	void pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n );
};

//==================================================================
}

#endif