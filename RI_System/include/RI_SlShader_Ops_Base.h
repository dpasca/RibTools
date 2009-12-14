//==================================================================
/// RI_SlShader_Ops_Base.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_BASE_H
#define RI_SLSHADER_OPS_BASE_H

#include "RI_SlRunContext.h"

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
template <class TA>
void Inst_CMPLT( SlRunContext &ctx )
{
	const TA	*lhs	= ctx.GetVoidRO( (const TA *)0, 1 );
	const TA	*rhs	= ctx.GetVoidRO( (const TA *)0, 2 );

	DASSERT( !ctx.IsSymbolVarying( 1 ) );
	DASSERT( !ctx.IsSymbolVarying( 2 ) );

	// emm.. not truly uniform for now !!
	if ( lhs[0][0] < rhs[0][0] )
	{
		u_int	addr = ctx.GetOp( 3 )->mAddress.mOffset;

		ctx.GotoInstruction( addr );
	}
	else
		ctx.NextInstruction();
}

//==================================================================
template <class TA>
void Inst_LD1( SlRunContext &ctx )
{
	TA		*lhs		= ctx.GetVoidRW( (TA *)0,  1 );
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	TA		tmp = TA( ctx.GetImmFloat( 2 ) );

	if ( lhs_varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i] = tmp;
			}
		}
	}
	else
	{
		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = tmp;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA>
void Inst_LD3( SlRunContext &ctx )
{
	TA		*lhs		= ctx.GetVoidRW( (TA *)0,  1 );
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	TA		tmp = TA( ctx.GetImmFloat( 2 ), ctx.GetImmFloat( 3 ), ctx.GetImmFloat( 4 ) );

	if ( lhs_varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i] = tmp;
			}
		}
	}
	else
	{
		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = tmp;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_MOVVS3( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW( (		TA *)0, 1 );
	const TB*	op1	= ctx.GetVoidRO( (const TB *)0, 2 );
	const TB*	op2	= ctx.GetVoidRO( (const TB *)0, 3 );
	const TB*	op3	= ctx.GetVoidRO( (const TB *)0, 4 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op3_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );
		int		op3_step = ctx.GetSymbolVaryingStep( 4 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i][0] = op1[op1_offset];
				lhs[i][1] = op2[op2_offset];
				lhs[i][2] = op3[op3_offset];
			}

			op1_offset	+= op1_step;
			op2_offset	+= op2_step;
			op3_offset	+= op3_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) &&
				 !ctx.IsSymbolVarying( 4 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0][0] = op1[0];
			lhs[0][1] = op2[0];
			lhs[0][2] = op3[0];
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, const OpBaseTypeID opBaseTypeID>
void Inst_1Op( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW( (		TA *)0, 1 );
	const TB*	op1	= ctx.GetVoidRO( (const TB *)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				if ( opBaseTypeID == OBT_MOV	) lhs[i] = op1[op1_offset]; else
				if ( opBaseTypeID == OBT_ABS	) lhs[i] = DAbs( op1[op1_offset] );
					else { DASSERT( 0 ); }
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			if ( opBaseTypeID == OBT_MOV	) lhs[0] = op1[0]; else
			if ( opBaseTypeID == OBT_ABS	) lhs[0] = DAbs( op1[0] );
				else { DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
static inline void Inst_MovXX( SlRunContext &ctx )
{
		  SlStr*	lhs	= ctx.GetVoidRW( (		SlStr *)0, 1 );
	const SlStr*	op1	= ctx.GetVoidRO( (const SlStr *)0, 2 );

	DASSERT(
		ctx.IsSymbolVarying( 1 ) == false &&
		ctx.IsSymbolVarying( 2 ) == false );

	strcpy_s( lhs[0].mStr, op1[0].mStr );

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Sign( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW( (		TA *)0, 1 );
	const TB*	op1	= ctx.GetVoidRO( (const TB *)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i] = DSign( op1[op1_offset] );
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = DSign( op1[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, class TC, const OpBaseTypeID opBaseTypeID>
void Inst_2Op( SlRunContext &ctx )
{
		  TA*	lhs	= ctx.GetVoidRW( (		TA *)0, 1 );
	const TB*	op1	= ctx.GetVoidRO( (const TB *)0, 2 );
	const TC*	op2	= ctx.GetVoidRO( (const TC *)0, 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

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
				if ( opBaseTypeID == OBT_ADD ) lhs[i] = op1[op1_offset] + op2[op2_offset]; else
				if ( opBaseTypeID == OBT_SUB ) lhs[i] = op1[op1_offset] - op2[op2_offset]; else
				if ( opBaseTypeID == OBT_MUL ) lhs[i] = op1[op1_offset] * op2[op2_offset]; else
				if ( opBaseTypeID == OBT_DIV ) lhs[i] = op1[op1_offset] / op2[op2_offset]; else
				{ DASSERT( 0 ); }
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
			if ( opBaseTypeID == OBT_ADD ) lhs[0] = op1[0] + op2[0]; else
			if ( opBaseTypeID == OBT_SUB ) lhs[0] = op1[0] - op2[0]; else
			if ( opBaseTypeID == OBT_MUL ) lhs[0] = op1[0] * op2[0]; else
			if ( opBaseTypeID == OBT_DIV ) lhs[0] = op1[0] / op2[0]; else
			{ DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Dot_SVV( SlRunContext &ctx )
{
		  SlScalar*	lhs	= ctx.GetVoidRW( (	  SlScalar *)0, 1 );
	const SlVec3*	op1	= ctx.GetVoidRO( (const SlVec3 *)0, 2 );
	const SlVec3*	op2	= ctx.GetVoidRO( (const SlVec3 *)0, 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

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
				lhs[i] = op1[op1_offset].GetDot( op2[op2_offset] );
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
			lhs[0] = op1[0].GetDot( op2[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Pow_SSS( SlRunContext &ctx )
{
		  SlScalar*	lhs	= ctx.GetVoidRW( (		SlScalar *)0, 1 );
	const SlScalar*	op1	= ctx.GetVoidRO( (const SlScalar *)0, 2 );
	const SlScalar*	op2	= ctx.GetVoidRO( (const SlScalar *)0, 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

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
				lhs[i] = DPow( op1[op1_offset], op2[op2_offset] );
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
			lhs[0] = DPow( op1[0], op2[0] );
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class T, const OpBaseTypeID opBaseTypeID>
void Inst_Min_Max( SlRunContext &ctx )
{
		  T*	lhs	= ctx.GetVoidRW( (	    T *)0, 1 );
	const T*	op1	= ctx.GetVoidRO( (const T *)0, 2 );
	const T*	op2	= ctx.GetVoidRO( (const T *)0, 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

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
				if ( opBaseTypeID == OBT_MIN ) lhs[i] = DMin( op1[op1_offset], op2[op2_offset] ); else
				if ( opBaseTypeID == OBT_MAX ) lhs[i] = DMax( op1[op1_offset], op2[op2_offset] ); else
				{ DASSERT( 0 ); }
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
			if ( opBaseTypeID == OBT_MIN ) lhs[0] = DMin( op1[0], op2[0] ); else
			if ( opBaseTypeID == OBT_MAX ) lhs[0] = DMax( op1[0], op2[0] ); else
			{ DASSERT( 0 ); }
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_GetVComp( SlRunContext &ctx )
{
		  SlScalar*	lhs	= ctx.GetVoidRW( (		SlScalar*)0, 1 );
	const SlVec3*	op1	= ctx.GetVoidRO( (const	SlVec3	*)0	,2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i] = op1[op1_offset][COMP_IDX];
			}
			
			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0] = op1[0][COMP_IDX];
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_SetVComp( SlRunContext &ctx )
{
		  SlVec3*	lhs	= ctx.GetVoidRW( (		SlVec3	*)0, 1 );
	const SlScalar*	op1	= ctx.GetVoidRO( (const SlScalar*)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			if ( ctx.IsProcessorActive( i ) )
			{
				lhs[i][COMP_IDX] = op1[op1_offset];
			}
			
			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		if ( ctx.IsProcessorActive( 0 ) )
		{
			lhs[0][COMP_IDX] = op1[0];
		}
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}

#endif
