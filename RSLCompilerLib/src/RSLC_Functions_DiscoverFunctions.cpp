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
static const Function *getFunctionByName(
					const char *pFindName,
					const TokNode *pNode,
					const DVec<Function> &funcs )
{
	// TODO: will need to eventually support nested functions
	// ..for the time being we just use the flat list 'funcs' that is
	// kept in the root node

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const char *pFuncName = funcs[i].mpNameNode->GetTokStr();
		if ( 0 == strcmp( pFuncName, pFindName ) )
			return &funcs[i];
	}

	return NULL;
}

//==================================================================
static bool handleRetValueForFuncDeclaration(
							TokNode	*pFuncName,
							TokNode	*pRetType,
							TokNode *pNode,
							DVec<Function> &funcs,
							VarType &out_retVarType,
							bool &out_isFuncopDecl
							)
{
	out_retVarType		= VT_UNKNOWN;
	out_isFuncopDecl	= false;

	// is it a funcop ?
	if ( pRetType->mpToken->id == T_KW___funcop )
	{
		out_isFuncopDecl = true;
		out_retVarType = VT_VOID;	// funcops return nothing !
		return true;
	}
	else	// is ti a datatype instead ?
	if ( pRetType->mpToken->idType == T_TYPE_DATATYPE )
	{
		// something properly specified ?
		out_retVarType = VarTypeFromToken( pRetType->mpToken );
		return true;
	}
	else
	{
		// Ambiguity !
		//  ..is this a nested function with no return value or is it a
		//  funcop call such as "illuminance()" ?
		const Function *pFoundFunc =
					getFunctionByName( pFuncName->GetTokStr(), pNode, funcs );

		if ( pFoundFunc )
		{
			if ( pFoundFunc->mpRetTypeTok->id == T_KW___funcop )
			{
				// yes, it is a funcop _call_, so it's not a function declaration
				return false;
			}
			else
			{
				throw Exception(
						pFuncName,
						"Invalid function call of '%s',"
						" missing a semicolon somewhere ?",
							pFuncName->GetTokStr() );
			}
		}
		else
		{
			// ..otherwise assume it's a (nested) function declaration but with
			// no return value
			out_retVarType = VT_FLOAT;
			return true;
		}
	}
}

//==================================================================
static void discoverFuncsDeclarations_sub(
								TokNode *pNode,
								DVec<Function> &funcs )
{
	if ( !pNode->IsNonTerminal() && !pNode->IsDataType() )
		return;

	TokNode	*pFuncName	= pNode;
	TokNode	*pParamsBlk	= pNode->GetRight();

	// do we have a potential params block
	if ( !pParamsBlk )
		return;

	// does it look like a params block ?
	switch ( pParamsBlk->GetBlockType() )
	{
	case BLKT_DECL_PARAMS_SH:
	case BLKT_DECL_PARAMS_FN:
	case BLKT_CALL_OR_DELC_PARAMS_FN:
		break;

	default:
		return;
	}

	TokNode	*pRetType	= pNode->GetLeft();
	if NOT( pRetType )
		return;

	TokNode	*pCodeBlock	= pParamsBlk->GetRight();
	if ( !pCodeBlock || !pCodeBlock->IsCodeBlock() )
		return;

	// see if this is a function declaration and what's the return value and
	// whether or not it's a funcop kind of call ("illuminance()", etc.)
	VarType	retVarType;
	bool	isFuncopDecl;
	if NOT(
		handleRetValueForFuncDeclaration(
						pFuncName,
						pRetType,
						pNode,
						funcs,
						retVarType,
						isFuncopDecl ) )
		return;

	// if it is a function declaration, then proceed

	Function	*pFunc = funcs.grow();

	removeCommasFromParams( pParamsBlk );

	pFunc->mpParamsNode		= pParamsBlk;
	pFunc->mpCodeBlkNode	= pCodeBlock;
	pFunc->mpNameNode		= pFuncName;
	pFunc->mpRetTypeTok		= pRetType->mpToken;
	pFunc->mRetVarType		= retVarType;

	// params block becomes children of the function body:
	// pFuncName			  pFuncName
	//           (PAR	 ->			    {CODE
	//             {CODE				  (PAR
	//
	pFunc->mpCodeBlkNode->Reparent( pFunc->mpParamsNode );
	pFunc->mpParamsNode->AddChild( pFunc->mpCodeBlkNode );

	pFuncName->mNodeType = TokNode::TYPE_FUNCDEF;	// mark the node as a function definition
	pFuncName->mIsFuncOp = isFuncopDecl;			// finally set this

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

//==================================================================
static void discoverFuncsDeclarations(
								TokNode *pNode,
								DVec<Function> &funcs )
{
	discoverFuncsDeclarations_sub( pNode, funcs );

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		discoverFuncsDeclarations( pNode->mpChilds[i], funcs );
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
		const Function *pFunc =
				getFunctionByName(
						pFuncCallNode->GetTokStr(),
						pFuncCallNode,
						funcs );

		if NOT( pFunc )
			return;

		// see if it's a funcop
		if ( pFunc->mpRetTypeTok->id == T_KW___funcop )
			isFuncOp = true;
	}

	if ( pRightNode->GetBlockType() == BLKT_CALL_OR_DELC_PARAMS_FN )
	{
		pRightNode->SetBlockType( BLKT_CALL_PARAMS_FN );

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

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;

		removeCommasFromParams( pRightNode );
	}
	else
	if ( isFuncOp )
	{
		// funcops with no params can be called without the brackets
		// ..for example "else()" is a funcop and it can be called as "else"
		// Note: funcops sch as "else" and "if" are defined in RSLC_Builtins.sl 

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;

		removeCommasFromParams( pRightNode );
	}
}

//==================================================================
static void discoverFuncsCalls( TokNode *pNode, const DVec<Function> &funcs, int &out_parentIdx )
{
	if ( (pNode->IsNonTerminal() ||
		  pNode->IsDataType()) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		discoverFuncsCalls_sub( pNode, funcs, out_parentIdx );
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
		discoverFuncsCalls( pNode->mpChilds[i], funcs, i );
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot, pRoot->GetFuncs() );

	int	idx = 0;
	discoverFuncsCalls( pRoot, pRoot->GetFuncs(), idx );
}

//==================================================================
}
