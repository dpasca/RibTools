//==================================================================
/// RI_SVM_Ops_Texture.h
///
/// Created by Davide Pasca - 2010/1/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_TEXTURE_H
#define RI_SVM_OPS_TEXTURE_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
template <class TA, const size_t N_COORDS>
void Inst_Texture( Context &ctx )
{
			TA		*lhs	= ctx.GetRW( (			TA *)0, 1 );
	const SlStr		*pName	= ctx.GetRO( (const	 SlStr *)0, 2 );
	const Float_	*pST[8][2];
	int				stOffs[8][2];
	int				stSteps[8][2];

	if ( N_COORDS >= 1 )
	{
		for (size_t i=0; i < N_COORDS; ++i)
		{
			pST[i][0] = ctx.GetRO( (const	 Float_*)0, 3 + i );
			pST[i][1] = ctx.GetRO( (const	 Float_*)0, 4 + i );
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

	bool	des_varying = ctx.IsSymbolVarying( 1 );

	if ( des_varying )
	{
		for (u_int i=0; i < ctx.mBlocksN; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( i );
			{
			if ( sizeof(TA) == sizeof(Float_) )
				lhs[i] = TA( 0.5f );// pST[0][0][ stOffs[0][0] ] + pST[0][1][ stOffs[0][1] ];
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
	else
	{
		DASSERT( !ctx.IsSymbolVarying( 3 ) );

		for (u_int i=0; i < 1; ++i)
		{
			SLRUNCTX_BLKWRITECHECK( 0 );
			{
			if ( sizeof(TA) == sizeof(Float_) )
				lhs[i] = TA( 0.5f );//lhs[i] = pST[0][0][ stOffs[0][0] ] + pST[0][1][ stOffs[0][1] ];
			else
			{
				DASSERT( 0 );
			}
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
