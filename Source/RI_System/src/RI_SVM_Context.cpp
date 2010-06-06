//==================================================================
/// RI_SVM_RunContext.cpp
///
/// Created by Davide Pasca - 2009/2/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Context.h"
#include "RI_MicroPolygonGrid.h"
#include "RI_Attributes.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
/// Context
//==================================================================
Context::Context( SymbolIList &symsIList, size_t maxPointsN ) :
	mMaxPointsN(maxPointsN)
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

	mpSIMDFlags		= DNEW int [ mMaxPointsN ];

	mpGrid			= NULL;

	mFopStack.clear();
}

//==================================================================
Context::~Context()
{
	DSAFE_DELETE_ARRAY( mpSIMDFlags );

	// unbind the last used data segment
	if ( mpDataSegment )
		mpShaderInst->Unbind( mpDataSegment );

	// delete eventual sub-contextes 8)
	FreeActLightsCtxs();
}

//==================================================================
// this is called only once and from the end of the grid constructor
// so to get the pointer of the grid that owns this object
void Context::Init( WorkGrid *pGrid )
{
	// make sure it hasn't been set already !
	DASSERT( mpGrid == NULL );

	mpGrid = pGrid;
}

//==================================================================
void Context::SetupIfChanged(
			const Attributes	&attribs,
			const ShaderInst	*pShaderInst,
			u_int				blocksXN,
			u_int				pointsYN,
			size_t				pointsN )
{
	mBlocksXN	= blocksXN;
	mPointsYN	= pointsYN;

	DASSERT( pointsN <= mMaxPointsN );

	// only do a new bind if the attributes have changed
	if ( &attribs != mpAttribs || mpShaderInst != pShaderInst )
	{
		// unbind the previous data segment
		if ( mpDataSegment )
		{
			mpShaderInst->Unbind( mpDataSegment );
			mpDataSegment = NULL;
		}

		// also delete eventual lights subcontextes.. they'll be built again
		// if necessary during the shading
		FreeActLightsCtxs();

		// reset the cache (where, for example, return value for
		// ambient() is stored)
		mCache.Reset( mpAttribs );

		mpShaderInst	= pShaderInst;
		mpAttribs		= &attribs;

		DASSERT( 0 != mpShaderInst->moShader.Use() );
		mpDataSegment =
			mpShaderInst->Bind(
						mpAttribs->GetGlobalSymList(),
						*mpGridSymIList,
						mDefParamValsStartPCs );
	}
}

//==================================================================
void Context::InitializeSIMD( size_t samplesN )
{
	DASSERT( samplesN <= mMaxPointsN );
	mPointsN = (u_int)samplesN;
	mBlocksN = DMT_SIMD_BLOCKS( mPointsN );

	for (u_int i=0; i < mPointsN; ++i)
		mpSIMDFlags[i] = 0;
}

//==================================================================
void Context::ActLightsCtxs_CheckInit()
{
	// already allocated ? ..then just return
	if ( mpActLightsCtxs.size() )
		return;

	size_t	n = mpAttribs->mActiveLights.size();

	mpActLightsCtxs.reserve( n );

	for (size_t i=0; i < n; ++i)
	{
		Context *pCtx = DNEW Context( mpGrid->mSymbolIs, MP_GRID_MAX_SIZE );

		// add the context to the list of light contextes
		mpActLightsCtxs.push_back( pCtx );

		// init the context (bind to this grid)
		pCtx->Init( mpGrid );
	}
}

//==================================================================
void Context::FreeActLightsCtxs()
{
	for (size_t i=0; i < mpActLightsCtxs.size(); ++i)
	{
		DSAFE_DELETE( mpActLightsCtxs[i] );
	}

	// clear the vector.. important as we check for the vector
	// size to determine if the contextes are allocated or not
	mpActLightsCtxs.clear();
}

//==================================================================
}
//==================================================================
}
