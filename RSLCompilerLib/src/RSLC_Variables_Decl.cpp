//==================================================================
/// RSLC_Variables_Decl.cpp
///
/// Created by Davide Pasca - 2010/3/22
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"

#include "RSLC_Variables.h"

//==================================================================
namespace RSLC
{

//==================================================================
static bool discoverVars_FindTypeUntilNonTerm(
                            TokNode *pNode,
                            size_t	&i,
                            TokNode	* &out_pDTypeNode ,
                            TokNode	* &out_pDetailNode,
                            TokNode	* &out_pOutputNode
                            )
{
    BlockType	blkType = pNode->GetBlockType();
    
    for (; i < pNode->mpChilds.size(); ++i)
    {
        TokNode	*pChild = pNode->mpChilds[i];

        if ( pChild->GetTokIDType() == T_TYPE_DATATYPE )
        {
            out_pDTypeNode = pChild;
        }
        else
        if ( pChild->GetTokIDType() == T_TYPE_DETAIL )
        {
            if ( out_pDetailNode )
                throw Exception( "Broken variable declaration", pChild );

            out_pDetailNode = pChild;
        }
        else
        if ( pChild->GetTokID() == T_KW_output )
        {
            if ( out_pOutputNode &&
                (	blkType != BLKT_DECL_PARAMS_SH
                 && blkType != BLKT_DECL_PARAMS_FN
                 && blkType != BLKT_CALL_OR_DELC_PARAMS_FN) )
            {
                throw Exception(
                        "Keyword 'output' can be specified only in "
                        "function and shader parameters declaration", pChild );
            }

            out_pOutputNode = pChild;
        }
        else
        if ( pChild->IsNonTerminal() )
        {
            // came to reach a non-terminal
            return true;
        }
        else
        {
            // not a variables declaration after all !
            return false;
        }
    }

    // didn't come to reach a non-terminal
    return false;
}

//==================================================================
static bool discoverVars_IsArray(
                            TokNode *pNode,
                            size_t	&i )
{
    // is this an array declaration ?
    if ( (i+1) < pNode->mpChilds.size() )
    {
        const TokNode	*pNextChild = pNode->mpChilds[i+1];

        // is this maybe an array declaration ?
        if ( pNextChild->GetTokID() == T_OP_LFT_SQ_BRACKET )
        {
            i += 2;	// advance to _after_ the [
            return true;
        }
    }

    return false;
}

//==================================================================
static bool discoverVars_FindNext( TokNode *pBlkNode, size_t &i, bool &out_isFuncDecl )
{
    // also see if it's a function declaration after all !
    // such as: float dude()
    out_isFuncDecl = false;

    // NOTE: a valid variable declaration must be followed by any of the following symbols
    // ',', ';', '='
    // ..however it could be followed by nothing (when in a function params block)

    bool isParamsDecl;
    switch ( pBlkNode->GetBlockType() )
    {
    case BLKT_DECL_PARAMS_SH:
    case BLKT_DECL_PARAMS_FN:
    case BLKT_CALL_OR_DELC_PARAMS_FN:
        isParamsDecl = true;
        break;

    default:
        isParamsDecl = false;
        break;
    }

    // if we have nothing following
    if ( (i+1) >= pBlkNode->mpChilds.size() )
    {
        // ..is this because we are in a function/shader params block ?
        if NOT( isParamsDecl )
        {
            throw Exception(
                        pBlkNode->mpChilds[i],
                        "Broken variable declaration. Did you forget a semicolon ?" );
        }

        return true;	// is done
    }
    else
    {
        i += 1;

        // what comes next ? ..ensure that it's something we expect
        TokenID	tokIDAfterNonTerm = pBlkNode->mpChilds[i]->GetTokID();

        switch ( tokIDAfterNonTerm  )
        {
        // find next ; , or nothing if we are in function params
        case T_OP_ASSIGN:
            for (; i < pBlkNode->mpChilds.size(); ++i)
            {
                switch ( pBlkNode->mpChilds[i]->GetTokID() )
                {
                case T_OP_COMMA:	return false;	// continue
                case T_OP_SEMICOL:	return true;	// is done
                default: break;
                }
            }

            //  dude( var = 1 ) // ..case
            if ( isParamsDecl )
                return true;	// done
            else
            {
                // float a = 1 junk  // ..case
                throw Exception(
                            pBlkNode->mpChilds[i],
                            "Broken variable declaration. Did you forget a semicolon ?" );

                return false;	// continue
            }
            break;

        case T_OP_COMMA:	return false;	// continue
        case T_OP_SEMICOL:	return true;	// is done

        // if the non-terminal is followed by a ( , then it's likely
        // to be a function declaration
        case T_OP_LFT_BRACKET:
                out_isFuncDecl = true;
                return true;

        default:
            throw Exception(
                        pBlkNode->mpChilds[i],
                        "Unexpected symbol after the variable declaration" );

            return true;	// is done (doesn't really matter here)
        }
    }
}

// TODO: functions and shader params are allowed to change type after a comma !
//==================================================================
static size_t discoverVariablesDeclarations_sub( TokNode *pBlkNode, size_t i )
{
    BlockType	blkType = pBlkNode->GetBlockType();

    TokNode	*pDTypeNode	;
    TokNode	*pDetailNode;
    TokNode	*pOutputNode;

    pDTypeNode  = NULL;
    pDetailNode = NULL;
    pOutputNode = NULL;

    if ( pBlkNode->mpChilds[i]->mpToken->sourceLine == 77 )
    {
        int yoyo = 1;
    }

    for (; i < pBlkNode->mpChilds.size();)
    {
        bool foundNonTerm =
            discoverVars_FindTypeUntilNonTerm(
                            pBlkNode,
                            i,
                            pDTypeNode ,
                            pDetailNode,
                            pOutputNode
                            );

        // not variables declaration code ?
        if NOT( foundNonTerm )
            return i;

        TokNode *pLastNonTerm = pBlkNode->mpChilds[ i ];

        // is this an array ?
        bool isArray = discoverVars_IsArray( pBlkNode, i );

        // is this the end of the declaration block ?
        bool isFuncDecl;
        bool isLastVariable = discoverVars_FindNext( pBlkNode, i, isFuncDecl );
        // it's a function declaration... nevermind then !
        if ( isFuncDecl )
            return i;

        //----------------
        // finally add the variable
        Variable *pVar = AddVariable(
                                pBlkNode,
                                pDTypeNode,
                                pDetailNode,
                                NULL,
                                pLastNonTerm,
                                isArray,
                                pOutputNode != NULL );

        if ( blkType == BLKT_DECL_PARAMS_SH )
        {
            // mark as shader param, if so
            pVar->mIsSHParam = true;	
        }
        //----------------

        i += 1;

        if ( isLastVariable )
            break;
    }

    return i;
}

//==================================================================
void DiscoverVariablesDeclarations( TokNode *pNode )
{
    for (size_t i = 0; i < pNode->mpChilds.size(); ++i)
        DiscoverVariablesDeclarations( pNode->mpChilds[i] );

    BlockType	blkType = pNode->GetBlockType();

    if (	blkType == BLKT_DECL_PARAMS_SH
         || blkType == BLKT_DECL_PARAMS_FN
         || blkType == BLKT_CALL_OR_DELC_PARAMS_FN
         || blkType == BLKT_CODEBLOCK
         || blkType == BLKT_ROOT )
    {
        for (size_t i=0; i < pNode->mpChilds.size();)
        {
            TokNode	*pTokNode = pNode->GetChildTry( i );
            TokenIDType	idType = pTokNode->GetTokIDType();

            if (idType == T_TYPE_DATATYPE ||
                idType == T_TYPE_DETAIL ||
                // idType == T_KW_output ) -- TODO: verify this
                pTokNode->GetTokID() == T_KW_output )
            {
                i = discoverVariablesDeclarations_sub( pNode, i );
            }
            else
                ++i;
        }
    }
}

//==================================================================
}
