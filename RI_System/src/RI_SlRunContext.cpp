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
SlRunContext::SlRunContext( SymbolIList &symsIList, size_t maxPointsN )
{
	mBlocksXN;
	mPointsYN;

	mProgramCounterIdx = 0;
	mPointsN		= 0;
	mBlocksN		= 0;
	mpDataSegment	= 0;
	mpShaderInst	= 0;
	mpGridSymIList	= &symsIList;
	mpAttribs		= 0;

	mMaxPointsN		= maxPointsN;
	mpSIMDFlags		= DNEW int [ mMaxPointsN ];

	mpGrid			= NULL;
}

//==================================================================
SlRunContext::~SlRunContext()
{
	DSAFE_DELETE_ARRAY( mpSIMDFlags );

	// unbind the last used data segment
	if ( mpDataSegment )
		mpShaderInst->Unbind( mpDataSegment );
}

//==================================================================
// this is called only once and from the end of the grid constructor
// so to get the pointer of the grid that owns this object
void SlRunContext::Init( MicroPolygonGrid *pGrid )
{
	// make sure it hasn't been set already !
	DASSERT( mpGrid == NULL );

	mpGrid = pGrid;
}

//==================================================================
void SlRunContext::Setup(
			const Attributes		&attribs,
			const SlShaderInstance	*pShaderInst,
			u_int					blocksXN,
			u_int					pointsYN,
			size_t					pointsN )
{
	mBlocksXN	= blocksXN;
	mPointsYN	= pointsYN;
	mMaxPointsN	= pointsN;

	// only do a new bind if the attributes have changed
	if ( &attribs != mpAttribs || mpShaderInst != pShaderInst )
	{
		// unbind the previous data segment
		if ( mpDataSegment )
		{
			mpShaderInst->Unbind( mpDataSegment );
			mpDataSegment = NULL;
		}

		mpShaderInst	= pShaderInst;
		mpAttribs		= &attribs;

		DASSERT( 0 != mpShaderInst->mpShader );
		mpDataSegment =
			mpShaderInst->Bind(
						*mpGridSymIList,
						mDefParamValsStartPCs );
	}

	// reset the program counter
	mProgramCounterIdx = 0;
	mProgramCounter[ mProgramCounterIdx ] = INVALID_PC;

	// initialize the SIMD state
	InitializeSIMD( mMaxPointsN );

	// initialize the non uniform/constant values with eventual default data
	for (size_t i=0; i < mpShaderInst->mpShader->mpShaSyms.size(); ++i)
	{
		SlValue	&slValue = mpDataSegment[i];

		if ( slValue.Flags.mOwnData &&
			 slValue.mpSrcSymbol->mpDefaultVal != NULL )
		{
			DASSERT( slValue.Data.pVoidValue != NULL );

			size_t samplesN =
				slValue.mpSrcSymbol->IsVarying() ? mMaxPointsN : 1;

			slValue.mpSrcSymbol->FillDataWithDefault(
							slValue.Data.pVoidValue, samplesN );
		}
	}
}

//==================================================================
void SlRunContext::InitializeSIMD( size_t samplesN )
{
	DASSERT( samplesN <= mMaxPointsN );
	mPointsN = samplesN;
	mBlocksN = RI_GET_SIMD_BLOCKS( mPointsN );

	for (u_int i=0; i < mPointsN; ++i)
		mpSIMDFlags[i] = 0;
}

//==================================================================
}

