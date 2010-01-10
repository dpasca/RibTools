//==================================================================
/// RI_SlShader_Ops_Compare.h
///
/// Created by Davide Pasca - 2010/1/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_COMPARE_H
#define RI_SLSHADER_OPS_COMPARE_H

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
	const TA	*lhs	= ctx.GetRO( (const TA *)0, 1 );
	const TA	*rhs	= ctx.GetRO( (const TA *)0, 2 );

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
void Inst_SETCMP_EQ( SlRunContext &ctx )
{
	  VecNMask*	lhs	= ctx.GetRW( (VecNMask *)0, 1 );
	const TB*	op1	= ctx.GetRO( (const TB *)0, 2 );
	const TB*	op2	= ctx.GetRO( (const TB *)0, 3 );

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
				if ( opBaseTypeID == OBT_SETEQ	 ) lhs[i] = CmpMaskEQ(op1[op1_offset], op2[op2_offset]); else
				if ( opBaseTypeID == OBT_SETNEQ	 ) lhs[i] = CmpMaskNE(op1[op1_offset], op2[op2_offset]); else
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
			TB	expandedOp1 = GetBroadcast0Lane( op1[0] );	// replicate a single value through the whole SIMD register
			TB	expandedOp2 = GetBroadcast0Lane( op2[0] );	// ...
			if ( opBaseTypeID == OBT_SETEQ	 ) lhs[0] = CmpMaskEQ(expandedOp1, expandedOp2); else
			if ( opBaseTypeID == OBT_SETNEQ	 ) lhs[0] = CmpMaskNE(expandedOp1, expandedOp2); else
			{ DASSERT( 0 );		}
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
template <class TB, const OpBaseTypeID opBaseTypeID>
void Inst_SETCMP_EQ_NoVary( SlRunContext &ctx )
{
	  VecNMask*	lhs	= ctx.GetRW( (VecNMask *)0, 1 );
	const TB*	op1	= ctx.GetRO( (const TB *)0, 2 );
	const TB*	op2	= ctx.GetRO( (const TB *)0, 3 );

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
void Inst_SETCMP_REL( SlRunContext &ctx )
{
	  VecNMask*	lhs	= ctx.GetRW( (VecNMask *)0, 1 );
	const TB*	op1	= ctx.GetRO( (const TB *)0, 2 );
	const TB*	op2	= ctx.GetRO( (const TB *)0, 3 );

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
				if ( opBaseTypeID == OBT_SETLE	 ) lhs[i] = CmpMaskLE(op1[op1_offset], op2[op2_offset]); else
				if ( opBaseTypeID == OBT_SETGE	 ) lhs[i] = CmpMaskGE(op1[op1_offset], op2[op2_offset]); else
				if ( opBaseTypeID == OBT_SETLT	 ) lhs[i] = CmpMaskLT(op1[op1_offset], op2[op2_offset]); else
				if ( opBaseTypeID == OBT_SETGT	 ) lhs[i] = CmpMaskGT(op1[op1_offset], op2[op2_offset]); else
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
			TB	expandedOp1 = GetBroadcast0Lane( op1[0] );	// replicate a single value through the whole SIMD register
			TB	expandedOp2 = GetBroadcast0Lane( op2[0] );	// ...
			if ( opBaseTypeID == OBT_SETLE	 ) lhs[0] = CmpMaskLE(expandedOp1, expandedOp2); else
			if ( opBaseTypeID == OBT_SETGE	 ) lhs[0] = CmpMaskGE(expandedOp1, expandedOp2); else
			if ( opBaseTypeID == OBT_SETLT	 ) lhs[0] = CmpMaskLT(expandedOp1, expandedOp2); else
			if ( opBaseTypeID == OBT_SETGT	 ) lhs[0] = CmpMaskGT(expandedOp1, expandedOp2); else
			{ DASSERT( 0 );		}
			}
		}
	}

	ctx.NextInstruction();
}

void Inst_IfTrue( SlRunContext &ctx );
void Inst_OrElse( SlRunContext &ctx );

//==================================================================
}

//==================================================================
}

#endif
