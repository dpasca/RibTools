/*
 *  RI_MicroPolygonGrid.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Attributes.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
MicroPolygonGrid::MicroPolygonGrid() :
	mXDim(0),
	mYDim(0),
	mpPointsWS(0),
	mPointsN(0),
	mpDataCi(0),
	mpDataOi(0),
	mpDataCs(0),
	mpDataOs(0)
{
	static const size_t allocN = MAX_SIZE;

	SlSymbol	symbol;
	symbol.Reset();

	// allocate some standard varying params
	symbol.mIsVarying = true;
	symbol.mArraySize = allocN;
	symbol.mStorage = SlSymbol::PARAMETER;

	symbol.mName = "P";
	symbol.mType = SlSymbol::POINT;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mSymbols.push_back( symbol );
	mpPointsWS = (Point3 *)symbol.mpDefaultVal;

	symbol.mName = "I";
	symbol.mType = SlSymbol::VECTOR;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mSymbols.push_back( symbol );

	symbol.mName = "N";
	symbol.mType = SlSymbol::NORMAL;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mSymbols.push_back( symbol );

	symbol.mName = "Ng";
	symbol.mType = SlSymbol::NORMAL;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mSymbols.push_back( symbol );

	symbol.mName = "Ci";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mpDataCi = (Color *)symbol.mpDefaultVal;
	mSymbols.push_back( symbol );
	
	symbol.mName = "Oi";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mpDataOi = (Color *)symbol.mpDefaultVal;
	mSymbols.push_back( symbol );
	
	symbol.mName = "Cs";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mpDataCs = (Color *)symbol.mpDefaultVal;
	mSymbols.push_back( symbol );
	
	symbol.mName = "Os";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = NULL;
	symbol.AllocData();
	mpDataOs = (Color *)symbol.mpDefaultVal;
	mSymbols.push_back( symbol );
}

//==================================================================
MicroPolygonGrid::~MicroPolygonGrid()
{
	for (size_t i=0; i < mSymbols.size(); ++i)
	{
		mSymbols[i].FreeData();
	}
}

//==================================================================
static void fillColArray( Color *pCol, size_t n, float r, float g, float b )
{
	for (size_t i=0; i < n; ++i)
	{
		pCol[i].Set( r, g, b );
	}
}

//==================================================================
void MicroPolygonGrid::Setup(
						u_int xdim,
						u_int ydim,
						const float uRange[2],
						const float vRange[2],
						const Matrix44 &mtxLocalWorld )
{
	mXDim = xdim;
	mYDim = ydim;
	mPointsN = mXDim * mYDim;
	mMtxLocalWorld = mtxLocalWorld;
	mURange[0] = uRange[0];
	mURange[1] = uRange[1];
	mVRange[0] = vRange[0];
	mVRange[1] = vRange[1];

	DASSERT( mPointsN <= MAX_SIZE );

	// $$$ why +1 ? For derivatives ?
	//mpPoints = new Point3[ (xdim+1) * (ydim+1) ];

	fillColArray( mpDataCi, mPointsN, 1.0f, 0.0f, 0.0f );
	fillColArray( mpDataOi, mPointsN, 0.0f, 1.0f, 0.0f );
	fillColArray( mpDataCs, mPointsN, 1.0f, 1.0f, 1.0f );
	fillColArray( mpDataOs, mPointsN, 1.0f, 1.0f, 1.0f );
}

//==================================================================
void MicroPolygonGrid::Shade( Attributes &attribs )
{
	attribs.mShaderInstance.Run( *this, &attribs );
}

//==================================================================
}
