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
static bool doVTypesMatch(
				const Function	&func,
				const TokNode	&callNode,
				VarType			retCastVType,
				bool			laxTypes )
{
	if ( callNode.mpChilds.size() != func.mParamsVarTypes.size() )
		return false;

	// do we have a return cast value to match ?
	if ( retCastVType != VT_UNKNOWN )
	{
		if NOT( IsSameVarType( retCastVType, func.mRetVarType, laxTypes ) )
			return false;
	}

	for (size_t i=0; i < callNode.mpChilds.size(); ++i)
	{
		VarType	vtype = callNode.mpChilds[i]->GetVarType();

		DASSERT( vtype != VT_UNKNOWN );

		if NOT( IsSameVarType( vtype, func.mParamsVarTypes[i], laxTypes ) )
			return false;
	}

	return true;
}

//==================================================================
const Function *MatchFunctionByParams( TokNode *pFCallNode, const DVec<Function> &funcs )
{
	Function	*pFound = NULL;

	TokNode		*pFCallParams = pFCallNode->GetChildTry( 0 );

	VarType		retCastVType = pFCallNode->mFuncCall.mReturnCastVType;

	Function	*pBestMatch = NULL;
	size_t		convertMatches = 0;
	size_t		exactMatches = 0;

	// try exact matches first
	for (size_t i=0; i < funcs.size(); ++i)
	{
		if ( strcmp(
				pFCallNode->mpToken->GetStrChar(),
				funcs[i].mpNameNode->GetTokStr()
				) )
		{
			continue;
		}

		if ( pFCallParams )
		{
			// match number and types of params
			if NOT( doVTypesMatch( funcs[i], *pFCallParams, retCastVType, false ) )
				continue;
		}

		return &funcs[i];
	}

	// if there are no params (param-less funcop) then there
	// is no point in trying to match the function further
	// ..because there is no param matching to do 8)
	if NOT( pFCallParams )
	{
		return NULL;
	}

	// now try "lax" match.. where vector, point and normal are all considered the same
	// ..this is very likely how RSL works 8)
	for (size_t i=0; i < funcs.size(); ++i)
	{
		if ( strcmp(
				pFCallNode->mpToken->GetStrChar(),
				funcs[i].mpNameNode->GetTokStr()
				) )
		{
			continue;
		}

		// match number and types of params
		if ( pFCallParams )
		{
			if NOT( doVTypesMatch( funcs[i], *pFCallParams, retCastVType, true ) )
				continue;
		}

		return &funcs[i];
	}


	return NULL;
}

//==================================================================
/*
node1           node1    
	dest     	    =   
           ->  		  dest 
			 		  src
*/
static void insertAssignToNode( TokNode *pDestNode, const TokNode *pSrcNode )
{
	TokNode	*pAssgnNode = DNEW TokNode( "=", T_OP_ASSIGN, T_TYPE_OPERATOR );
	TokNode *pSrcClone = cloneBranch( pSrcNode );

	pAssgnNode->ReplaceNode( pDestNode );

	pAssgnNode->AddChild( pDestNode );
	pAssgnNode->AddChild( pSrcClone );
}

//==================================================================
static TokNode *insertAssignToVar(
						const VarLink	&destVLink,
						const TokNode	*pSrcNode )
{
	TokNode	*pAssgnNode = DNEW TokNode( "=", T_OP_ASSIGN, T_TYPE_OPERATOR );
	TokNode	*pSrcClone = cloneBranch( pSrcNode );
	
	TokNode	*pNewDest = DNEW TokNode(
								destVLink.GetVarPtr()->GetUseName().c_str(),
								T_TD_TEMPDEST,
								T_TYPE_TEMPDEST );
	pNewDest->mVarLink = destVLink;

	pAssgnNode->AddChild( pNewDest );
	pAssgnNode->AddChild( pSrcClone );

	return pAssgnNode;
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

		insertAssignToNode( pParamsHooks->mpChilds[i], pPassParam );
		//printf( "PASSSS %s = %s\n", pClonedParamsHooks->mpChilds[i]->GetTokStr(), pPassParam->GetTokStr() );

		// $$$ need to get the function params types from definition here..
	}

}

//==================================================================
// Kind of a nasty special case.. but what can you do ?
static void handleFuncopEndForIfElse( TokNode *pFuncParamsHooks )
{
	//pFuncParamsHooks->mpN

	TokNode	*pRight = pFuncParamsHooks->GetRight();

	if ( pRight )
	{
		pRight = pRight->GetRight();
		if ( pRight && 0 == strcmp( pRight->GetTokStr(), "else" ) )
		{
			pFuncParamsHooks->mOutputFuncOpEnd = false;
			return;
		}
	}

	pFuncParamsHooks->mOutputFuncOpEnd = true;
}


//==================================================================
static void resolveFunctionCalls(
						TokNode *pNode,
						const DVec<Function> &funcs )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolveFunctionCalls( pNode->mpChilds[i], funcs );
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		const Function	*pFunc = NULL;

		// bogus params ?
		for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		{
			VarType vtype = pNode->mpChilds[i]->GetVarType();
			if ( vtype == VT_UNKNOWN )
			{
				break;
			}
		}

		pFunc = MatchFunctionByParams( pNode, funcs );

		if ( pFunc )
		{
			TokNode	*pClonedParamsHooks = cloneBranch( pFunc->mpParamsNode );

			TokNode	*pFollowingStatement = NULL;
			if ( pFunc->mpRetTypeTok->id == T_KW___funcop )
			{
				pFollowingStatement = pNode->GetRight();
			}

			// place the params block where the function call (name) currently is
			pClonedParamsHooks->ReplaceNode( pNode );

			// don't add the return value for "void" functions !
			if ( pFunc->mRetVarType != VT_VOID )
			{
				// add a return node/variable
				AddVariable(
					pClonedParamsHooks->mpParent,
					DNEW TokNode( DNEW Token( *pFunc->mpRetTypeTok ) ),
					DNEW TokNode( "varying", T_DE_varying, T_TYPE_DETAIL ),	// %%% forced varying for now !
					NULL,
					pClonedParamsHooks );
			}

			const TokNode	*pPassParams = pNode->GetChildTry( 0 );

			assignPassingParams( pClonedParamsHooks, pPassParams );

			// a funcop ?
			if ( pFunc->mpRetTypeTok->id == T_KW___funcop )
			{
				// NOTE: do re really need to do this reparenting ??!

				if NOT( pFollowingStatement )
					throw Exception( "Missing statement !", pNode );

				pClonedParamsHooks->mIsFuncOp = true;

				handleFuncopEndForIfElse( pClonedParamsHooks );
			}
		}
		else
		{
			// not found ?
			const char *pFuncName = pNode->mpToken->GetStrChar();

			// is it an asm intrinsic ?
			if ( pFuncName != strstr( pFuncName, "_asm_" ) )
			{
				// if not, then throw an exception
				throw Exception(
							DUT::SSPrintFS( "Could not find the function: %s", pFuncName ),
							pNode );
			}
		}
	}
}

//==================================================================
static TokNode *instrumentFCallReturn( TokNode *pReturnNode, const VarLink &destVLink )
{
	TokNode	*pRetNode = pReturnNode->GetRight();

	TokNode	*pAssign = insertAssignToVar( destVLink, pRetNode );

	pAssign->ReplaceNode( pReturnNode );
	DSAFE_DELETE( pReturnNode );

	pRetNode->UnlinkFromParent();
	DSAFE_DELETE( pRetNode );

	return pAssign;
}

//==================================================================
/*
BLKT_FNPARAMS individuates the parameters.. however, after previous
processing, the first node in the params is he "return value" node
..there may not be one, so we have to be careful about the validity
of pFnParams->mVarLink

	(					-- BLKT_FNPARAMS
		{
			return
			+
				1
				2

	(->ReturnDest		-- BLKT_FNPARAMS
		{
			=
				ReturnDest
				+
					1
					2
*/
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

		// do we actually have any params in input ?
		if ( pFnParams->mVarLink.IsValid() )
		{
			pNode = instrumentFCallReturn( pReturnNode, pFnParams->mVarLink );

			// pNode points to the next node to be used to continue dig recursively
			pNode = pNode->GetChildTry( 1 );
			DASSERT( pNode != NULL );
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolveFunctionReturns( pNode->mpChilds[i], pAssignOpsToRemove );
	}
}

//==================================================================
void ResolveFunctionCalls( TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	resolveFunctionCalls( pNode, funcs );

	DVec<TokNode *>	pAssignOpsToRemove;
	resolveFunctionReturns( pNode, pAssignOpsToRemove );
}

//==================================================================
static void closeFuncOps_sub( TokNode *pNode )
{
	if ( pNode->mIsFuncOp && pNode->mOutputFuncOpEnd )
	{
		TokNode	*pFuncOpEndMarker = DNEW TokNode( "_asm_funcopend", T_NONTERM, T_TYPE_NONTERM );
		pFuncOpEndMarker->mNodeType = TokNode::TYPE_FUNCCALL;
		pNode->GetRight()->AddChild( pFuncOpEndMarker );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		closeFuncOps_sub( pNode->mpChilds[i] );
	}

}

//==================================================================
void CloseFuncOps( TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		// for every shader (should be just 1, maybe ?)
		if ( funcs[i].IsShader() )
		{
			closeFuncOps_sub( funcs[i].mpCodeBlkNode );
		}
	}
}

//==================================================================
}
