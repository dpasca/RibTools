//==================================================================
/// RI_SVM_Ops_Compare.h
///
/// Created by Davide Pasca - 2010/1/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_COMPARE_H
#define RI_SVM_OPS_COMPARE_H

#include "RI_SVM_Context.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
template <class TA>
void Inst_CMPLT( Context &ctx, u_int blocksN )
{
	const TA	*lhs = (const TA*)ctx.GetRO( 1 );
	const TA	*rhs = (const TA*)ctx.GetRO( 2 );

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
template <class _T>
inline _T GetBroadcast0Lane( const _T &val )
{
	return _T( val[0] );
}
//==================================================================
template <>
inline VecNMask GetBroadcast0Lane( const VecNMask &val )
{
	VecNMask	tmp = val;
	VecNMask_Broadcast0Lane( tmp );
	return tmp;
}

//==================================================================
template <class TB, const OpBaseTypeID opBaseTypeID>
void Inst_SETCMP_EQ( Context &ctx, u_int blocksN )
{
	  VecNMask*	lhs	= (VecNMask*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TB*	op2	= (const TB*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			if ( opBaseTypeID == OBT_SETEQ	 ) lhs[i] = CmpMaskEQ(op1[op1_idx], op2[op2_idx]); else
			if ( opBaseTypeID == OBT_SETNEQ	 ) lhs[i] = CmpMaskNE(op1[op1_idx], op2[op2_idx]); else
			{ DASSERT( 0 ); }
		}
		
		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB, const OpBaseTypeID opBaseTypeID>
void Inst_SETCMP_EQ_NoVary( Context &ctx, u_int blocksN )
{
	  VecNMask*	lhs	= (VecNMask*)ctx.GetRW( 1 );
	const TB*	op1	= (const TB*)ctx.GetRO( 2 );
	const TB*	op2	= (const TB*)ctx.GetRO( 3 );

		DASSERT( !ctx.IsSymbolVarying( 1 ) &&
				 !ctx.IsSymbolVarying( 2 ) &&
				 !ctx.IsSymbolVarying( 3 ) );

	for (u_int i=0; i < 1; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( 0 );
		{
		if ( opBaseTypeID == OBT_SETEQ	 ) lhs[0] = (op1[0] == op2[0] ? VecNMaskFull : VecNMaskEmpty); else
		if ( opBaseTypeID == OBT_SETNEQ	 ) lhs[0] = (op1[0] != op2[0] ? VecNMaskFull : VecNMaskEmpty); else
		{ DASSERT( 0 );		}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB, const OpBaseTypeID opBaseTypeID>
void Inst_SETCMP_REL( Context &ctx, u_int blocksN )
{
	  VecNMask*	lhs	=  (VecNMask*)ctx.GetRW( 1 );
	const TB*	op1	=  (const TB*)ctx.GetRO( 2 );
	const TB*	op2	=  (const TB*)ctx.GetRO( 3 );

	int		op1_idx = 0;
	int		op2_idx = 0;
	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op2_step = ctx.GetSymbolVaryingStep( 3 );

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			if ( opBaseTypeID == OBT_SETLE	 ) lhs[i] = CmpMaskLE(op1[op1_idx], op2[op2_idx]); else
			if ( opBaseTypeID == OBT_SETGE	 ) lhs[i] = CmpMaskGE(op1[op1_idx], op2[op2_idx]); else
			if ( opBaseTypeID == OBT_SETLT	 ) lhs[i] = CmpMaskLT(op1[op1_idx], op2[op2_idx]); else
			if ( opBaseTypeID == OBT_SETGT	 ) lhs[i] = CmpMaskGT(op1[op1_idx], op2[op2_idx]); else
			{ DASSERT( 0 ); }
		}

		op1_idx	+= op1_step;
		op2_idx	+= op2_step;
	}

	ctx.NextInstruction();
}

void Inst_IfTrue( Context &ctx, u_int blocksN );
void Inst_OrElse( Context &ctx, u_int blocksN );

//==================================================================
}
//==================================================================
}

#endif
