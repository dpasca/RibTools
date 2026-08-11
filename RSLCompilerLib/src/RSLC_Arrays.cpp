//==================================================================
/// RSLC_Arrays.cpp
///
/// Created by Davide Pasca - 2010/3/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <cmath>

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"

#include "RSLC_Arrays.h"

//==================================================================
namespace RSLC
{

//==================================================================
void RealizeArraysSizes( TokNode *pNode )
{
    DVec<Variable>	&vars = pNode->GetVars();
    for (size_t i=0; i < vars.size(); ++i)
    {
        if NOT( vars[i].mIsArray )
            continue;

        if ( vars[i].GetVarType() != VT_FLOAT )
        {
            throw Exception(
                vars[i].mpDefNameNode,
                "Only float arrays are currently implemented" );
        }

        /* At this point, we are dealing with the following structure
         (because of the reparenting by operators..
    
        float varname[3]

        ..is..

        float
        [				// pSqBracket
            varname		// vars[i].mpDefNameNode
            3			// pArrSizeNode
        */

        TokNode *pSqBracket = vars[i].mpDefNameNode->mpParent;
        DASSERT( pSqBracket != NULL );

        TokNode	*pArrSizeNode = pSqBracket->GetChildTry( 1 );
        DASSERT( pArrSizeNode != NULL );

        if ( pArrSizeNode->GetTokID() != T_VL_NUMBER )
        {
            throw Exception(
                pArrSizeNode,
                "Array size must be a compile-time constant" );
        }

        const double arrSize = pArrSizeNode->mpToken->ConstNum;

        if (
            !std::isfinite( arrSize ) ||
            arrSize <= 0 ||
            std::trunc( arrSize ) != arrSize )
        {
            throw Exception( pSqBracket, "Array size must be positive integral value" );
        }

        // enforce a "large enough" maximum size
        if ( arrSize >= static_cast<double>(1 << 20) )
        {
            throw Exception(
                pSqBracket,
                "Array size must be less than %i",
                1 << 20 );
        }

        // finally set the array size
        vars[i].mArraySize = static_cast<U32>(arrSize);
    }

    for (size_t i=0; i < pNode->mpChilds.size(); ++i)
        RealizeArraysSizes( pNode->mpChilds[i] );
}

//==================================================================
void ValidateArrayAccesses( TokNode *pNode )
{
    if ( pNode->IsTokenID( T_OP_LFT_SQ_BRACKET ) )
    {
        TokNode *pArrayNode = pNode->GetChildTry( 0 );
        TokNode *pIndexNode = pNode->GetChildTry( 1 );
        if ( pArrayNode == nullptr || pIndexNode == nullptr )
            throw Exception( pNode, "Incomplete array access" );

        const Variable *pVariable = pArrayNode->mVarLink.GetVarPtr();
        if ( pVariable == nullptr )
            throw Exception( pArrayNode, "Unknown array '%s'", pArrayNode->GetTokStr() );

        // The declaration itself has the same square-bracket shape as an access.
        if ( pVariable->mpDefNameNode != pArrayNode )
        {
            if ( !pVariable->IsArray() )
            {
                throw Exception(
                    pArrayNode,
                    "Variable '%s' is not an array",
                    pArrayNode->GetTokStr() );
            }

            if ( pIndexNode->GetTokID() == T_VL_NUMBER )
            {
                const double index = pIndexNode->mpToken->ConstNum;
                if (
                    !std::isfinite( index ) ||
                    index < 0 ||
                    std::trunc( index ) != index )
                {
                    throw Exception(
                        pIndexNode,
                        "Array index must be a non-negative integral value" );
                }

                if ( index >= static_cast<double>(pVariable->mArraySize) )
                {
                    throw Exception(
                        pIndexNode,
                        "Array index %.0f is outside '%s[%u]'",
                        index,
                        pArrayNode->GetTokStr(),
                        pVariable->mArraySize );
                }
            }
        }
    }

    for (TokNode *pChild : pNode->mpChilds)
        ValidateArrayAccesses( pChild );
}

//==================================================================
void GenerateArrayAssignOperators( TokNode *pNode )
{
    return;

    // convert:
    //	=				// T_OP_ASSIGN
    //		[			// child 0
    //			arr
    //			idx
    //		src			// child 1
    //
    // ..to..
    //
    //	=[				// T_OP_ASSIGN_ARR_ITEM
    //		arr
    //		idx
    //		src
    if ( pNode->IsTokenID( T_OP_ASSIGN ) )
    {
        TokNode	*pDestNode = pNode->GetChildTry( 0 );
        DASSERT( pDestNode != NULL );

        if ( pDestNode->IsTokenID( T_OP_LFT_SQ_BRACKET ) )
        {
            DASSERT( pDestNode->mpChilds.size() == 2 );

            // "hijack" the assignment operator and turn it into
            // an "array item assignment" virtual operator
            pNode->mpToken->id = T_OP_ASSIGN_ARR_ITEM;
            pNode->mpToken->str = "=[";
        }
    }

    for (size_t i=0; i < pNode->mpChilds.size(); ++i)
        GenerateArrayAssignOperators( pNode->mpChilds[i] );
}

//==================================================================
}
