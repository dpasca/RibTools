//==================================================================
/// RSLC_Variables.cpp
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_Exceptions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void AddVariable(
					TokNode *pNode,
					TokNode *pDTypeNode,
					TokNode *pDetailNode,
					TokNode *pSpaceCastTok,
					TokNode *pNameNode )
{
	Variable	*pVar = pNode->GetVars().grow();

	pVar->mpDTypeTok		= pDTypeNode->mpToken;
	pVar->mpOwnerNode		= pNode;
	pVar->mpDetailTok		= pDetailNode ? pDetailNode->mpToken : NULL;
	pVar->mpSpaceCastTok	= pSpaceCastTok ? pSpaceCastTok->mpToken : NULL;
	pVar->mpDefNameTok		= pNameNode ? pNameNode->mpToken : NULL;

	if ( pVar->mpDefNameTok )
	{
		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_%s",
							pNode->mBlockID,
								pVar->mpDefNameTok->str.c_str() );
	}
	else
	{
		u_int	tmpID = pNode->GetData().mVariables.size() - 1;

		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_tmp%0", pNode->mBlockID, tmpID );
	}

	if ( pVar->mpDetailTok )
	{
		if ( pVar->mpDetailTok )
		{
			if ( pVar->mpDetailTok->idType != T_TYPE_DETAIL )
				throw Exception( "Bad detail type !", pVar->mpDetailTok );

			if ( pVar->mpDetailTok->id == T_DE_varying )
				pVar->mIsVarying = true;
			else
				pVar->mIsVarying = false;
		}
		else
		{
			// varying until disproved !
			pVar->mIsVarying = true;
		}
	}
}

//==================================================================
static bool fndVarInBlock(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	*pChild2
		)
{
	if ( pChild0->mpToken->idType == T_TYPE_DATATYPE ||
		 pChild0->mpToken->idType == T_TYPE_DETAIL )
	{
		TokNode	*pDTypeNode = NULL;
		TokNode	*pDetailNode = NULL;
		TokNode	*pNameNode = NULL;

		// 3 possible cases

		// 1) TYPE NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= NULL;
			pNameNode	= pChild1;
			i += 2;
		}
		else	// 2) TYPE DETAIL NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild1;
			pNameNode	= pChild2;
			i += 3;
		}
		else	// 3) DETAIL TYPE NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild1;
			pNameNode	= pChild2;
			i += 3;
		}
		else
		{
			throw Exception( "Broken variable declaration", pChild0->mpToken );
		}

		// no "space cast" in the declaration in the curl braces
		AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pNameNode );

		return true;
	}

	return false;
}

//==================================================================
static bool fndVarInExpr(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	*pChild2
		)
{
	if ( pChild0->mpToken->idType == T_TYPE_DATATYPE )
	{
		// 2 possible cases
		// 1) DT (+constructor params)
		// 2) DT CAST (+constructor params)

		// always varying..

		TokNode *pSpaceCast = NULL;

		if ((pChild1 && pChild1->mpToken->id == T_VL_STRING) )
		{
			pSpaceCast = pChild1;
			i += 1;
		}

		i += 1;

		AddVariable( pNode, pChild0, NULL, pSpaceCast, NULL );

		return true;
	}

	return false;
}

//==================================================================
static void discoverVariablesDeclarations( TokNode *pNode )
{
	// $$$ Note: should assume root to be one big curl bracket..
	if ( pNode->mpToken &&
		 pNode->mpToken->id != T_OP_LFT_CRL_BRACKET &&
		 pNode->mpToken->id != T_OP_LFT_BRACKET )
	{
		return;
	}

	if ( pNode->mpToken )
	{
		DASSERT(
			pNode->mBlockType == BLKT_SHPARAMS ||
			pNode->mBlockType == BLKT_CODEBLOCK ||
			pNode->mBlockType == BLKT_EXPRESSION
			);

		for (size_t i=0; i < pNode->mpChilds.size();)
		{
			TokNode	*pChild0 = pNode->mpChilds[i];
			TokNode	*pChild1 = NULL;
			TokNode	*pChild2 = NULL;

			if ( (i+1) < pNode->mpChilds.size() ) pChild1 = pNode->mpChilds[i + 1];
			if ( (i+2) < pNode->mpChilds.size() ) pChild2 = pNode->mpChilds[i + 2];

			if ( pNode->mBlockType == BLKT_CODEBLOCK ||
				 pNode->mBlockType == BLKT_SHPARAMS	// $$$ NOT REALLY THE SAME ..but for now, it's ok
				 )
			{
				if NOT( fndVarInBlock( i, pNode, pChild0, pChild1, pChild2 ) )
				{
					++i;
					continue;
				}
			}
			else
			if ( pNode->mBlockType == BLKT_EXPRESSION )
			{
				if NOT( fndVarInExpr( i, pNode, pChild0, pChild1, pChild2 ) )
				{
					++i;
					continue;
				}
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		discoverVariablesDeclarations( pNode->mpChilds[i] );
	}
}

/*
//==================================================================
static bool isVarUsedAsLValue( const char *pVarName, TokNode *pNode )
{
	for (TokNode *pNode2 = pNode; pNode2; pNode2 = pNode2->GetNext())
	{
	}

	// recurse on all children that are NOT code blocks (because
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		isVarUsedAsLValue( pVarName, pNode->mpChilds[i] );
	}
}
*/

//==================================================================
void discoverVariablesUsage( TokNode *pNode )
{
	// for every non-terminal
	if ( pNode->IsNonTerminal() )
	{
		// scan backward and up, looking for a variable definition matching the name
		for (TokNode *pVarDefBlock = pNode->mpParent; pVarDefBlock; pVarDefBlock = pVarDefBlock->mpParent)
		{
			// do we have variables declaration ?
			if ( pVarDefBlock->GetVars().size() )
			{
				Variable *pVar = pVarDefBlock->FindVariableByDefName( pNode->GetTokStr() );

				if ( pVar )
				{
					pNode->mpVarDef = pVar;
				}
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		discoverVariablesUsage( pNode->mpChilds[i] );
	}
}

//==================================================================
void DiscoverVariables( TokNode *pNode )
{
	discoverVariablesDeclarations( pNode );
	discoverVariablesUsage( pNode );
}

//==================================================================
}
