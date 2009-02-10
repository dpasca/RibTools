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
						const Matrix44 &mtxObjectCurrent )
{
	DSAFE_DELETE_ARRAY( mpPoints );
	mpPoints = new Point3[ (xdim+1) * (ydim+1) ];
	mXDim = xdim;
	mYDim = ydim;
	mMtxObjectCurrent = mtxObjectCurrent;
}

//==================================================================
}
