//==================================================================
/// RI_SVM_Ops_Base.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_BASE_H
#define RI_SVM_OPS_BASE_H

#include "RI_SVM_Context.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
template <class TA>
void Inst_LD1( Context &ctx, u_int blocksN )
{
	TA		*lhs	= (TA *)ctx.GetRW( 1 );
	TA		tmp		= TA( ctx.GetImmFloat( 2 ) );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = tmp;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA>
void Inst_LD3( Context &ctx, u_int blocksN )
{
	TA		*lhs	= (TA *)ctx.GetRW( 1 );
	TA		tmp		= TA( ctx.GetImmFloat( 2 ), ctx.GetImmFloat( 3 ), ctx.GetImmFloat( 4 ) );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = tmp;
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_MOVVS3( Context &ctx, u_int blocksN )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TB*	op2	= (const TB*)ctx.GetRO( 3 );
	const TB*	op3	= (const TB*)ctx.GetRO( 4 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op3_offset = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );
	int		op3_step = ctx.GetSymbolVaryingStep( 4 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i][0] = op1[op1_idx];
			lhs[i][1] = op2[op2_idx];
			lhs[i][2] = op3[op3_offset];
		}

		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
		op3_offset	+= op3_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Mov( Context &ctx, u_int blocksN )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
		lhs[i] = op1[op1_idx];
		}

		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Abs( Context &ctx, u_int blocksN )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
		lhs[i] = DAbs( op1[op1_idx] );
		}

		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Sign( Context &ctx, u_int blocksN )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = DSign( op1[op1_idx] );
		}

		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, class TC, const OpBaseTypeID opBaseTypeID>
void Inst_2Op( Context &ctx, u_int blocksN )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TC*	op2	= (const TC*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			if ( opBaseTypeID == OBT_ADD ) lhs[i] = op1[op1_idx] + op2[op2_idx]; else
			if ( opBaseTypeID == OBT_SUB ) lhs[i] = op1[op1_idx] - op2[op2_idx]; else
			if ( opBaseTypeID == OBT_MUL ) lhs[i] = op1[op1_idx] * op2[op2_idx]; else
			if ( opBaseTypeID == OBT_DIV ) lhs[i] = op1[op1_idx] / op2[op2_idx]; else
			{ DASSERT( 0 ); }
		}

		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Dot_SVV( Context &ctx, u_int blocksN )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );
	const Float3_*	op2	= (const Float3_*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = op1[op1_idx].GetDot( op2[op2_idx] );
		}
		
		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Length_SV( Context &ctx, u_int blocksN )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
		lhs[i] = op1[op1_idx].GetLength();
		}
		
		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Pow_SSS( Context &ctx, u_int blocksN )
{
		  Float_*	lhs	= (		 Float_*)ctx.GetRW( 1 );
	const Float_*	op1	= (const Float_*)ctx.GetRO( 2 );
	const Float_*	op2	= (const Float_*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = DPow( op1[op1_idx], op2[op2_idx] );
		}
		
		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class T, const OpBaseTypeID opBaseTypeID>
void Inst_Min_Max( Context &ctx, u_int blocksN )
{
		  T*	lhs	=  (	  T*)ctx.GetRW( 1 );
	const T*	op1	=  (const T*)ctx.GetRO( 2 );
	const T*	op2	=  (const T*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			if ( opBaseTypeID == OBT_MIN ) lhs[i] = DMin( op1[op1_idx], op2[op2_idx] ); else
			if ( opBaseTypeID == OBT_MAX ) lhs[i] = DMax( op1[op1_idx], op2[op2_idx] ); else
			{ DASSERT( 0 ); }
		}
		
		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_GetVComp( Context &ctx, u_int blocksN )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = op1[op1_idx][COMP_IDX];
		}
		
		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_SetVComp( Context &ctx, u_int blocksN )
{
		  Float3_*	lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const Float_ *	op1	= (const Float_ *)ctx.GetRO( 2 );

	int		op1_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i][COMP_IDX] = op1[op1_idx];
		}
		
		op1_idx	+= op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}

#endif
