//==================================================================
/// RI_SlShader_Ops_Lighting.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_LIGHTING_H
#define RI_SLSHADER_OPS_LIGHTING_H

//==================================================================
namespace RI
{

class SlRunContext;

//==================================================================
namespace SOP
{

//==================================================================
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Illuminance( SlRunContext &ctx )
{
	// only from surface shaders !!
	DASSERT( ctx.GetShader()->mType == SlShader::TYPE_SURFACE );

	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

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

	SlVec3	*pL		= (	 SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	ctx.NextInstruction();
	u_int bodyStartAddr = ctx.GetCurPC();

	ctx.mIlluminanceCtx.Init(
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
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Solar( SlRunContext &ctx )
{
	// only from light shaders !!
	DASSERT( ctx.GetShader()->mType == SlShader::TYPE_LIGHT );

	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	// don't really need the following...
	// u_short funcOpEndAddr = ctx.GetOp(0)->mOpCode.mFuncopEndAddr;

	SlVec3	*pL		= (	 SlVec3 *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	if ( INCLUDES_AXIS_ANGLE )
	{
		const SlVec3*	pAxis	= ctx.GetVoidRO( (		  SlVec3 *)0, 1 );
		const SlScalar*	pAngle	= ctx.GetVoidRO( (const SlScalar *)0, 2 );

		// set to the value of the axis..
		// NOTE: ignoring the angle for now !

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			// not checking for active processor ?
			pL[i] = -pAxis->GetNormalized();	// why '-' ?! ..just because ?
		}		
	}
	else
	{
		// set light direction as 0,0,0
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			// not checking for active processor ?
			pL[i] = SlVec3( 0.f );
		}
	}

	// yes.. we are in a solar() block 8)
	ctx.mIsInSolar = true;

	ctx.NextInstruction();
}

//==================================================================
void Inst_FuncopEnd( SlRunContext &ctx );

//==================================================================
void Inst_Diffuse( SlRunContext &ctx );
void Inst_Ambient( SlRunContext &ctx );

//==================================================================
}

//==================================================================
}

#endif
