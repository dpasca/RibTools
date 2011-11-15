//==================================================================
/// RI_SVM_Ops_Lighting.cpp
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Symbol.h"
#include "RI_State.h"
#include "RI_Attributes.h"
#include "RI_LightSource.h"
#include "RI_SVM_Context.h"
#include "RI_SVM_Ops_Lighting.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
// this is a simplified version.. until lights become available 8)
void Inst_Ambient( Context &ctx, u_int blocksN )
{
	if NOT( ctx.mCache.mAmbientColDone )
	{
		ctx.mCache.mAmbientColDone = true;

		SlColor	ambCol( 0.f );

		SlColor	*pCl = (SlColor *)ctx.mpGridSymIList->FindSymbolIData( "Cl" );

		const DVec<LightSourceT *>	&pLights	= ctx.mpAttribs->mpState->GetLightSources();
		const DVec<U16>				&actLights	= ctx.mpAttribs->mActiveLights;

		// make sure that the subcontextes for the lights are initialized
		ctx.ActLightsCtxs_CheckInit();

		for (size_t i=0; i < actLights.size(); ++i)
		{
			// get the light from the attributes (though attributes takes them from "State")
			const LightSourceT	*pLight = ctx.mpAttribs->GetLight( i );

			if NOT( pLight->mIsAmbient )
				continue;

			Context *pCtx = ctx.GetActLightCtx( i );

			// setup the new context
			pCtx->SetupIfChanged(
						*ctx.mpAttribs,				// same attribs as the current (surface only ?) shader
						pLight->moShaderInst.get(),	// shader instance from the light source
						ctx.mBlocksXN,				// same dimensions as the current shader
						ctx.mPointsYN );			// ...

			// run the light shader !!
			pCtx->mpShaderInst->Run( *pCtx );

			ambCol += *pCl;
		}

		ctx.mCache.mAmbientCol = ambCol;
	}

	SlColor	*lhs	= (SlColor *)ctx.GetRW( 1 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = ctx.mCache.mAmbientCol;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void FuncOpEnd_Illuminance( Context &ctx )
{
	DASSERT( ctx.mIlluminanceCtx.IsActive() );

	if ( ctx.mIlluminanceCtx.mActLightIdx < ctx.mIlluminanceCtx.mActLightsN )
	{
		const LightSourceT	*pLight = NULL;

		// find the next non-ambient light
		for (;
			ctx.mIlluminanceCtx.mActLightIdx < ctx.mIlluminanceCtx.mActLightsN;
			++ctx.mIlluminanceCtx.mActLightIdx )
		{
			// get the light from the attributes (though attributes takes them from "State")
			pLight = ctx.mpAttribs->GetLight( ctx.mIlluminanceCtx.mActLightIdx );

			if ( pLight->mIsAmbient )
				continue;

			// make sure that the subcontextes for the lights are initialized
			ctx.ActLightsCtxs_CheckInit();

			Context *pCtx = ctx.GetActLightCtx( ctx.mIlluminanceCtx.mActLightIdx );

			// setup the new context
			pCtx->SetupIfChanged(
						*ctx.mpAttribs,				// same attribs as the current (surface only ?) shader
						pLight->moShaderInst.get(),	// shader instance from the light source
						ctx.mBlocksXN,				// same dimensions as the current shader
						ctx.mPointsYN );			// ...

			// run the light shader !!
			pCtx->mpShaderInst->Run( *pCtx );

			ctx.mIlluminanceCtx.Increment();
			ctx.GotoInstruction( ctx.mIlluminanceCtx.mBodyStartAddr );
			return;
		}
	}

	ctx.mIlluminanceCtx.Reset();
	ctx.mFopStack.pop();
	ctx.NextInstruction();
}

//==================================================================
void Inst_FuncopEnd( Context &ctx, u_int blocksN )
{
	u_int funcopFlgs =	ctx.mFopStack.top();

	// are ending an if or an else ?
	if ( funcopFlgs & (SRC_FuncopStack::FLG_IFTRUE | SRC_FuncopStack::FLG_ORELSE) )
	{
		// just continue
		ctx.mFopStack.pop();
		ctx.NextInstruction();
	}
	else
	// are we doing illuminance ?
	if ( funcopFlgs & SRC_FuncopStack::FLG_ILLUMINANCE )
	{
		FuncOpEnd_Illuminance( ctx );
	}
	else
	if ( funcopFlgs & SRC_FuncopStack::FLG_SOLAR )
	{
		ctx.mFopStack.pop();
		ctx.NextInstruction();
	}
	else
	{
		ctx.NextInstruction();
		DASSTHROW( 0, ("'funcop' stack broken ?!") );
	}
}

//==================================================================
}
//==================================================================
}
