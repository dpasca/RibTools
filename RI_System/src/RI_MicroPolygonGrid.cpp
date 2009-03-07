/*
 *  RI_MicroPolygonGrid.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Attributes.h"
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
	mXDim = xdim;
	mYDim = ydim;
	mPointsN = mXDim * mYDim;
	mMtxObjectCurrent = mtxObjectCurrent;
	mURange[0] = uRange[0];
	mURange[1] = uRange[1];
	mVRange[0] = vRange[0];
	mVRange[1] = vRange[1];

	// $$$ why +1 ? For derivatives ?
	//mpPoints = new Point3[ (xdim+1) * (ydim+1) ];

	SlSymbol	symbol;

	symbol.Reset();

	// allocate some standard varying params
	symbol.mIsVarying = true;
	symbol.mArraySize = mPointsN;
	symbol.mStorage = SlSymbol::PARAMETER;

	symbol.mName = "P";
	symbol.mType = SlSymbol::POINT;
	symbol.mpDefaultVal = new Point3 [ mPointsN ];
	mSymbols.push_back( symbol );
	mpPoints = (Point3 *)symbol.mpDefaultVal;

	symbol.mName = "Ci";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = new Color [ mPointsN ];
	mSymbols.push_back( symbol );
	
	symbol.mName = "Oi";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = new Color [ mPointsN ];
	mSymbols.push_back( symbol );
	
	symbol.mName = "Cs";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = new Color [ mPointsN ];
	mSymbols.push_back( symbol );
	
	symbol.mName = "Os";
	symbol.mType = SlSymbol::COLOR;
	symbol.mpDefaultVal = new Color [ mPointsN ];
	mSymbols.push_back( symbol );	
}

//==================================================================
MicroPolygonGrid::~MicroPolygonGrid()
{
	for (size_t i=0; i < mSymbols.size(); ++i)
	{
		if ( mSymbols[i].mIsVarying )
		{
			switch ( mSymbols[i].mType )
			{
			case SlSymbol::FLOAT:	delete [] ((float *)mSymbols[i].mpDefaultVal); break;
			case SlSymbol::POINT:	delete [] ((Point3 *)mSymbols[i].mpDefaultVal ); break;
			case SlSymbol::COLOR:	delete [] ((Color *)mSymbols[i].mpDefaultVal ); break;
			case SlSymbol::STRING:	delete [] ((char *)mSymbols[i].mpDefaultVal ); break;
			case SlSymbol::VECTOR:	delete [] ((Vector3 *)mSymbols[i].mpDefaultVal ); break;
			case SlSymbol::NORMAL:	delete [] ((Vector3 *)mSymbols[i].mpDefaultVal ); break;
			case SlSymbol::MATRIX:	delete [] ((Matrix44 *)mSymbols[i].mpDefaultVal ); break;

			default:
				DASSERT( 0 );
				break;
			}
		}
	}
}

//==================================================================
void MicroPolygonGrid::Shade( Attributes &attribs )
{
	attribs.mShaderInstance.Run( *this );
}

	
//==================================================================
}
