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
void Inst_LD1( Context &ctx )
{
	TA		*lhs		= (TA *)ctx.GetRW( 1 );
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	TA		tmp = TA( ctx.GetImmFloat( 2 ) );

	if ( lhs_varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = tmp;
			}
		}
	}
	else
	{
		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
				lhs[0] = tmp;
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA>
void Inst_LD3( Context &ctx )
{
	TA		*lhs		= (TA *)ctx.GetRW( 1 );
	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	TA		tmp = TA( ctx.GetImmFloat( 2 ), ctx.GetImmFloat( 3 ), ctx.GetImmFloat( 4 ) );

	if ( lhs_varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = tmp;
			}
		}
	}
	else
	{
		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
				lhs[0] = tmp;
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_MOVVS3( Context &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TB*	op2	= (const TB*)ctx.GetRO( 3 );
	const TB*	op3	= (const TB*)ctx.GetRO( 4 );

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
			SLRUNCTX_BLKWRITECHECK( i );
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

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0][0] = op1[0];
			lhs[0][1] = op2[0];
			lhs[0][2] = op3[0];
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Mov( Context &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
			lhs[i] = op1[op1_offset];
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = op1[0];
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB>
void Inst_Abs( Context &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
			lhs[i] = DAbs( op1[op1_offset] );
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = DAbs( op1[0] );
			}
		}
	}

	ctx.NextInstruction();
}

/*
//==================================================================
static inline void Inst_MovXX( Context &ctx )
{
		  SlStr*	lhs	= ctx.GetRW( (		SlStr *)0, 1 );
	const SlStr*	op1	= ctx.GetRO( (const SlStr *)0, 2 );

	DASSERT(
		ctx.IsSymbolVarying( 1 ) == false &&
		ctx.IsSymbolVarying( 2 ) == false );

	strcpy_s( lhs[0].mStr, op1[0].mStr );

	ctx.NextInstruction();
}
*/

//==================================================================
template <class TA, class TB>
void Inst_Sign( Context &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = DSign( op1[op1_offset] );
			}

			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = DSign( op1[0] );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TA, class TB, class TC, const OpBaseTypeID opBaseTypeID>
void Inst_2Op( Context &ctx )
{
		  TA*	lhs	= (		 TA*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TC*	op2	= (const TC*)ctx.GetRO( 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	u_int	blocksN = ctx.IsSymbolVarying( 1 ) ? ctx.mBlocksN : 1;

	int		op1_offset = 0;
	int		op2_offset = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
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

	ctx.NextInstruction();
}

//==================================================================
void Inst_Dot_SVV( Context &ctx )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );
	const Float3_*	op2	= (const Float3_*)ctx.GetRO( 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
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

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
				lhs[0] = op1[0].GetDot( op2[0] );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Length_SV( Context &ctx )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
			lhs[i] = op1[op1_offset].GetLength();
			}
			
			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = op1[0].GetLength();
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Pow_SSS( Context &ctx )
{
		  Float_*	lhs	= (		 Float_*)ctx.GetRW( 1 );
	const Float_*	op1	= (const Float_*)ctx.GetRO( 2 );
	const Float_*	op2	= (const Float_*)ctx.GetRO( 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
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

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
				lhs[0] = DPow( op1[0], op2[0] );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class T, const OpBaseTypeID opBaseTypeID>
void Inst_Min_Max( Context &ctx )
{
		  T*	lhs	=  (	  T*)ctx.GetRW( 1 );
	const T*	op1	=  (const T*)ctx.GetRO( 2 );
	const T*	op2	=  (const T*)ctx.GetRO( 3 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op2_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op2_step = ctx.GetSymbolVaryingStep( 3 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
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

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			if ( opBaseTypeID == OBT_MIN ) lhs[0] = DMin( op1[0], op2[0] ); else
			if ( opBaseTypeID == OBT_MAX ) lhs[0] = DMax( op1[0], op2[0] ); else
			{ DASSERT( 0 );		}
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_GetVComp( Context &ctx )
{
		  Float_ *	lhs	= (		 Float_ *)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = op1[op1_offset][COMP_IDX];
			}
			
			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0] = op1[0][COMP_IDX];
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <const size_t COMP_IDX>
void Inst_SetVComp( Context &ctx )
{
		  Float3_*	lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const Float_ *	op1	= (const Float_ *)ctx.GetRO( 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_offset = 0;
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i][COMP_IDX] = op1[op1_offset];
			}
			
			op1_offset	+= op1_step;
		}
	}
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 2 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			lhs[0][COMP_IDX] = op1[0];
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
