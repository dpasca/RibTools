//==================================================================
/// RI_SVM_Ops_Lighting.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_LIGHTING_H
#define RI_SVM_OPS_LIGHTING_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{
class Context;

//==================================================================
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Illuminance( Context &ctx )
{
	// only from surface shaders !!
	DASSERT( ctx.GetShader()->mType == Shader::TYPE_SURFACE );

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

	const Float3_*	pPos	= ctx.GetRO( (		Float3_ *)0, 1 );
	const Float3_*	pAxis	;
	const Float_*	pAngle	;

	if ( INCLUDES_AXIS_ANGLE )
	{
		pAxis	= ctx.GetRO( (		  Float3_ *)0, 2 );
		pAngle	= ctx.GetRO( (const Float_ *)0, 3 );
	}
	else
	{
		pAxis	= NULL;
		pAngle	= NULL;
	}

	Float3_	*pL		= (	 Float3_ *)ctx.mpGridSymIList->FindSymbolIData( "L" );

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

	ctx.mFopStack.push( SRC_FuncopStack::FLG_ILLUMINANCE );
}

//==================================================================
template<bool INCLUDES_AXIS_ANGLE>
void Inst_Solar( Context &ctx )
{
	// only from light shaders !!
	DASSERT( ctx.GetShader()->mType == Shader::TYPE_LIGHT );

	DASSTHROW( !ctx.IsInFuncop(), ("Nested funcop ?!") );

	// don't really need the following...
	// u_short funcOpEndAddr = ctx.GetOp(0)->mOpCode.mFuncopEndAddr;

	Float3_	*pL		= (	 Float3_ *)ctx.mpGridSymIList->FindSymbolIData( "L" );

	if ( INCLUDES_AXIS_ANGLE )
	{
		const Float3_*	pAxis	= ctx.GetRO( (		  Float3_ *)0, 1 );
		const Float_*	pAngle	= ctx.GetRO( (const Float_ *)0, 2 );

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
			pL[i] = Float3_( 0.f );
		}
	}

	// yes.. we are in a solar() block 8)
	ctx.mFopStack.push( SRC_FuncopStack::FLG_SOLAR );

	ctx.NextInstruction();
}

//==================================================================
void Inst_FuncopEnd( Context &ctx );

//==================================================================
void Inst_Diffuse( Context &ctx );
void Inst_Ambient( Context &ctx );

//==================================================================
}
//==================================================================
}

#endif
