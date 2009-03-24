/*
 *  RI_MicroPolygonGrid.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_MICROPOLYGONGRID_H
#define RI_MICROPOLYGONGRID_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include "DMath/include/DMath.h"
#include "RI_SlSymbol.h"

//==================================================================
namespace RI
{

//==================================================================
class Attributes;

//==================================================================
class MicroPolygonGrid
{
public:
	static const u_int	MAX_SIZE = 64 * 64;

	u_int			mXDim;
	u_int			mYDim;
	u_int			mPointsN;
	Point3			*mpPoints;
	Matrix44		mMtxObjectCurrent;
	float			mURange[2];
	float			mVRange[2];
	SlSymbolList	mSymbols;
	
	MicroPolygonGrid() : mXDim(0), mYDim(0), mpPoints(0), mPointsN(0)
	{
	}

	~MicroPolygonGrid();

	void Setup(u_int xdim,
			   u_int ydim,
			   const float uRange[2],
			   const float vRange[2],
			   const Matrix44 &mtxObjectCurrent );
			   
	u_int GetPointsN() const { return mPointsN; }
	
	void Shade( Attributes &attribs );
};

//==================================================================
}

#endif
