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
void *MicroPolygonGrid::addSymI( const SymbolList &globalSyms, const char *pName )
{
	const Symbol	*pSrcSym = globalSyms.FindSymbol( pName );

	SymbolI	*pSymI = mSymbolIs.AddInstance( *pSrcSym, MP_GRID_MAX_SIZE );

	return pSymI->GetRWData();
}

//==================================================================
MicroPolygonGrid::MicroPolygonGrid( const SymbolList &globalSyms ) :
	mXDim(0),
	mYDim(0),
	mpPointsWS(0),
	mPointsN(0),
	mpDataCi(0),
	mpDataOi(0),
	mpDataCs(0),
	mpDataOs(0),
	mSurfRunCtx(mSymbolIs, MP_GRID_MAX_SIZE),
	mDispRunCtx(mSymbolIs, MP_GRID_MAX_SIZE)
{
	// symbols added to match globals declared in RSLC_Builtins.sl

	mpDataCs = (SlColor *)	addSymI( globalSyms, "Cs"	);
	mpDataOs = (SlColor *)	addSymI( globalSyms, "Os"	);
	mpPointsWS = (SlVec3 *)	addSymI( globalSyms, "P"	);
							addSymI( globalSyms, "dPdu"	);
							addSymI( globalSyms, "dPdv"	);
							addSymI( globalSyms, "_oodu");
							addSymI( globalSyms, "_oodv");
							addSymI( globalSyms, "N"	);
							addSymI( globalSyms, "Ng"	);
							addSymI( globalSyms, "u"	);
							addSymI( globalSyms, "v"	);
							addSymI( globalSyms, "du"	);
							addSymI( globalSyms, "dv"	);
							addSymI( globalSyms, "s"	);
							addSymI( globalSyms, "t"	);
							addSymI( globalSyms, "L"	);
							addSymI( globalSyms, "Cl"	);
							addSymI( globalSyms, "I"	);
	mpDataCi = (SlColor *)	addSymI( globalSyms, "Ci"	);
	mpDataOi = (SlColor *)	addSymI( globalSyms, "Oi"	);
							addSymI( globalSyms, "E"	);

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

	DASSERT( mPointsN <= MP_GRID_MAX_SIZE );

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
	if ( attribs.moDisplaceSHI.Use() )
	{
		mDispRunCtx.Setup(
						attribs,
						attribs.moDisplaceSHI.Use(),
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
					attribs.moSurfaceSHI.Use(),
					RI_GET_SIMD_BLOCKS( mYDim ),
					mYDim,
					mPointsN );

	mSurfRunCtx.mpShaderInst->Run( mSurfRunCtx );
}

//==================================================================
}
