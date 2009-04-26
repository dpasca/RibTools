/*
 *  RI_HiderBase.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_HIDERBASE_H
#define RI_HIDERBASE_H

#include "RI_Base.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;
class Primitive;

//==================================================================
/// Bucket
//==================================================================
class Bucket
{
public:
	int	mX1;
	int	mY1;
	int	mX2;
	int	mY2;

	DVec<Primitive *>	mpPrims;

public:
	Bucket( int x1, int y1, int x2, int y2 ) :
		mX1(x1),
		mY1(y1),
		mX2(x2),
		mY2(y2)
	{
	}

	~Bucket()
	{
		for (size_t i=0; i < mpPrims.size(); ++i)
			delete mpPrims[i];
	}

	bool Contains( int x, int y )
	{
		return x >= mX1 && y >= mY1 && x < mX2 && y < mY2;
	}


	DVec<Primitive *>	&GetPrimList()	{ return mpPrims;	}
};

//==================================================================
/// HiderBase
//==================================================================
class HiderBase
{
public:
	Matrix44		mMtxWorldCamera;
	Matrix44		mMtxWorldProj;
	float			mHalfXRes;
	float			mHalfYRes;

public:
	HiderBase()				{}
	virtual ~HiderBase()	{}

	virtual void WorldBegin(
					const Options &opt,
					const Matrix44 &mtxWorldCamera
				) = 0;

	virtual void Insert( Primitive *pPrim ) = 0;

	virtual void InsertSplitted(	
						Primitive			*pSplitPrim,
						Primitive			&srcPrim
						) = 0;

	virtual void Remove( Primitive *pPrim ) = 0;

	virtual void WorldEnd() = 0;

	virtual float RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld ) const = 0;

	virtual void Hide( MicroPolygonGrid &g ) = 0;
	
	virtual u_int		GetOutputDataWd() const = 0;
	virtual u_int		GetOutputDataHe() const = 0;
	virtual const float *GetOutputData() const = 0;
};

//==================================================================
}

#endif