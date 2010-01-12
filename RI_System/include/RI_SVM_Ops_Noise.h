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
inline void Inst_Noise1( Context &ctx, u_int blocksN )
{
		  Float_*	lhs	= (		 Float_*)ctx.GetRW( 1 );
	const TB	*	op1	= (const TB	   *)ctx.GetRO( 2 );

	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op1_idx = 0;

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = Noise::unoise1( op1[op1_idx] );
		}

		op1_idx += op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB>
inline void Inst_Noise3( Context &ctx, u_int blocksN )
{
		  Float3_*	lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const TB	 *	op1	= (const TB		*)ctx.GetRO( 2 );

	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op1_idx = 0;

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = Noise::unoise3( op1[op1_idx] );
		}

		op1_idx += op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}

#endif
