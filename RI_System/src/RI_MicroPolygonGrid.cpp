//==================================================================
/// RI_MicroPolygonGrid.cpp
///
/// Created by Davide Pasca - 2009/2/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include <memory>
#include "RI_Attributes.h"
#include "RI_MicroPolygonGrid.h"
#include "RI_SlRunContext.h"

//==================================================================
using std::auto_ptr;

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
	mpDataOs(0),
	mSurfRunCtx(mSymbols, MAX_SIZE),
	mDispRunCtx(mSymbols, MAX_SIZE),
	mSymbols(16)
{
	static const size_t allocN = MAX_SIZE;

	SlSymbol *pSymbol;

	// allocate some standard varying params
	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "P";
	pSymbol->mType = SlSymbol::POINT;
	mpPointsWS = (SlVec3 *)pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "oodu";
	pSymbol->mType = SlSymbol::FLOAT;
	pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "oodv";
	pSymbol->mType = SlSymbol::FLOAT;
	pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "I";
	pSymbol->mType = SlSymbol::VECTOR;
	pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "N";
	pSymbol->mType = SlSymbol::NORMAL;
	pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "Ng";
	pSymbol->mType = SlSymbol::NORMAL;
	pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "Ci";
	pSymbol->mType = SlSymbol::COLOR;
	mpDataCi = (SlColor *)pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "Oi";
	pSymbol->mType = SlSymbol::COLOR;
	mpDataOi = (SlColor *)pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "Cs";
	pSymbol->mType = SlSymbol::COLOR;
	mpDataCs = (SlColor *)pSymbol->AllocData( MAX_SIZE );

	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = true;
	pSymbol->mStorage = SlSymbol::PARAMETER;
	pSymbol->mName = "Os";
	pSymbol->mType = SlSymbol::COLOR;
	mpDataOs = (SlColor *)pSymbol->AllocData( MAX_SIZE );
}

//==================================================================
MicroPolygonGrid::~MicroPolygonGrid()
{
}

//==================================================================
static void fillColArray( SlColor *pCol, size_t n, float r, float g, float b )
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( n );

	for (size_t i=0; i < blocksN; ++i)
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
	mXBlocks = RI_GET_SIMD_BLOCKS( xdim );

	// ensure that the width is multiple of the SIMD width
	DASSERT( (mXBlocks * RI_SIMD_BLK_LEN) == mXDim );

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
void MicroPolygonGrid::Displace( const Attributes &attribs )
{
	if ( attribs.mDisplaceSHI.IsSet() )
	{
		mDispRunCtx.Setup(
						attribs,
						&attribs.mDisplaceSHI,
						RI_GET_SIMD_BLOCKS( mYDim ),
						mYDim,
						mPointsN );

		mDispRunCtx.mpShaderInst->Run( mDispRunCtx );
	}
}

//==================================================================
void MicroPolygonGrid::Shade( const Attributes &attribs )
{
	mSurfRunCtx.Setup(
					attribs,
					&attribs.mSurfaceSHI,
					RI_GET_SIMD_BLOCKS( mYDim ),
					mYDim,
					mPointsN );

	mSurfRunCtx.mpShaderInst->Run( mSurfRunCtx );
}

//==================================================================
}
