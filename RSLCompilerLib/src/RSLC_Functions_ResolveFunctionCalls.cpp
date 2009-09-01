//==================================================================
/// RSLC_Functions_ResolveFunctionCalls.cpp
///
/// Created by Davide Pasca - 2009/9/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <map>
#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static TokNode *cloneBranch_BuildNodes(
								TokNode *pNode,
								TokNode *pDstParent,
								std::map<TokNode*,TokNode*>	&oldToNewMap )
{
	TokNode	*pNewNode = DNEW TokNode( *pNode );

	oldToNewMap[pNode] = pNewNode;

	if ( pDstParent )
		pDstParent->AddChild( pNewNode );

	if ( pNewNode->mVarLink.IsValid() )
	{
		TokNode	*pReplaceNode = oldToNewMap[ pNode->mVarLink.mpNode ];

		if ( pReplaceNode )
		{
			DASSERT( pNode->mVarLink.mpNode->GetVars().size() != 0 );
			DASSERT( pNode->mVarLink.mpNode->GetVars().size() == pReplaceNode->GetVars().size() );

			pNewNode->mVarLink.mpNode = pReplaceNode;
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		cloneBranch_BuildNodes( pNode->mpChilds[i], pNewNode, oldToNewMap );
	}

	return pNewNode;
}

//==================================================================
static TokNode *cloneBranch( TokNode *pNode )
{
	std::map<TokNode*,TokNode*>	oldToNewMap;

	TokNode	*pNewBase = cloneBranch_BuildNodes( pNode, NULL, oldToNewMap );

	return pNewBase;
}

//==================================================================
static const Function *matchFunctionByParams( TokNode *pFCallNode, const DVec<Function> &funcs )
{
	Function	*pFound = NULL;

	TokNode *pFCallParams = pFCallNode->GetChildTry( 0 );
	DASSERT( pFCallParams != NULL );

	//for (size_t i=0; i < funcs.size(); ++i)
	{
		TokNode	*pCallParams = pFCallNode->GetChildTry( 0 );
		for (size_t j=0; j < pCallParams->mpChilds.size(); ++j)
		{
			const TokNode	*pParam = pCallParams->mpChilds[j];
			// assignment to temp register case
			if ( pParam->mpToken->id == T_OP_ASSIGN )
			{
				const TokNode	*pParamVal = pParam->GetChildTry(0);
				// first child is the destination..
				if ( pParamVal->mpToken->id == T_TD_TEMPDEST )
				{
					printf( "Param %i, is '%s'\n",
								j,
								VarTypeToString( pParamVal->GetVarType() )
							);
				}
			}
		}
	}

	Function	*pBestMatch = NULL;
	size_t		convertMatches = 0;
	size_t		exactMatches = 0;

	for (size_t i=0; i < funcs.size(); ++i)
	{
		if ( strcasecmp(
				pFCallNode->mpToken->GetStrChar(),
				funcs[i].mpNameNode->GetTokStr()
				) )
			continue;

		return &funcs[i];
	}

	return NULL;
}

//==================================================================
/*
dest

---------

=
	dest
	src
*/
static void insertAssignTo( TokNode *pDestNode, TokNode *pSrcNode )
{
	TokNode	*pAssgnNode = DNEW TokNode( DNEW Token( "=", T_OP_ASSIGN, T_TYPE_OPERATOR ) );

	TokNode *pCloneSrcNode = cloneBranch( pSrcNode );

	pAssgnNode->ReplaceNode( pDestNode );

	pAssgnNode->AddChild( pDestNode );
	pAssgnNode->AddChild( pCloneSrcNode );
}

/*
//==================================================================
static void insertAssignToTemp( TokNode *pNode, size_t childIdx, VarType varType, bool isVarying )
{
	Token	*pDestRegTok = DNEW Token();
	Token	*pAssgnOpTok = DNEW Token();

	pDestRegTok->idType	= T_TYPE_TEMPDEST;
	pDestRegTok->id		= T_TD_TEMPDEST;
	pDestRegTok->str	= "TEMP_DEST_REG";

	pAssgnOpTok->idType = T_TYPE_OPERATOR;
	pAssgnOpTok->id		= T_OP_ASSIGN;
	pAssgnOpTok->str	= "=";

	TokNode	*pAssgnNode = DNEW TokNode( pAssgnOpTok );

	TokNode	*pOldNode = pNode->mpChilds[childIdx];
	pNode->mpChilds[childIdx] = pAssgnNode;
	pAssgnNode->mpParent = pNode;

	TokNode *pDestRegNode = pAssgnNode->AddNewChild( pDestRegTok );
	pDestRegNode->mBuild_TmpReg.SetType( varType, isVarying );

	pAssgnNode->AddChild( pOldNode );
}
*/

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
static void assignPassingParams( TokNode *pClonedParams, TokNode *pPassParams )
{
	for (size_t i=0,j=0; i < pClonedParams->mpChilds.size(); ++i)
	{
		if ( pClonedParams->mpChilds[i]->mpToken->id != T_NONTERM )
			continue;

		TokNode	*pPassParam = pPassParams->GetChildTry( j++ );

		if NOT( pPassParam )
			throw Exception( "Missing parameter ?", pPassParams );

		insertAssignTo( pClonedParams->mpChilds[i], pPassParam );
		//printf( "PASSSS %s = %s\n", pClonedParams->mpChilds[i]->GetTokStr(), pPassParam->GetTokStr() );

		// $$$ need to get the function params types from definition here..
	}

}

//==================================================================
static void resolveFunctionCalls( TokNode *pNode, const DVec<Function> &funcs, size_t &parentIdx )
{
	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		const Function	*pFunc = matchFunctionByParams( pNode, funcs );

		if ( pFunc )
		{
			TokNode	*pClonedParams = cloneBranch( pFunc->mpParamsNode );

			pClonedParams->ReplaceNode( pNode );

			// return node
			AddVariable(
				pClonedParams->mpParent,
				DNEW TokNode( DNEW Token( *pFunc->mpRetTypeTok ) ),
				DNEW TokNode( DNEW Token( "varying", T_DE_varying, T_TYPE_DETAIL ) ),	// %%% forced varying for now !
				NULL,
				pClonedParams );

			TokNode	*pPassParams = pNode->GetChildTry( 0 );

			assignPassingParams( pClonedParams, pPassParams );

			DSAFE_DELETE( pNode );
			//return;

			pNode = pClonedParams;
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolveFunctionCalls( pNode->mpChilds[i], funcs, i );
	}
}

//==================================================================
static TokNode *insertAssignToNode(
						const VarLink	&destVLink,
						TokNode	*pSrcNode )
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
static TokNode *instrumentFCallReturn( TokNode *pReturnNode, const VarLink	&destVLink )
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
