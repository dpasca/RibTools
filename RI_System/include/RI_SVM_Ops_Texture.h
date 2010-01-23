//==================================================================
/// RI_SVM_Ops_Texture.h
///
/// Created by Davide Pasca - 2010/1/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_TEXTURE_H
#define RI_SVM_OPS_TEXTURE_H

#include "RI_Texture.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
template <class TA, const size_t N_COORDS>
void Inst_Texture( Context &ctx, u_int blocksN )
{
			TA		*lhs	= (			TA *)ctx.GetRW( 1 );
	const SlStr		*pName	= (const SlStr *)ctx.GetRO( 2 );
	const Float_	*pST[8][2];
	int				stOffs[8][2];
	int				stSteps[8][2];

	RCOwn<Texture>	oTex;
	oTex.Borrow( ctx.mpAttribs->GetTexture( pName->mStr ) );

	if ( N_COORDS >= 1 )
	{
		for (size_t i=0; i < N_COORDS; ++i)
		{
			pST[i][0] = (const Float_*)ctx.GetRO( 3 + i );
			pST[i][1] = (const Float_*)ctx.GetRO( 4 + i );
			stOffs[i][0] = 0;
			stOffs[i][1] = 0;
			stSteps[i][0] = ctx.GetSymbolVaryingStep( 3 + i );
			stSteps[i][1] = ctx.GetSymbolVaryingStep( 4 + i );
		}
	}
	else
	{
		SymbolI	*pSymS = ctx.mpGridSymIList->FindSymbolI( "s" );
		SymbolI	*pSymT = ctx.mpGridSymIList->FindSymbolI( "t" );

		pST[0][0]	= (const Float_*)pSymS->GetData();
		pST[0][1]	= (const Float_*)pSymT->GetData();
		stOffs[0][0] = 0;
		stOffs[0][1] = 0;
		stSteps[0][0] = ctx.GetSymbolVaryingStep( 3 );
		stSteps[0][1] = ctx.GetSymbolVaryingStep( 4 );
	}

/*
	Float_	dsDu[ MP_GRID_MAX_SIZE_SIMD_BLKS ];
	Float_	dtDu[ MP_GRID_MAX_SIZE_SIMD_BLKS ];

	{
		u_int	blk = 0;
		for (u_int iy=0; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				const Float_	&blk_s	 = pST[0][0][blk];
				const Float_	&blk_t	 = pST[0][1][blk];

				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					dsDu[0][sub] = blk_s[0][sub] - blk_s[0][sub-1];
					dtDu[0][sub] = blk_t[0][sub] - blk_t[0][sub-1];
				}

				dsDu[0] = dsDu[1];
				dsDu[0] = dsDu[1];
				blkdPDu[1][0] = blkdPDu[1][1];
				blkdPDu[2][0] = blkdPDu[2][1];

				blkdPDu = blkdPDu * pOODu[blk];
			}
		}
	}
*/
/*
	if ( N_COORDS >= 1 )
	{
		Float_	above_s[ MP_GRID_MAX_DIM_SIMD_BLKS ];
		Float_	above_t[ MP_GRID_MAX_DIM_SIMD_BLKS ];

		float	prev_dsDu = 0;
		float	prev_dtDv = 0;

		u_int	blk = 0;
		for (u_int iy=0; iy < ctx.mPointsYN; ++iy)
		{
			for (u_int ixb=0; ixb < 1; ++ixb, ++blk)
			{
				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					dsDu[sub] = pST[0][sub] - pST[0][sub-1];
					dtDv[sub] = pST[1][sub] - pST[1][sub-1];

					oTex->Sample_1_1x1( sample, s00, t00 );
				}

				prev_dsDu = pST[0][DMT_SIMD_FLEN-1];
				prev_dtDv = pST[1][DMT_SIMD_FLEN-1];
			}

			for (u_int ixb=1; ixb < ctx.mBlocksXN; ++ixb, ++blk)
			{
				dsDu[0][0] = pST[0][0] - prev_dsDu;
				dtDv[1][0] = pST[1][0] - prev_dtDv;

				for (u_int sub=1; sub < DMT_SIMD_FLEN; ++sub)
				{
					dsDu[sub] = pST[0][sub] - pST[0][sub-1];
					dtDv[sub] = pST[1][sub] - pST[1][sub-1];
				}

				prev_dsDu = pST[0][DMT_SIMD_FLEN-1];
				prev_dtDv = pST[1][DMT_SIMD_FLEN-1];

				blkdPDu = blkdPDu * pOODu[blk];
			}
		}
	}
	else
*/
	{
		for (u_int i=0; i < blocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
				Float_	s00 = pST[0][0][i];
				Float_	t00 = pST[0][1][i];

				if ( sizeof(TA) == sizeof(Float_) )
				{
					Float_	sample;

					oTex->Sample_1_filter( sample, s00, t00 );

					lhs[i] = sample;
				}
				else
				{
					DASSERT( 0 );
				}
			}

			for (size_t sti=0; sti < N_COORDS; ++sti)
			{
				stOffs[sti][0] += stSteps[sti][0];
				stOffs[sti][1] += stSteps[sti][1];
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
