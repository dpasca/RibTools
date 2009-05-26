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
	Variable	*pVar = pNode->GetData().mVariables.grow();

	pVar->mpDTypeTok		= pDTypeNode->mpToken;
	pVar->mpDetailTok		= pDetailNode ? pDetailNode->mpToken : NULL;
	pVar->mpSpaceCastTok	= pSpaceCastTok ? pSpaceCastTok->mpToken : NULL;
	pVar->mpNameTok			= pNameNode ? pNameNode->mpToken : NULL;

	if ( pVar->mpNameTok )
	{
		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_%s",
							pNode->mBlockID,
								pVar->mpNameTok->str.c_str() );
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
		// 3 possible cases
		// 1) TYPE UNK
		// 2) TYPE DETAIL UNK
		// 3) DETAIL TYPE UNK

		TokNode	*pDTypeNode = NULL;
		TokNode	*pDetailNode = NULL;
		TokNode	*pNameNode = NULL;

		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= NULL;
			pNameNode	= pChild1;
			i += 2;
		}
		else
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild1;
			pNameNode	= pChild2;
			i += 3;
		}
		else
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_NONTERM) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild2;
			pNameNode	= pChild1;
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
void DiscoverVariables( TokNode *pNode )
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
		DiscoverVariables( pNode->mpChilds[i] );
	}
}

//==================================================================
}
