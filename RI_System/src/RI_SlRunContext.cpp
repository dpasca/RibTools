//==================================================================
/// RI_SlRunContext.cpp
///
/// Created by Davide Pasca - 2009/2/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlRunContext.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlRunContext
//==================================================================
void SlRunContext::InitializeSIMD( MicroPolygonGrid &g )
{
	mSIMDCount = g.GetPointsN();
	mSIMDBlocksN = RI_GET_SIMD_BLOCKS( mSIMDCount );

	mpSIMDFlags = new int [ mSIMDCount ];
	for (u_int i=0; i < mSIMDCount; ++i)
		mpSIMDFlags[i] = 0;
		
	mpSymbols = &g.mSymbols;
}


//==================================================================
}

