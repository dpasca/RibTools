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
WorkGrid::WorkGrid( const SymbolList &globalSyms ) :
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
	mpPointsWS = (Float3_ *)	addSymI( globalSyms, "P"	);
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
WorkGrid::~WorkGrid()
{
}

//==================================================================
void *WorkGrid::addSymI( const SymbolList &globalSyms, const char *pName )
{
	const Symbol	*pSrcSym = globalSyms.FindSymbol( pName );

	SymbolI	*pSymI = mSymbolIs.AddInstance( *pSrcSym, MP_GRID_MAX_SIZE );

	return pSymI->GetRWData();
}

//==================================================================
static void fillColArray( SlColor *pCol, size_t n, float r, float g, float b )
{
	size_t	blocksN = DMT_SIMD_BLOCKS( n );

	for (size_t i=0; i < blocksN; ++i)
	{
		pCol[i].Set( r, g, b );
	}
}

//==================================================================
void WorkGrid::Setup(
						u_int xdim,
						u_int ydim,
						const float uRange[2],
						const float vRange[2],
						const Matrix44 &mtxLocalWorld,
						const Matrix44 &mtxWorldCamera )
{
	mXDim = xdim;
	mXBlocks = DMT_SIMD_BLOCKS( xdim );

	// ensure that the width is multiple of the SIMD width
	DASSERT( (mXBlocks * DMT_SIMD_FLEN) == mXDim );

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
void WorkGrid::Displace( const Attributes &attribs )
{
	if ( attribs.moDisplaceSHI.Use() )
	{
		mDispRunCtx.SetupIfChanged(
						attribs,
						attribs.moDisplaceSHI.Use(),
						DMT_SIMD_BLOCKS( mXDim ),
						mYDim,
						mPointsN );

		mDispRunCtx.mpShaderInst->Run( mDispRunCtx );
	}
}

//==================================================================
void WorkGrid::Shade( const Attributes &attribs )
{
	mSurfRunCtx.SetupIfChanged(
					attribs,
					attribs.moSurfaceSHI.Use(),
					DMT_SIMD_BLOCKS( mXDim ),
					mYDim,
					mPointsN );

	mSurfRunCtx.mpShaderInst->Run( mSurfRunCtx );
}

//==================================================================
/// 
//==================================================================
ShadedGrid::ShadedGrid() :
					mPointsN(0),
					mpPointsCS(NULL),
					mpPointsCloseCS(NULL),
					mpCi(NULL),
					mpOi(NULL)
{
}

//==================================================================
ShadedGrid::~ShadedGrid()
{
	DSAFE_DELETE_ARRAY( mpPointsCS );
}

//==================================================================
void ShadedGrid::Init( u_int pointsN )
{
	mPointsN = pointsN;

	size_t	blocksN = DMT_SIMD_BLOCKS( pointsN );

	size_t	offs[5] = { 0 };

	offs[0] =			sizeof(*mpPointsCS		) * blocksN;
	offs[1] = offs[0] + sizeof(*mpPointsCloseCS	) * blocksN;
	offs[2] = offs[1] + sizeof(*mpPosWin		) * blocksN;
	offs[3] = offs[2] + sizeof(*mpCi			) * blocksN;
	offs[4] = offs[3] + sizeof(*mpOi			) * blocksN;

	mpPointsCS		= (Float3_	*)DNEW U8 [ offs[4] ];
	mpPointsCloseCS = (Float3_	*)((U8 *)mpPointsCS + offs[0]);
	mpPosWin		= (Float2_	*)((U8 *)mpPointsCS + offs[1]);
	mpCi			= (SlColor	*)((U8 *)mpPointsCS + offs[2]);
	mpOi			= (SlColor	*)((U8 *)mpPointsCS + offs[3]);
}

//==================================================================
}
