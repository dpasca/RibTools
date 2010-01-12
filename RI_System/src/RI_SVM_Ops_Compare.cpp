//==================================================================
/// RI_SVM_Ops_Compare.cpp
///
/// Created by Davide Pasca - 2010/1/10
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Ops_Compare.h"
#include "RI_SVM_Context.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

/*
	ifrue.b

	[...]

	orelse

	[...]

	funcopend
*/

//==================================================================
void Inst_IfTrue( Context &ctx )
{
	const VecNMask*	op1	= (const VecNMask *)ctx.GetRW( 1 );

	bool	varying = ctx.IsSymbolVarying( 1 );

	if ( varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
			//lhs[i] = CmpMaskLE(op1[op1_offset], op2[op2_offset]);
			}
		}

		// we are in if-true (non-uniform)
		ctx.mFopStack.push(
			SRC_FuncopStack::FLG_IFTRUE |
			SRC_FuncopStack::FLG_IFTRUE_VARY );

		ctx.NextInstruction();
	}
	else
	{
		bool	doIfBody = false;

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			if ( op1[0] == VecNMaskFull )
				doIfBody = true;
			}
		}

		if ( doIfBody )
		{
			ctx.mFopStack.push(
				SRC_FuncopStack::FLG_IFTRUE |
				SRC_FuncopStack::FLG_IFTRUE_TRUE );

			// fall through the body
			ctx.NextInstruction();
		}
		else
		{
			// go tot he end of the if or begin of the else
			const CPUWord *pOp = ctx.GetOp(0);

			u_short beginElse_endifAddr = pOp->mOpCode.mFuncopEndAddr;

			ctx.GotoInstruction( beginElse_endifAddr );

			ctx.mFopStack.push(
				SRC_FuncopStack::FLG_IFTRUE );
		}
	}
}

//==================================================================
void Inst_OrElse( Context &ctx )
{
	u_int	funcopFlgs = ctx.mFopStack.top();

	ctx.mFopStack.pop();

	DASSERT( 0 != (funcopFlgs & SRC_FuncopStack::FLG_IFTRUE) );

	if ( funcopFlgs & SRC_FuncopStack::FLG_IFTRUE_VARY )
	{
		// set the state to or-else and continue..
		ctx.mFopStack.push( SRC_FuncopStack::FLG_ORELSE );
		// fall through the else body
		ctx.NextInstruction();
	}
	else
	if ( funcopFlgs & SRC_FuncopStack::FLG_IFTRUE_TRUE )
	{
		// already went through the body of the "if", so skip
		// the body of the "else"

		const CPUWord *pOp = ctx.GetOp(0);

		u_short endifAddr = pOp->mOpCode.mFuncopEndAddr;

		// go beyond the closing funcopend ..we don't care for any
		// validation..
		ctx.GotoInstruction( endifAddr );
		ctx.NextInstruction();
	}
	else
	{
		// the "if" body didn't execute, so..
		// set the state to or-else and continue..
		ctx.mFopStack.push( SRC_FuncopStack::FLG_ORELSE );
		// fall through the else body
		ctx.NextInstruction();
	}
}

//==================================================================
}
//==================================================================
}
