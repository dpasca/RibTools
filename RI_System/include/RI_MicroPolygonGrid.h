//==================================================================
/// RI_MicroPolygonGrid.h
///
/// Created by Davide Pasca - 2009/2/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

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
	SlColor	*mpDataCi;
	SlColor	*mpDataOi;
	SlColor	*mpDataCs;
	SlColor	*mpDataOs;

public:
	static const u_int	MAX_SIZE = 48 * 48;

	u_int			mXDim;
	u_int			mYDim;
	u_int			mPointsN;
	Point3			*mpPointsWS;
	Matrix44		mMtxLocalWorld;
	float			mURange[2];
	float			mVRange[2];
	SlSymbolList	mSymbols;
	
	MicroPolygonGrid();
	~MicroPolygonGrid();

	void Setup(u_int xdim,
			   u_int ydim,
			   const float uRange[2],
			   const float vRange[2],
			   const Matrix44 &mtxLocalWorld );
			   
	u_int GetPointsN() const { return mPointsN; }
	
	void Shade( Attributes &attribs );
};

//==================================================================
}

#endif
