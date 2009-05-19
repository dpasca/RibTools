//==================================================================
/// RI_SlShader_Ops_Noise.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_NOISE_H
#define RI_SLSHADER_OPS_NOISE_H

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
template <class TB>
inline void Inst_Noise1( SlRunContext &ctx )
{
		  SlScalar*	lhs	= ctx.GetVoidRW<SlScalar>( 1 );
	const TB*		op1	= ctx.GetVoidRO<TB>( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = Noise::unoise1( op1[op1_offset] );

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = Noise::unoise1( op1[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB>
inline void Inst_Noise3( SlRunContext &ctx )
{
		  SlVec3*	lhs	= ctx.GetVoidRW<SlVec3>( 1 );
	const TB*		op1	= ctx.GetVoidRO<TB>( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
				lhs[i] = Noise::unoise3( op1[op1_offset] );

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = Noise::unoise3( op1[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}

#endif
