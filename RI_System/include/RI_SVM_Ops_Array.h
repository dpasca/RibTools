//==================================================================
/// RI_SVM_Ops_Array.h
/// Fixed-size array instructions for the shader VM.
//==================================================================

#ifndef RI_SVM_OPS_ARRAY_H
#define RI_SVM_OPS_ARRAY_H

#include <cmath>
#include <cstddef>

#include "RI_SVM_Context.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
inline size_t arrayIndexFromFloat( float index, const Symbol &arraySymbol )
{
    if (
        !std::isfinite( index ) ||
        index < 0 ||
        std::trunc( index ) != index ||
        index >= static_cast<float>(arraySymbol.GetArraySize()) )
    {
        DEX_OUT_OF_RANGE(
            "Array index %g is outside '%s[%zu]'",
            index,
            arraySymbol.GetNameChr(),
            arraySymbol.GetArraySize() );
    }

    return static_cast<size_t>(index);
}

//==================================================================
inline void Inst_ArrayLoad_Float( Context &ctx, u_int blocksN )
{
    auto *pDestination = static_cast<Float_ *>(ctx.GetRW( 1 ));
    const auto *pArray = static_cast<const Float_ *>(ctx.GetRO( 2 ));
    const auto *pIndex = static_cast<const Float_ *>(ctx.GetRO( 3 ));
    const Symbol &arraySymbol = ctx.GetSymbol( 2 );

    DASSTHROW( arraySymbol.IsArray(), "Array load requires an array operand" );

    const size_t arraySize = arraySymbol.GetArraySize();
    const bool destinationIsVarying = ctx.IsSymbolVarying( 1 );
    const bool arrayIsVarying = ctx.IsSymbolVarying( 2 );
    const bool indexIsVarying = ctx.IsSymbolVarying( 3 );

    if ( !destinationIsVarying )
    {
        DASSTHROW(
            !arrayIsVarying && !indexIsVarying,
            "A uniform destination cannot load from a varying array access" );

        const size_t index = arrayIndexFromFloat( (*pIndex)[0], arraySymbol );
        pDestination[0] = pArray[index];
        ctx.NextInstruction();
        return;
    }

    for (size_t blockIndex = 0; blockIndex < blocksN; ++blockIndex)
    {
        const size_t arrayBlockOffset = arrayIsVarying ? blockIndex * arraySize : 0;
        const size_t indexBlock = indexIsVarying ? blockIndex : 0;

        for (size_t laneIndex = 0; laneIndex < DMT_SIMD_FLEN; ++laneIndex)
        {
            const size_t pointIndex = blockIndex * DMT_SIMD_FLEN + laneIndex;
            if ( pointIndex >= ctx.GetProcessorsN() )
                break;

            const size_t index = arrayIndexFromFloat(
                pIndex[indexBlock][indexIsVarying ? laneIndex : 0],
                arraySymbol );
            const size_t arrayLane = arrayIsVarying ? laneIndex : 0;
            pDestination[blockIndex][laneIndex] =
                pArray[arrayBlockOffset + index][arrayLane];
        }
    }

    ctx.NextInstruction();
}

//==================================================================
inline void Inst_ArrayStore_Float( Context &ctx, u_int blocksN )
{
    (void)blocksN;

    const auto *pSource = static_cast<const Float_ *>(ctx.GetRO( 1 ));
    auto *pArray = static_cast<Float_ *>(ctx.GetRW( 2 ));
    const auto *pIndex = static_cast<const Float_ *>(ctx.GetRO( 3 ));
    const Symbol &arraySymbol = ctx.GetSymbol( 2 );

    DASSTHROW( arraySymbol.IsArray(), "Array store requires an array operand" );

    const size_t arraySize = arraySymbol.GetArraySize();
    const bool sourceIsVarying = ctx.IsSymbolVarying( 1 );
    const bool arrayIsVarying = ctx.IsSymbolVarying( 2 );
    const bool indexIsVarying = ctx.IsSymbolVarying( 3 );

    if ( !arrayIsVarying )
    {
        DASSTHROW(
            !sourceIsVarying && !indexIsVarying,
            "A uniform array cannot be written using varying operands" );

        const size_t index = arrayIndexFromFloat( (*pIndex)[0], arraySymbol );
        pArray[index] = pSource[0];
        ctx.NextInstruction();
        return;
    }

    for (size_t blockIndex = 0; blockIndex < ctx.mBlocksN; ++blockIndex)
    {
        const size_t sourceBlock = sourceIsVarying ? blockIndex : 0;
        const size_t indexBlock = indexIsVarying ? blockIndex : 0;
        const size_t arrayBlockOffset = blockIndex * arraySize;

        for (size_t laneIndex = 0; laneIndex < DMT_SIMD_FLEN; ++laneIndex)
        {
            const size_t pointIndex = blockIndex * DMT_SIMD_FLEN + laneIndex;
            if ( pointIndex >= ctx.GetProcessorsN() )
                break;

            const size_t index = arrayIndexFromFloat(
                pIndex[indexBlock][indexIsVarying ? laneIndex : 0],
                arraySymbol );
            const size_t sourceLane = sourceIsVarying ? laneIndex : 0;
            pArray[arrayBlockOffset + index][laneIndex] =
                pSource[sourceBlock][sourceLane];
        }
    }

    ctx.NextInstruction();
}

//==================================================================
}
//==================================================================
}

#endif
