//==================================================================
/// RI_SlShader_Ops_Base.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_BASE_H
#define RI_SLSHADER_OPS_BASE_H

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
template <class TA, class TB, const OpCodeID opCodeID>
void Inst_1Op( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW<TA>( 1 );
	const TB*	op1	= ctx.GetVoidRO<TB>( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	const u_int opCodeMsk = opCodeID & ~OPERANDS_VEC_MSK;

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( opCodeMsk == OP_SS_MOV	) lhs[i] = op1[op1_offset]; else
				if ( opCodeMsk == OP_SS_ABS	) lhs[i] = DAbs( op1[op1_offset] );
					else { DASSERT( 0 ); }
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( opCodeMsk == OP_SS_MOV	) lhs[0] = op1[0]; else
			if ( opCodeMsk == OP_SS_ABS	) lhs[0] = DAbs( op1[0] );
				else { DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, const OpCodeID opCodeID>
void Inst_2Op( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW<TA>( 1 );
	const TA*	op1	= ctx.GetVoidRO<TA>( 2 );
	const TB*	op2	= ctx.GetVoidRO<TB>( 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	const u_int opCodeMsk = opCodeID & ~OPERANDS_VEC_MSK;

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( opCodeMsk == OP_SS_ADD ) lhs[i] = op1[op1_offset] + op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_SUB ) lhs[i] = op1[op1_offset] - op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_MUL ) lhs[i] = op1[op1_offset] * op2[op2_offset]; else
				if ( opCodeMsk == OP_SS_DIV ) lhs[i] = op1[op1_offset] / op2[op2_offset];
					else { DASSERT( 0 ); }
			}
			
			op1_offset	+= op1_step;
			op2_offset	+= op2_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( opCodeMsk == OP_SS_ADD ) lhs[0] = op1[0] + op2[0]; else
			if ( opCodeMsk == OP_SS_SUB ) lhs[0] = op1[0] - op2[0]; else
			if ( opCodeMsk == OP_SS_MUL ) lhs[0] = op1[0] * op2[0]; else
			if ( opCodeMsk == OP_SS_DIV ) lhs[0] = op1[0] / op2[0];
				else { DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}

#endif
