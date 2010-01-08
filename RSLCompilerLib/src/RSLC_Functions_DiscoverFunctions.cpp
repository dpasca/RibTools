//==================================================================
/// RSLC_Functions_DiscoverFunctions.cpp
///
/// Created by Davide Pasca - 2009/9/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void removeCommasFromParams( TokNode *pNode )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode	*pChild = pNode->mpChilds[i];
		if ( pChild->mpToken->id == T_OP_COMMA )
		{
			if ( pChild->mpChilds.size() != 0 )
			{
				throw Exception( "Broken function definition",  pNode );
			}

			pChild->UnlinkFromParent();
			DSAFE_DELETE( pChild );
			--i;
		}
	}
}

//==================================================================
static void discoverFuncsDeclarations( TokNode *pRoot )
{
	DVec<Function>	&funcs = pRoot->GetFuncs();

	for (size_t i=0; i < pRoot->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pRoot->mpChilds[i];

		if NOT( pNode->IsCodeBlock() )
			continue;

		TokNode	*pParamsBlk	= NULL;
		TokNode	*pFuncName	= NULL;
		TokNode	*pRetType	= NULL;

		if ( !(pParamsBlk	= pRoot->GetChildTry(i-2)) ||
			 !(pFuncName	= pRoot->GetChildTry(i-3)) ||
			 !(pRetType		= pRoot->GetChildTry(i-4)) )
		{
			throw Exception( "Broken function definition",  pNode->mpToken );
		}

		Function	*pFunc = funcs.grow();

		removeCommasFromParams( pParamsBlk );

		pFunc->mpParamsNode		= pParamsBlk;
		pFunc->mpCodeBlkNode	= pNode;
		pFunc->mpNameNode		= pFuncName;
		pFunc->mpRetTypeTok		= pRetType->mpToken;

		// params block becomes children of the function body
		pFunc->mpCodeBlkNode->Reparent( pFunc->mpParamsNode );
		pFunc->mpParamsNode->AddChild( pFunc->mpCodeBlkNode );

		i -= 1;

		pFuncName->mNodeType = TokNode::TYPE_FUNCDEF;	// mark the node as a function definition

		// is it a funcop ?
		if ( pRetType->mpToken->id == T_KW___funcop )
		{
			pFuncName->mIsFuncOp = true;
		}

		// what's the return value ?
		if ( pRetType->mpToken->idType == T_TYPE_DATATYPE )
		{
			// something properly specified ?
			pFunc->mRetVarType = VarTypeFromToken( pRetType->mpToken );
		}
		else
		{
			// otherwise it's just "void"
			pFunc->mRetVarType = VT_VOID;
		}

		for (size_t j=0; j < pFunc->mpParamsNode->mpChilds.size(); ++j)
		{
			const TokNode	*pParamNode = pFunc->mpParamsNode->mpChilds[j];
			const Token		*pParamTok = pParamNode->mpToken;

			if ( pParamTok->idType == T_TYPE_NONTERM )
			{
				DASSERT( pParamNode->mVarLink.IsValid() );

				pFunc->mParamsVarTypes.push_back( pParamNode->GetVarType() );
			}
		}
	}
}

//==================================================================
static void discoverFuncsCalls_sub(
						TokNode				 *pFuncCallNode,
						const DVec<Function> &funcs,
						int					 &out_parentIdx )
{
	// constructor ?
	bool	isDataType = pFuncCallNode->IsDataType();

	TokNode *pRightNode = pFuncCallNode->GetRight();

	if ( pRightNode && pRightNode->mpToken->id == T_VL_STRING )
	{
		if ( isDataType )
		{
			// ok, it's just a space cast..

			// ..next should be a bracket
			TokNode *pParamsList = pRightNode->GetRight();

			pRightNode->Reparent( pParamsList );
			pParamsList->mpChilds.push_front( pRightNode );

			pRightNode = pParamsList;
/*
			// ..reparent the cast string as child of the function call node
			out_parentIdx -= 1;

			pRightNode->Reparent( pFuncCallNode );
			pFuncCallNode->mpChilds.push_back( pRightNode );
*/
		}
		else
		{
			throw Exception( "Unknown usage, why a string ?",  pFuncCallNode );
		}
	}

	// should have something following because it has to be one of the following case
	// - an actual function call, in which case there should be an open bracket
	// - a funcop such as "else", with no params, but that should have a following
	//   statement or block
	if NOT( pRightNode )
	{
		return;
		//throw Exception( "Incomplete statement", pFuncCallNode );
	}

	//if ( pFunc->mpRetTypeTok->id == T_KW___funcop )

	// do an early match to see if there is at least one function call
	// with this name  ..actual match will happen later in resolveFunctionCalls()
	// ..because it's more complex as it needs to resolve ambiguities stemming from
	// freaking function overloading.. which is nice but complicates things
	bool isFuncOp = false;

	// if it doesn't start by "_asm_"
	if ( pFuncCallNode->GetTokStr() != strstr( pFuncCallNode->GetTokStr(), "_asm_" ) )
	{
		bool functionNameExists = false;
	
		for (size_t i=0; i < funcs.size(); ++i)
		{
			if ( 0 == strcmp(
						pFuncCallNode->GetTokStr(),
						funcs[i].mpNameNode->GetTokStr() ) )
			{
				functionNameExists = true;
				if ( funcs[i].mpRetTypeTok->id == T_KW___funcop )
					isFuncOp = true;

				break;
			}
		}

		if NOT( functionNameExists )
			return;
	}


	if ( pRightNode->mpToken->id == T_OP_LFT_BRACKET )
	{
		// set the block type as a function call
		pRightNode->UpdateBlockTypeToFuncCall();

		/*
			b
			(
			)

			..becomes..

			b
				(
				)

			..the parent loop will skip one, so we need to backpedal..
		*/

		out_parentIdx -= 1;

		pRightNode->Reparent( pFuncCallNode );
		pFuncCallNode->mpChilds.push_back( pRightNode );

		removeCommasFromParams( pRightNode );

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
	}
	else
	if ( isFuncOp )
	{
		// funcops with no params can be called without the brackets
		// ..for example "else()" is a funcop and it can be called as "else"
		// Note: funcops sch as "else" and "if" are defined in RSLC_Builtins.sl 

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
	}
}

//==================================================================
static void discoverFuncsCalls( TokNode *pNode, const DVec<Function> &funcs, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

/*
	if ( !pNode->IsParentRoot() &&  // parent cannot be root !!
		 (pNode->IsNonTerminal() ||
		  pNode->IsStdFunction() ||
		  isDataType) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
*/

	if ( (pNode->IsNonTerminal() ||
		  isDataType) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		discoverFuncsCalls_sub( pNode, funcs, out_parentIdx );
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		discoverFuncsCalls( pNode->mpChilds[i], funcs, i );
	}
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot );

	int	idx = 0;
	discoverFuncsCalls( pRoot, pRoot->GetFuncs(), idx );
}

//==================================================================
}
