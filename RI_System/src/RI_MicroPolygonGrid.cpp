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
void *MicroPolygonGrid::addGlobalSym( const char *pName, Symbol::Type symType, bool isVarying )
{
	Symbol *pSymbol;

	// allocate some standard varying params
	pSymbol = mSymbols.Grow();
	pSymbol->mIsVarying = isVarying;
	pSymbol->mStorage = Symbol::GLOBAL;
	pSymbol->mName = pName;
	pSymbol->mType = symType;
	
	return pSymbol->AllocData( isVarying ? MAX_SIZE : 1 );
}

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
	mDispRunCtx(mSymbols, MAX_SIZE)
{
	// symbols added to match globals declared in RSLC_Builtins.sl

	mpDataCs = (SlColor *)	addGlobalSym( "Cs",		Symbol::COLOR );
	mpDataOs = (SlColor *)	addGlobalSym( "Os",		Symbol::COLOR );
	mpPointsWS = (SlVec3 *)	addGlobalSym( "P",		Symbol::POINT );
							//addGlobalSym( "dPdu",	Symbol::VECTOR );	// or point ?
							//addGlobalSym( "dPdv",	Symbol::VECTOR );	// or point ?
							addGlobalSym( "oodu",	Symbol::FLOAT );
							addGlobalSym( "oodv",	Symbol::FLOAT );
							addGlobalSym( "N",		Symbol::NORMAL );
							addGlobalSym( "Ng",		Symbol::NORMAL );
							addGlobalSym( "u",		Symbol::FLOAT );
							addGlobalSym( "v",		Symbol::FLOAT );
							addGlobalSym( "du",		Symbol::FLOAT );
							addGlobalSym( "dv",		Symbol::FLOAT );
							addGlobalSym( "s",		Symbol::FLOAT );
							addGlobalSym( "t",		Symbol::FLOAT );
							addGlobalSym( "L",		Symbol::VECTOR );
							addGlobalSym( "Cl",		Symbol::COLOR );
							addGlobalSym( "I",		Symbol::VECTOR );
	mpDataCi = (SlColor *)	addGlobalSym( "Ci",		Symbol::COLOR );
	mpDataOi = (SlColor *)	addGlobalSym( "Oi",		Symbol::COLOR );
							addGlobalSym( "E",		Symbol::POINT, false );

	// initialize these.. do it here because it's not proper/safe
	// to do in the constructor..
	mSurfRunCtx.Init( this );
	mDispRunCtx.Init( this );
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
						const Matrix44 &mtxLocalWorld,
						const Matrix44 &mtxWorldCamera )
{
	mXDim = xdim;
	mXBlocks = RI_GET_SIMD_BLOCKS( xdim );

	// ensure that the width is multiple of the SIMD width
	DASSERT( (mXBlocks * RI_SIMD_BLK_LEN) == mXDim );

	mYDim = ydim;
	mPointsN = mXDim * mYDim;
	mURange[0] = uRange[0];
	mURange[1] = uRange[1];
	mVRange[0] = vRange[0];
	mVRange[1] = vRange[1];

	mMtxLocalWorld	= mtxLocalWorld;
	mMtxWorldCamera	= mtxWorldCamera;

	mMtxLocalCamera = mMtxLocalWorld * mMtxWorldCamera;

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
