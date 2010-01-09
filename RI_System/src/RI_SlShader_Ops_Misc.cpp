//==================================================================
/// RI_SlShader_Ops_Misc.cpp
///
/// Created by Davide Pasca - 2009/10/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlRunContext.h"
#include "RI_SlShader_Ops_Misc.h"

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
void Inst_Faceforward( SlRunContext &ctx )
{
		  Float3_* lhs	= ctx.GetRW( (		Float3_ *)0, 1 );
	const Float3_* pN	= ctx.GetRO( (const Float3_ *)0, 2 );
	const Float3_* pI	= ctx.GetRO( (const Float3_ *)0, 3 );

	const SymbolI*	pNgSymI = ctx.mpGridSymIList->FindSymbolI( "Ng" );
	const Float3_*	pNg = (const Float3_ *)pNgSymI->GetData();

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );
	bool	N_step	= ctx.IsSymbolVarying( 2 );
	bool	I_step	= ctx.IsSymbolVarying( 3 );
	bool	Ng_step	= pNgSymI->IsVarying() ? 1 : 0;

	if ( lhs_varying )
	{
		int		N_offset	= 0;
		int		I_offset	= 0;
		int		Ng_offset	= 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = pN[N_offset] * DSign( -pI[I_offset].GetDot( pNg[Ng_offset] ) );
			}

			N_offset	+= N_step	;
			I_offset	+= I_step	;
			Ng_offset	+= Ng_step	;
		}
	}
	else
	{
		DASSERT( !N_step		);
		DASSERT( !I_step		);
		DASSERT( !Ng_step	);

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
				lhs[0] = pN[0] * DSign( -pI[0].GetDot( pNg[0] ) );
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Normalize( SlRunContext &ctx )
{
		  Float3_*	lhs	= ctx.GetRW( (		Float3_ *)0, 1 );
	const Float3_*	op1	= ctx.GetRO( (const Float3_ *)0, 2 );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	if ( lhs_varying )
	{
		int		op1_step = ctx.GetSymbolVaryingStep( 2 );
		int		op1_offset = 0;

		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				lhs[i] = op1[op1_offset].GetNormalized();
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
				lhs[0] = op1[0].GetNormalized();
			}
		}
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_CalculateNormal( SlRunContext &ctx )
{
		  Float3_*	lhs	= ctx.GetRW( (		Float3_ *)0, 1 );
	const Float3_*	op1	= ctx.GetRO( (const Float3_ *)0, 2 );

	const Float_*	pOODu	= (const Float_*)ctx.mpGridSymIList->FindSymbolIData( "_oodu" );
	const Float_*	pOODv	= (const Float_*)ctx.mpGridSymIList->FindSymbolIData( "_oodv" );

	// only varying input and output !
	DASSERT( ctx.IsSymbolVarying( 1 ) && ctx.IsSymbolVarying( 2 ) );

	bool	lhs_varying = ctx.IsSymbolVarying( 1 );

	//SLRUNCTX_BLKWRITECHECK( i );

	Float3_	dPDu[ MP_GRID_MAX_SIMD_BLKS ];

	{
		u_int	blk = 0;
		for (u_int iy=0; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				const Float3_	&blkOp1 = op1[blk];
				Float3_			&blkdPDu = dPDu[blk];

				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					blkdPDu[0][sub] = blkOp1[0][sub] - blkOp1[0][sub-1];
					blkdPDu[1][sub] = blkOp1[1][sub] - blkOp1[1][sub-1];
					blkdPDu[2][sub] = blkOp1[2][sub] - blkOp1[2][sub-1];
				}

				blkdPDu[0][0] = blkdPDu[0][1];
				blkdPDu[1][0] = blkdPDu[1][1];
				blkdPDu[2][0] = blkdPDu[2][1];

				blkdPDu = blkdPDu * pOODu[blk];
			}
		}
	}

	{
		u_int	blk = ctx.mBlocksXN;
		for (u_int iy=1; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				Float3_	dPDv = (op1[blk] - op1[blk - ctx.mBlocksXN]) * pOODv[blk];

				lhs[blk] = dPDu[blk].GetCross( dPDv ).GetNormalized();
			}
		}

		for (u_int blk=0; blk < ctx.mBlocksXN; ++blk)
			lhs[blk] = lhs[blk + ctx.mBlocksXN];
	}

	ctx.NextInstruction();
}

//==================================================================
}

//==================================================================
}
