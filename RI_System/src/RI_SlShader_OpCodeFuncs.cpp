//==================================================================
/// RI_SlShader_OpCodeFuncs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Noise.h"
#include "RI_SlShader_Ops_Base.h"
#include "RI_SlShader_Ops_Lighting.h"
#include "RI_SlShader_Ops_Noise.h"
#include "RI_SlShader_Ops_XForm.h"
#include "RI_SlShader_Ops_Misc.h"

#include "RI_SlShader_OpCodeFuncs.h"

//==================================================================
namespace RI
{

//==================================================================
//==================================================================
void Inst_SolarBegin_VS( SlRunContext &ctx )
{
	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	const SlVec3	*pAxis	= ctx.GetVoidRO( (		  SlVec3 *)0, 1 );
	const SlScalar	*pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 2 );

	SlVec3		*pL		= (SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	// 'L' becomes the normalized axis
	*pL = pAxis->GetNormalized();

	// TODO: check with angle to see if we need to skip this
	// start looping by the funcop end
	if ( 0 )
	{
		// if we are skipping then Cl = 0 (do we really need it ?)

		SlColor	*pCl = (SlColor *)ctx.mpGridSymIList->FindSymbolIData( "Cl" );
		*pCl = SlColor( 0 );

		ctx.GotoInstruction( ctx.GetOp(0)->mOpCode.mFuncopEndAddr );
	}

	ctx.NextInstruction();
}

//==================================================================
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Illuminance( SlRunContext &ctx )
{
	u_short funcOpEndAddr = ctx.GetOp(0)->mOpCode.mFuncopEndAddr;

	// any lights ?
	if NOT( ctx.mpAttribs->mActiveLights.size() )
	{
		// no ? Skip the whole thing
		ctx.GotoInstruction( funcOpEndAddr );
		ctx.NextInstruction();
		return;
	}

	const SlVec3*	pPos	= ctx.GetVoidRO( (		SlVec3 *)0, 1 );
	const SlVec3*	pAxis	;
	const SlScalar*	pAngle	;

	if ( INCLUDES_AXIS_ANGLE )
	{
		pAxis	= ctx.GetVoidRO( (		  SlVec3 *)0, 2 );
		pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 3 );
	}
	else
	{
		pAxis	= NULL;
		pAngle	= NULL;
	}

	SlColor	*pCl	= (	SlColor	*)ctx.mpGridSymIList->FindSymbolIData( "Cl" );
	SlVec3	*pL		= (	 SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	ctx.NextInstruction();
	u_int bodyStartAddr = ctx.GetCurPC();

	ctx.mSlIlluminanceCtx.Init(
			bodyStartAddr,
			pAxis,
			pAngle,
			pL,
			ctx.mpAttribs->mActiveLights.size()
			);

	// start looping by the funcop end
	ctx.GotoInstruction( funcOpEndAddr );
}

//==================================================================
void Inst_FuncopEnd( SlRunContext &ctx )
{
	// are we doing illuminance ?
	if ( ctx.mSlIlluminanceCtx.IsActive() )
	{
		if ( ctx.mSlIlluminanceCtx.mActLightIdx < ctx.mSlIlluminanceCtx.mActLightsN )
		{
			const LightSourceT	*pLight = NULL;

			// find the next non-ambient light
			for (;
				ctx.mSlIlluminanceCtx.mActLightIdx < ctx.mSlIlluminanceCtx.mActLightsN;
				++ctx.mSlIlluminanceCtx.mActLightIdx )
			{
				// get the light from the attributes (though attributes takes them from "State")
				pLight = ctx.mpAttribs->GetLight( ctx.mSlIlluminanceCtx.mActLightIdx );

				if ( pLight->mIsAmbient )
					continue;

				// make sure that the subcontextes for the lights are initialized
				ctx.ActLightsCtxs_CheckInit();

				SlRunContext *pCtx = ctx.GetActLightCtx( ctx.mSlIlluminanceCtx.mActLightIdx );

				// setup the new context
				pCtx->SetupIfChanged(
							*ctx.mpAttribs,				// same attribs as the current (surface only ?) shader
							pLight->moShaderInst.Use(),	// shader instance from the light source
							ctx.mBlocksXN,				// same dimensions as the current shader
							ctx.mPointsYN );			// ...

				// run the light shader !!
				pCtx->mpShaderInst->Run( *pCtx );

				ctx.mSlIlluminanceCtx.Increment();
				ctx.GotoInstruction( ctx.mSlIlluminanceCtx.mBodyStartAddr );
				return;
			}
		}

		ctx.mSlIlluminanceCtx.Reset();
		ctx.NextInstruction();
	}
	else
	{
		DASSTHROW( 0, ("'funcop' not matching !") );
	}
}

//==================================================================
//==================================================================
#define S SlScalar
#define V SlVec3
//#define MATRIX	Matrix44

//==================================================================
SlOpCodeFunc	_gSlOpCodeFuncs[] =
{
	NULL,	// ret

	SOP::Inst_1Op<S,S,OBT_MOV>,
	SOP::Inst_1Op<V,S,OBT_MOV>,
	SOP::Inst_1Op<V,V,OBT_MOV>,

	SOP::Inst_MovXX,

	SOP::Inst_1Op<S,S,OBT_ABS>,
	SOP::Inst_1Op<V,S,OBT_ABS>,
	SOP::Inst_1Op<V,V,OBT_ABS>,

	SOP::Inst_Sign<S,S>,

	SOP::Inst_2Op<S,S,S,OBT_ADD>,
	SOP::Inst_2Op<V,V,S,OBT_ADD>,
	SOP::Inst_2Op<V,S,V,OBT_ADD>,
	SOP::Inst_2Op<V,V,V,OBT_ADD>,

	SOP::Inst_2Op<S,S,S,OBT_SUB>,
	SOP::Inst_2Op<V,V,S,OBT_SUB>,
	SOP::Inst_2Op<V,S,V,OBT_SUB>,
	SOP::Inst_2Op<V,V,V,OBT_SUB>,

	SOP::Inst_2Op<S,S,S,OBT_MUL>,
	SOP::Inst_2Op<V,V,S,OBT_MUL>,
	SOP::Inst_2Op<V,S,V,OBT_MUL>,
	SOP::Inst_2Op<V,V,V,OBT_MUL>,

	SOP::Inst_2Op<S,S,S,OBT_DIV>,
	SOP::Inst_2Op<V,S,V,OBT_DIV>,
	SOP::Inst_2Op<V,V,S,OBT_DIV>,
	SOP::Inst_2Op<V,V,V,OBT_DIV>,

	SOP::Inst_MOVVS3<SlVec3,SlScalar>,

	SOP::Inst_Dot_SVV,

	SOP::Inst_Min_Max<S,OBT_MIN>,
	SOP::Inst_Min_Max<V,OBT_MIN>,

	SOP::Inst_Min_Max<S,OBT_MAX>,
	SOP::Inst_Min_Max<V,OBT_MAX>,

	SOP::Inst_LD1<S>,
	SOP::Inst_LD3<V>,

	SOP::Inst_CMPLT<S>,	// cmplt

	SOP::Inst_Noise1<SlScalar>,
	SOP::Inst_Noise1<SlVec2>,
	SOP::Inst_Noise1<SlVec3>,

	SOP::Inst_Noise3<SlScalar>,
	SOP::Inst_Noise3<SlVec2>,
	SOP::Inst_Noise3<SlVec3>,

	SOP::Inst_GetVComp<0>,
	SOP::Inst_GetVComp<1>,
	SOP::Inst_GetVComp<2>,
	SOP::Inst_SetVComp<0>,
	SOP::Inst_SetVComp<1>,
	SOP::Inst_SetVComp<2>,

	SOP::Inst_PXFormname_VXV,
	SOP::Inst_VXFormname_VXV,
	SOP::Inst_NXFormname_VXV,
	SOP::Inst_CXFormname_VXV,

	SOP::Inst_Normalize,
	SOP::Inst_Faceforward,
	SOP::Inst_Diffuse,
	SOP::Inst_Ambient,
	SOP::Inst_CalculateNormal,

	Inst_SolarBegin_VS,
	Inst_Illuminance<true>,
	Inst_Illuminance<false>,
	Inst_FuncopEnd,
};

#undef S
#undef V

//==================================================================
}
