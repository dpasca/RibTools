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
#include "RI_Attributes.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlRunContext
//==================================================================
SlRunContext::SlRunContext( const SlSymbolList &symbols, size_t maxPointsN )
{
	mProgramCounter = 0;
	mSIMDCount		= 0;
	mSIMDBlocksN	= 0;
	mpDataSegment	= 0;
	mpShaderInst	= 0;
	mpSymbols		= &symbols;
	mpAttribs		= 0;

	mMaxPointsN		= maxPointsN;
	mpSIMDFlags		= DNEW int [ mMaxPointsN ];
}

//==================================================================
SlRunContext::~SlRunContext()
{
	DSAFE_DELETE_ARRAY( mpSIMDFlags );

	// unbind the last used data segment
	if ( mpDataSegment )
		mpAttribs->mShaderInstance.Unbind( mpDataSegment );
}

//==================================================================
void SlRunContext::Setup( const Attributes &attribs, size_t pointsN )
{
	// only do a new bind if the attributes have changed
	if ( &attribs != mpAttribs )
	{
		mpShaderInst	= &attribs.mShaderInstance;
		mpAttribs		= &attribs;

		// unbind the previous data segment
		if ( mpDataSegment )
			mpAttribs->mShaderInstance.Unbind( mpDataSegment );

		mpDataSegment	= attribs.mShaderInstance.Bind( *mpSymbols );
	}

	// reset the program counter
	mProgramCounter = 0;
	// initialize the SIMD state
	InitializeSIMD( pointsN );
}

//==================================================================
void SlRunContext::InitializeSIMD( size_t samplesN )
{
	DASSERT( samplesN <= mMaxPointsN );
	mSIMDCount = samplesN;
	mSIMDBlocksN = RI_GET_SIMD_BLOCKS( mSIMDCount );

	for (u_int i=0; i < mSIMDCount; ++i)
		mpSIMDFlags[i] = 0;
}

//==================================================================
}

