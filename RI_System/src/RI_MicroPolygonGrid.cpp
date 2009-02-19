/*
 *  RI_MicroPolygonGrid.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
void MicroPolygonGrid::Setup(
						u_int xdim,
						u_int ydim,
						const float uRange[2],
						const float vRange[2],
						const Matrix44 &mtxObjectCurrent )
{
	DSAFE_DELETE_ARRAY( mpPoints );
	
	// $$$ why +1 ? For derivatives ?
	mpPoints = new Point3[ (xdim+1) * (ydim+1) ];

	mXDim = xdim;
	mYDim = ydim;
	mPointsN = mXDim * mYDim;
	mMtxObjectCurrent = mtxObjectCurrent;
	mURange[0] = uRange[0];
	mURange[1] = uRange[1];
	mVRange[0] = vRange[0];
	mVRange[1] = vRange[1];
}

//==================================================================
}
