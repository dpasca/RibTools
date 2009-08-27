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
#include "RI_Symbol.h"
#include "RI_SlRunContext.h"

//==================================================================
namespace RI
{

//==================================================================
class Attributes;
class SlRunContext;

/*
//==================================================================
template <class _T, size_t _MAX>
class DumbPool
{
	_T		mStorage[_MAX];
	size_t	mStorageN;

public:
	DumbPool() :
		mStorageN(0)
	{
	}

	~DumbPool()
	{
	}

	_T	*Alloc()
	{
		DASSERT( mStorageN < _MAX );
		return &mStorage[ mStorageN++ ];
	}
};
*/

//==================================================================
class MicroPolygonGrid
{
	SlColor			*mpDataCi;
	SlColor			*mpDataOi;
	SlColor			*mpDataCs;
	SlColor			*mpDataOs;
	SlRunContext	mSurfRunCtx;
	SlRunContext	mDispRunCtx;

public:
	static const u_int	MAX_SIZE = RI_GET_SIMD_PAD_SUBS( 48 ) * 48;
	static const u_int	MAX_SIMD_BLKS = RI_GET_SIMD_BLOCKS( MAX_SIZE );

	u_int			mXDim;
	u_int			mXBlocks;
	u_int			mYDim;
	u_int			mPointsN;
	SlVec3			*mpPointsWS;
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
	
	void Displace( const Attributes &attribs );
	void Shade( const Attributes &attribs );
};

//==================================================================
}

#endif
