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

#include "DTypes.h"
#include "DContainers.h"
#include "DMath.h"

//==================================================================
namespace RI
{

//==================================================================
class MicroPolygonGrid
{
public:
	u_int		mXDim;
	u_int		mYDim;
	Point3		*mpPoints;
	Matrix44	mMtxObjectCurrent;
	
	MicroPolygonGrid() : mXDim(0), mYDim(0), mpPoints(0)
	{
	}

	~MicroPolygonGrid()
	{
		DSAFE_DELETE_ARRAY( mpPoints );
	}

	void Setup( u_int xdim, u_int ydim, const Matrix44 &mtxObjectCurrent );
};

//==================================================================
}

#endif
