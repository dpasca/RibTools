//==================================================================
/// RI_SlShader_Binding.cpp
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SVM_Shader.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
static void matchSymbols( const Symbol &dst, const Symbol &src, bool &out_needConvertToVarying )
{
    out_needConvertToVarying = false;

    // this shouldn't really happen at this stage
    DASSTHROW( dst.IsName( src.GetNameChr() ),
        ("Names not matching ! %s != %s", dst.GetNameChr(), src.GetNameChr()) );

    DASSTHROW( dst.mType == src.mType,
                ("Type is %i but expecting %i for '%s'",
                    dst.mType,
                    src.mType,
                    dst.GetNameChr()) );

    if ( src.IsVarying() )
    {
        if NOT( dst.IsVarying() )
        {
            DASSTHROW( 0,
                ("Cannot assign a varying value to a non varying one for '%s'",
                    dst.GetNameChr()) );
        }
    }
    else
    {
        if ( dst.IsVarying() )
        {
            out_needConvertToVarying = true;
        }
    }

    if ( dst.IsConstant() )
    {
        if NOT( src.IsConstant() )
        {
            DASSTHROW( 0,
                ("Cannot assign a non-constant value to a constant one '%s'",
                    dst.GetNameChr()) );
        }
    }
}

//==================================================================
static void bindGlobalStorageSymbol(
                        Value				&destValue	,
                        const Symbol		&shaSym		,
                        SymbolIList			&gridSymIList,
                        const SymbolList	&globalSyms
                        )
{
    // instances of all symbols considered as globals for the shader are already
    // in the grid, therefore we look into the grid
    // Notice that in the state machine, symbols such as "Ka" are marked as global,
    // but in fact, those are considered as parameters in the shader.. while they
    // are global in the context of the state machine.. basically dst different kind
    // of globals !

    if ( shaSym.IsConstant() )
    {
        // Example symbols: PI

        // find the global symbol in the globals from the state machine

        const Symbol	*pGlobalSym = globalSyms.FindSymbol( shaSym.GetNameChr() );

        DASSTHROW( pGlobalSym != NULL, ("Could not find the global symbol %s !\n", shaSym.GetNameChr()) );

        bool needConvertToVarying;
        matchSymbols( shaSym, *pGlobalSym, needConvertToVarying );

        // grab the constant data from the global in the state machine.. not from the
        // symbol in the shader.. which is not supposed to exist 8)
        destValue.Flags.mOwnData = 0;
        destValue.SetDataR( pGlobalSym->GetConstantData(), pGlobalSym );
    }
    else
    {
        // Example symbols: P, Cs, N, Ng ...

        // find the global symbol's instance in the grid

        SymbolI	*pGridGlobalSymI = gridSymIList.FindSymbolI( shaSym.GetNameChr() );

        DASSTHROW( pGridGlobalSymI != NULL, ("Could not find the global symbol %s !\n", shaSym.GetNameChr()) );

        bool needConvertToVarying;
        matchSymbols( shaSym, *pGridGlobalSymI->GetSrcSymbol(), needConvertToVarying );

        // grab the data from the symbol the symbol instance in the grid
        destValue.Flags.mOwnData = 0;
        destValue.SetDataRW( pGridGlobalSymI->GetRWData(), pGridGlobalSymI->GetSrcSymbol() );
    }
}

//==================================================================
static void bindTemporaryStorageSymbol(
                        Value				&destValue	,
                        const Symbol		&shaSym		,
                        size_t				maxPoints
                        )
{
    if ( shaSym.IsConstant() )
    {
        // Example symbols: MY_FACTOR ...

        // temporary const is dst local const ..grab it from the shader

        destValue.Flags.mOwnData = 0;
        destValue.SetDataR( shaSym.GetConstantData(), &shaSym );
    }
    else
    {
        // Example symbols: temp2, $su1, $v1 ...
        // Note: Indeed, $ registers are considered as temporaries

        // temporary non-const requires local allocation

        size_t	allocN = shaSym.IsVarying() ? maxPoints : 1;

        destValue.Flags.mOwnData = 1;
        destValue.SetDataRW( shaSym.AllocClone( allocN ), &shaSym );
    }
}

//==================================================================
Value	*ShaderInst::Bind(
                    const SymbolList	&globalSyms,
                    SymbolIList			&gridSymIList,
                    DVec<u_int>			&out_defParamValsStartPCs ) const
{
    out_defParamValsStartPCs.clear();

    size_t	symbolsN = moShader->mpShaSyms.size();

    Value	*pDataSegment = DNEW Value [ symbolsN ];

    for (size_t i=0; i < symbolsN; ++i)
    {
        const Symbol		&shaSym			= *moShader->mpShaSyms[i];

        switch ( shaSym.mStorage )
        {
        case Symbol::STOR_GLOBAL:
            bindGlobalStorageSymbol(
                        pDataSegment[i],
                        shaSym,
                        gridSymIList,
                        globalSyms );
            break;

        case Symbol::STOR_PARAMETER:
            {
                // paranoia check for the moment..
                DASSERT( NULL == gridSymIList.FindSymbolI( shaSym.GetNameChr() ) );

                //---- Handle in-line params

                // will need to ensure that in-line params are uniform and constant !

                const SymbolI	*pParamSymI = mCallSymIList.FindSymbolI( shaSym.GetNameChr() );

                // additionally look into params in attributes ?

                if ( pParamSymI )
                {
                    const Symbol	*pSrcSymbol = pParamSymI->GetSrcSymbol();

                    bool needConvertToVarying;
                    matchSymbols( shaSym, *pSrcSymbol, needConvertToVarying );

                    if ( needConvertToVarying )
                    {
                        pDataSegment[i].AllocFillConstData( &shaSym, mMaxPointsN, pParamSymI->GetConstantData() );
                    }
                    else
                    {
                        // as is..
                        pDataSegment[i].Flags.mOwnData = 0;
                        pDataSegment[i].SetDataR( pParamSymI->GetConstantData(), pSrcSymbol );
                    }
                }
                else
                {
                    Value	&destValue = pDataSegment[i];

                    // is the param constant ?  ..odd, but...
                    if ( shaSym.IsConstant() )
                    {
                        // parameter const is dst local const ..grab it from the shader
                        destValue.Flags.mOwnData = 0;
                        destValue.SetDataR( shaSym.GetConstantData(), &shaSym );
                    }
                    else
                    {
                        // allocate storage for the param
                        size_t	allocN = shaSym.IsVarying() ? mMaxPointsN : 1;

                        pDataSegment[i].Flags.mOwnData = 1;
                        pDataSegment[i].SetDataRW( shaSym.AllocClone( allocN ), &shaSym );

                        // setup init code !
                        if ( moShader->mpShaSymsStartPCs[i] != INVALID_PC )
                            out_defParamValsStartPCs.push_back( moShader->mpShaSymsStartPCs[i] );
                    }
                }
            }
            break;

        case Symbol::STOR_TEMPORARY:
            bindTemporaryStorageSymbol( pDataSegment[i], shaSym, mMaxPointsN );
            break;

        default:
            DASSTHROW( 0, ("Unsupported data type !") );
            break;
        }
    }

    return pDataSegment;
}

//==================================================================
void ShaderInst::Unbind( Value * &pDataSegment ) const
{
    size_t	symbolsN = moShader->mpShaSyms.size();

    for (size_t i=0; i < symbolsN; ++i)
    {
        if ( pDataSegment[i].Flags.mOwnData )
            pDataSegment[i].mpSrcSymbol->FreeClone( pDataSegment[i].Data.pVoidValue );
    }

    DSAFE_DELETE_ARRAY( pDataSegment );
    pDataSegment = NULL;
}

//==================================================================
}
//==================================================================
}
