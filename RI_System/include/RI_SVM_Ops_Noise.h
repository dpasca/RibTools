//==================================================================
/// RI_SVM_Ops_Noise.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_NOISE_H
#define RI_SVM_OPS_NOISE_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
template <class TB>
inline void Inst_Noise1( Context &ctx )
{
		  Float_*	lhs	= ctx.GetRW( (Float_ *)0, 1 );
	const TB*		op1	= ctx.GetRO( (const TB *)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = Noise::unoise1( op1[op1_offset] );
			}

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = Noise::unoise1( op1[0] );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB>
inline void Inst_Noise3( Context &ctx )
{
		  Float3_*	lhs	= ctx.GetRW( (	  Float3_*)0, 1 );
	const TB	*	op1	= ctx.GetRO( (const TB	*)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	
	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = Noise::unoise3( op1[op1_offset] );
			}

			op1_offset += op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = Noise::unoise3( op1[0] );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}

#endif
