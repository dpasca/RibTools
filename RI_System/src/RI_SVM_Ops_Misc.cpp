//==================================================================
/// RI_SVM_Ops_Misc.cpp
///
/// Created by Davide Pasca - 2009/10/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Context.h"
#include "RI_SVM_Ops_Misc.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
void Inst_Faceforward( Context &ctx, u_int blocksN )
{
		  Float3_* lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const Float3_* pN	= (const Float3_*)ctx.GetRO( 2 );
	const Float3_* pI	= (const Float3_*)ctx.GetRO( 3 );

	const SymbolI*	pNgSymI = ctx.mpGridSymIList->FindSymbolI( "Ng" );
	const Float3_*	pNg = (const Float3_ *)pNgSymI->GetData();

	int		N_step	= ctx.GetSymbolVaryingStep( 2 );
	int		I_step	= ctx.GetSymbolVaryingStep( 3 );
	int		Ng_step	= pNgSymI->IsVarying() ? 1 : 0;

	int		N_offset	= 0;
	int		I_offset	= 0;
	int		Ng_offset	= 0;

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = pN[N_offset] * DSign( -pI[I_offset].GetDot( pNg[Ng_offset] ) );
		}

		N_offset	+= N_step	;
		I_offset	+= I_step	;
		Ng_offset	+= Ng_step	;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_Normalize( Context &ctx, u_int blocksN )
{
		  Float3_*	lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	int		op1_step = ctx.GetSymbolVaryingStep( 2 );
	int		op1_idx = 0;

	for (u_int i=0; i < blocksN; ++i)
	{
		SLRUNCTX_BLKWRITECHECK( i );
		{
			lhs[i] = op1[op1_idx].GetNormalized();
		}

		op1_idx += op1_step;
	}

	ctx.NextInstruction();
}

//==================================================================
void Inst_CalculateNormal( Context &ctx, u_int blocksN )
{
		  Float3_*	lhs	= (		 Float3_*)ctx.GetRW( 1 );
	const Float3_*	op1	= (const Float3_*)ctx.GetRO( 2 );

	const Float_*	pOODu	= (const Float_*)ctx.mpGridSymIList->FindSymbolIData( "_oodu" );
	const Float_*	pOODv	= (const Float_*)ctx.mpGridSymIList->FindSymbolIData( "_oodv" );

	// only varying input and output !
	DASSERT( ctx.IsSymbolVarying( 1 ) && ctx.IsSymbolVarying( 2 ) );

	//SLRUNCTX_BLKWRITECHECK( i );

	Float3_	dPDu[ MP_GRID_MAX_SIZE_SIMD_BLKS ];
	Float3_	P_LS[ MP_GRID_MAX_SIZE_SIMD_BLKS ];

	{
		const Matrix44	&mtxCameraLocal = ctx.mpGrid->mMtxCameraLocal;

#if DMT_SIMD_FLEN == 1
		u_int	blkRow = 0;

		for (u_int iy=0; iy < ctx.mPointsYN; ++iy, blkRow += ctx.mBlocksXN)
		{
			u_int	blkRowEnd = blkRow + ctx.mBlocksXN;

			// calculate the position in local space
			for (u_int ixb=blkRow; ixb < blkRowEnd; ++ixb)
				P_LS[ ixb ] = V3__V3W1_Mul_M44<Float_>( op1[ ixb ], mtxCameraLocal );

			const Float3_	*pPrevP_LS = &P_LS[ blkRow ];

			for (u_int ixb=blkRow; ixb < blkRowEnd; ++ixb)
			{
				const Float3_	&blkP_LS = P_LS[ixb];
				Float3_			&blkdPDu = dPDu[ixb];

				blkdPDu[0][0] = blkP_LS[0][0] - pPrevP_LS[0][0][0];
				blkdPDu[1][0] = blkP_LS[1][0] - pPrevP_LS[0][1][0];
				blkdPDu[2][0] = blkP_LS[2][0] - pPrevP_LS[0][2][0];

				pPrevP_LS = &blkP_LS;

				blkdPDu = blkdPDu * pOODu[ixb];
			}
		}
#else
		Float3	prev_blkP_LS;

		u_int	blk = 0;
		for (u_int iy=0; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < 1; ++ixb, ++blk)
			{
				// calculate the position in local space
				P_LS[blk] = V3__V3W1_Mul_M44<Float_>( op1[blk], mtxCameraLocal );

				const Float3_	&blkP_LS = P_LS[blk];
				Float3_			&blkdPDu = dPDu[blk];

				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					blkdPDu[0][sub] = blkP_LS[0][sub] - blkP_LS[0][sub-1];
					blkdPDu[1][sub] = blkP_LS[1][sub] - blkP_LS[1][sub-1];
					blkdPDu[2][sub] = blkP_LS[2][sub] - blkP_LS[2][sub-1];
				}

				prev_blkP_LS[0] = blkP_LS[0][DMT_SIMD_FLEN-1];
				prev_blkP_LS[1] = blkP_LS[1][DMT_SIMD_FLEN-1];
				prev_blkP_LS[2] = blkP_LS[2][DMT_SIMD_FLEN-1];

				blkdPDu[0][0] = blkdPDu[0][1];
				blkdPDu[1][0] = blkdPDu[1][1];
				blkdPDu[2][0] = blkdPDu[2][1];

				blkdPDu = blkdPDu * pOODu[blk];
			}

			for (u_int ixb=1; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				// calculate the position in local space
				P_LS[blk] = V3__V3W1_Mul_M44<Float_>( op1[blk], mtxCameraLocal );

				const Float3_	&blkP_LS = P_LS[blk];
				Float3_			&blkdPDu = dPDu[blk];

				blkdPDu[0][0] = blkP_LS[0][0] - prev_blkP_LS[0];
				blkdPDu[1][0] = blkP_LS[1][0] - prev_blkP_LS[1];
				blkdPDu[2][0] = blkP_LS[2][0] - prev_blkP_LS[2];

				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					blkdPDu[0][sub] = blkP_LS[0][sub] - blkP_LS[0][sub-1];
					blkdPDu[1][sub] = blkP_LS[1][sub] - blkP_LS[1][sub-1];
					blkdPDu[2][sub] = blkP_LS[2][sub] - blkP_LS[2][sub-1];
				}

				prev_blkP_LS[0] = blkP_LS[0][DMT_SIMD_FLEN-1];
				prev_blkP_LS[1] = blkP_LS[1][DMT_SIMD_FLEN-1];
				prev_blkP_LS[2] = blkP_LS[2][DMT_SIMD_FLEN-1];

				blkdPDu = blkdPDu * pOODu[blk];
			}
		}
#endif
	}

#if 0
	{
		const Float3_*	pN	= (const Float3_*)ctx.mpGridSymIList->FindSymbolIData( "N" );

		for (u_int blk=0; blk < ctx.mBlocksN; ++blk)
		{
			lhs[blk] = pN[blk];
		}
	}
#else
	{
		const Matrix44	&mtxLocalCameraNorm = ctx.mpGrid->mMtxLocalCameraNorm;

		u_int	blk = ctx.mBlocksXN;
		for (u_int iy=1; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				const Float3_	&P_LS_y0x = P_LS[blk - ctx.mBlocksXN];
				const Float3_	&P_LS_y1x = P_LS[blk];

				Float3_	dPDv = (P_LS_y1x - P_LS_y0x) * pOODv[blk];

				lhs[blk] = V3__V3W0_Mul_M44<Float_>(
									dPDu[blk].GetCross( dPDv ),
									mtxLocalCameraNorm			).GetNormalized();
			}
		}

		for (u_int blk=0; blk < ctx.mBlocksXN; ++blk)
			lhs[blk] = lhs[blk + ctx.mBlocksXN];
	}
#endif

	ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}
