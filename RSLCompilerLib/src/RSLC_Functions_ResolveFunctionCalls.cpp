//==================================================================
/// RSLC_Functions_ResolveFunctionCalls.cpp
///
/// Created by Davide Pasca - 2009/9/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <hash_map>
#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

typedef stdext::hash_map<const TokNode*,TokNode*>	NodeToNodeMap;

//==================================================================
static TokNode *cloneBranch_BuildNodes(
								const TokNode *pNode,
								TokNode *pDstParent,
								NodeToNodeMap &oldToNewMap )
{
	TokNode	*pNewNode = DNEW TokNode( *pNode );

	oldToNewMap[pNode] = pNewNode;

	if ( pDstParent )
		pDstParent->AddChild( pNewNode );

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		cloneBranch_BuildNodes( pNode->mpChilds[i], pNewNode, oldToNewMap );
	}

	return pNewNode;
}

//==================================================================
static void cloneBranch_RemapVarlinks(
								TokNode *pNode,
								NodeToNodeMap &oldToNewMap )
{
	if ( pNode->mVarLink.IsValid() )
	{
		const TokNode *pVarsNode = pNode->mVarLink.GetNode();

		if ( oldToNewMap.find( pVarsNode ) != oldToNewMap.end() )
		{
			TokNode	*pReplaceNode = oldToNewMap[ pVarsNode ];
	
			DASSERT( pVarsNode->GetVars().size() != 0 );
			DASSERT( pVarsNode->GetVars().size() == pReplaceNode->GetVars().size() );

			pNode->mVarLink.ReplaceNode( pReplaceNode );
		}
		else
		{
			//DASSERT( 0 );
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		cloneBranch_RemapVarlinks( pNode->mpChilds[i], oldToNewMap );
	}
}

//==================================================================
static TokNode *cloneBranch( const TokNode *pNode )
{
	NodeToNodeMap	oldToNewMap;

	TokNode	*pNewBase = cloneBranch_BuildNodes( pNode, NULL, oldToNewMap );

	cloneBranch_RemapVarlinks( pNewBase, oldToNewMap );


	return pNewBase;
}

//==================================================================
static bool doVTypesMatch( const Function &func, const TokNode &callNode )
{
	if ( callNode.mpChilds.size() != func.mParamsVarTypes.size() )
		return false;

	for (size_t i=0; i < callNode.mpChilds.size(); ++i)
	{
		VarType	vtype = callNode.mpChilds[i]->GetVarType();

		if ( func.mParamsVarTypes[i] != vtype )
			return false;
	}

	return true;
}

//==================================================================
static const Function *matchFunctionByParams( TokNode *pFCallNode, const DVec<Function> &funcs )
{
	Function	*pFound = NULL;

	TokNode *pFCallParams = pFCallNode->GetChildTry( 0 );
	DASSERT( pFCallParams != NULL );

	Function	*pBestMatch = NULL;
	size_t		convertMatches = 0;
	size_t		exactMatches = 0;

	for (size_t i=0; i < funcs.size(); ++i)
	{
		if ( strcmp(
				pFCallNode->mpToken->GetStrChar(),
				funcs[i].mpNameNode->GetTokStr()
				) )
		{
			continue;
		}

		// match number of params
		if NOT( doVTypesMatch( funcs[i], *pFCallParams ) )
			continue;

		return &funcs[i];
	}

	return NULL;
}

//==================================================================
static void instrumentFuncsCallsParams( TokNode *pNode, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		TokNode	*pParamsNode = pNode->GetChildTry( 0 );

		// skip eventual space-cast string
		if ( pParamsNode && pParamsNode->mpToken->id == T_VL_STRING )
		{
			pParamsNode = pNode->GetChildTry( 1 );
		}

		// do we have params ?
		if ( pParamsNode )
		{
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		instrumentFuncsCallsParams( pNode->mpChilds[i], i );
	}
}

//==================================================================
/*
dest

---------

=
	dest
	src
*/
static void insertAssignTo( TokNode *pDestNode, const TokNode *pSrcNode )
{
	TokNode	*pAssgnNode = DNEW TokNode( DNEW Token( "=", T_OP_ASSIGN, T_TYPE_OPERATOR ) );

	TokNode *pCloneSrcNode = cloneBranch( pSrcNode );

	pAssgnNode->ReplaceNode( pDestNode );

	pAssgnNode->AddChild( pDestNode );
	pAssgnNode->AddChild( pCloneSrcNode );
}

//==================================================================
static void assignPassingParams( TokNode *pParamsHooks, const TokNode *pPassParams )
{
	for (size_t i=0,j=0; i < pParamsHooks->mpChilds.size(); ++i)
	{
		if ( pParamsHooks->mpChilds[i]->mpToken->id != T_NONTERM )
			continue;

		TokNode	*pPassParam = pPassParams->GetChildTry( j++ );

		if NOT( pPassParam )
			throw Exception( "Missing parameter ?", pPassParams );

		insertAssignTo( pParamsHooks->mpChilds[i], pPassParam );
		//printf( "PASSSS %s = %s\n", pClonedParamsHooks->mpChilds[i]->GetTokStr(), pPassParam->GetTokStr() );

		// $$$ need to get the function params types from definition here..
	}

}

//==================================================================
#ifdef _DEBUG
static void verifyVarLinks( TokNode *pNode )
{
	DASSERT( pNode->mVarLink.IsNodeValid() );

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		verifyVarLinks( pNode->mpChilds[i] );
	}
}
#endif

//==================================================================
static void resolveFunctionCalls( TokNode *pNode, const DVec<Function> &funcs, size_t &parentIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolveFunctionCalls( pNode->mpChilds[i], funcs, i );
		//if NOT( resolveFunctionCalls( pNode->mpChilds[i], funcs, i ) )
		//	return false;
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
#ifdef _DEBUG
		TokNode *pParent = pNode->mpParent;
#endif

		const Function	*pFunc = matchFunctionByParams( pNode, funcs );

		if ( pFunc )
		{
			TokNode	*pClonedParamsHooks = cloneBranch( pFunc->mpParamsNode );

			// place the params block where the function call (name) currently is
			pClonedParamsHooks->ReplaceNode( pNode );

			// add a return node/variable
			AddVariable(
				pClonedParamsHooks->mpParent,
				DNEW TokNode( DNEW Token( *pFunc->mpRetTypeTok ) ),
				DNEW TokNode( DNEW Token( "varying", T_DE_varying, T_TYPE_DETAIL ) ),	// %%% forced varying for now !
				NULL,
				pClonedParamsHooks );

			const TokNode	*pPassParams = pNode->GetChildTry( 0 );

			assignPassingParams( pClonedParamsHooks, pPassParams );

/*
		#ifdef _DEBUG
			verifyVarLinks( pParent );
		#endif
			DSAFE_DELETE( pNode );
		#ifdef _DEBUG
			verifyVarLinks( pParent );
		#endif
*/
			//return;
			//pNode = pClonedParamsHooks;

			//return false;
		}
	}

	//return true;
}

//==================================================================
static TokNode *insertAssignToNode(
						const VarLink	&destVLink,
						const TokNode	*pSrcNode )
{
	TokNode	*pAssgnNode = DNEW TokNode(
								DNEW Token( "=", T_OP_ASSIGN, T_TYPE_OPERATOR )
								);

	TokNode	*pNewDest = DNEW TokNode( DNEW Token( "ReturnDest", T_TD_TEMPDEST, T_TYPE_TEMPDEST ) );

	pNewDest->mVarLink = destVLink;

	TokNode	*pSrcClone = cloneBranch( pSrcNode );

	pAssgnNode->AddChild( pNewDest );
	pAssgnNode->AddChild( pSrcClone );

	return pAssgnNode;
}

//==================================================================
static TokNode *instrumentFCallReturn( TokNode *pReturnNode, const VarLink &destVLink )
{
	TokNode	*pRetNode = pReturnNode->GetRight();

	TokNode	*pAssign = insertAssignToNode( destVLink, pRetNode );

	pAssign->ReplaceNode( pReturnNode );
	DSAFE_DELETE( pReturnNode );

	pRetNode->UnlinkFromParent();
	DSAFE_DELETE( pRetNode );

	return pAssign;
}

//==================================================================
static void resolveFunctionReturns( TokNode *pNode, DVec<TokNode *> &pAssignOpsToRemove )
{
	if ( pNode->mpToken && pNode->mpToken->id == T_KW_return )
	{
		TokNode *pReturnNode = pNode;

		TokNode	*pFnParams = pReturnNode;	// start from "return"
		while ( pFnParams )
		{
			if ( pFnParams->GetBlockType() == BLKT_FNPARAMS )
			{
				break;
			}
			pFnParams = pFnParams->mpParent;
		}
		DASSERT( pFnParams->GetBlockType() == BLKT_FNPARAMS );

		pNode = instrumentFCallReturn( pReturnNode, pFnParams->mVarLink );

		// pNode points to the next node to be used to continue dig recursively
		pNode = pNode->GetChildTry( 1 );
		DASSERT( pNode != NULL );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolveFunctionReturns( pNode->mpChilds[i], pAssignOpsToRemove );
	}
}

/*
	(
		{
			return
			+
				1
				2

	(->ReturnDest
		{
			=
				ReturnDest
				+
					1
					2
*/

//==================================================================
void ResolveFunctionCalls( TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	size_t	curIdx = 0;
	resolveFunctionCalls( pNode, funcs, curIdx );

	DVec<TokNode *>	pAssignOpsToRemove;
	resolveFunctionReturns( pNode, pAssignOpsToRemove );
}


//==================================================================
}
