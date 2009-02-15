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
		mpData = new float [ mWd * 3 * mHe ];
	}
	
	void Clear()
	{
		memset( mpData, 0, sizeof(float) * mWd * 3 * mHe );
	}
	
	void SetSample( int x, int y, const float *pVal )
	{
		if ( x >= 0 && y >= 0 && x < (int)mWd && y < (int)mHe )
		{
			float	*pDest = &mpData[ (x + mWd * y) * 3 ];
			pDest[0] = pVal[0];
			pDest[1] = pVal[1];
			pDest[2] = pVal[2];
		}
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
	DestBuffer			mDestBuff;

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