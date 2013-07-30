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
		if ( pChild->GetTokID() == T_OP_COMMA )
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
static void convertFCallParamsToExpressions( TokNode *pNode, u_int &io_blockCnt )
{
	if NOT( pNode->mpChilds.size() )
		return;

/* convert

func
	(
	p1
	,
	p2

..to..

func
	( <CALL PARAMS>
		( <EXPR>
			p1
		( <EXPR>
			p2
*/

	// set as a plain expression and return

	TokNode	*pParamExpr = DNEW TokNode( "(", T_OP_LFT_BRACKET, T_TYPE_OPERATOR, pNode );
	pNode->AddChildFront( pParamExpr );
	pParamExpr->SetBlockType( BLKT_EXPRESSION );
	pParamExpr->mBlockID = io_blockCnt++;

	for (size_t j=1; j < pNode->mpChilds.size();)
	{
		TokNode	*pChild = pNode->mpChilds[j];
		if ( pChild->GetTokID() == T_OP_COMMA )
		{
			pChild->mpToken->str = "(";
			pChild->SetTokID( T_OP_LFT_BRACKET );
			pChild->SetBlockType( BLKT_EXPRESSION );
			pChild->mBlockID = io_blockCnt++;
			pParamExpr = pChild;
			++j;
			continue;
		}

		TokNode	*pSrcParam = pNode->mpChilds[j];
		pSrcParam->Reparent( pParamExpr );
		pParamExpr->mpChilds.push_back( pSrcParam );

	}
}

//==================================================================
static const Function *getFunctionByName(
					const char *pFindName,
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
static bool isParamLessFuncop(
					const char *pFindName,
					const DVec<Function> &funcs )
{
	// TODO: will need to eventually support nested functions

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const char *pFuncName = funcs[i].mpNameNode->GetTokStr();

		if (	funcs[i].IsFuncOp()
			 && !funcs[i].HasParams()
			 && 0 == strcmp( pFuncName, pFindName ) )
		{
			return true;
		}
	}

	return false;
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
	if ( pRetType->GetTokID() == T_KW___funcop )
	{
		out_isFuncopDecl = true;
		out_retVarType = VT_VOID;	// funcops return nothing !
		return true;
	}
	else	// is ti a datatype instead ?
	if ( pRetType->GetTokIDType() == T_TYPE_DATATYPE )
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
					getFunctionByName( pFuncName->GetTokStr(), funcs );

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
								TokNode	*pDeclParamsBlk,
								DVec<Function> &funcs,
								u_int &io_blockCnt )
{
	if ( !pNode->IsNonTerminal() && !pNode->IsDataType() )
		return;

	TokNode	*pFuncName	= pNode;

	TokNode	*pRetType	= pNode->GetLeft();
	if NOT( pRetType )
		return;

	TokNode	*pCodeBlock	= pDeclParamsBlk->GetRight();
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
	if ( pDeclParamsBlk->GetBlockType() == BLKT_CALL_OR_DELC_PARAMS_FN )
		pDeclParamsBlk->SetBlockType( BLKT_DECL_PARAMS_FN );

	Function	*pFunc = funcs.grow();

	removeCommasFromParams( pDeclParamsBlk );
	//convertFCallParamsToExtressions( pParamsBlk, io_blockCnt );

	pFunc->mpParamsNode		= pDeclParamsBlk;
	pFunc->mpCodeBlkNode	= pCodeBlock;
	pFunc->mpNameNode		= pFuncName;
	pFunc->mpRetTypeTok		= pRetType->mpToken;
	pFunc->mRetVarType		= retVarType;
	pFunc->mRetVarIsArray	= false; // false for now, and probably forever 8)

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
			pFunc->mParamsVarIsArray.push_back( pParamNode->IsVarArray() );
		}
	}
}

//==================================================================
static void discoverFuncsCalls_sub(
						TokNode					*pFuncCallNode,
						TokNode					*pSpaceCastNode,
						TokNode					*pCallParamsBlk,
						const DVec<Function>	&funcs,
						u_int					&io_blockCnt,
						int						&out_parentIdx )
{
	if ( pSpaceCastNode )
	{
		/*
			func
			"dude"
			(
			a
			)
					..becomes..
			func
			(
			"dude"
			,
			a
			)
		*/

		if ( pCallParamsBlk->mpChilds.size() )
		{
			TokNode	*pCommaNode = DNEW TokNode( ",", T_OP_COMMA, T_TYPE_OPERATOR, pSpaceCastNode );
			pCallParamsBlk->AddChildFront( pCommaNode );
		}

		pSpaceCastNode->Reparent( pCallParamsBlk );
		pCallParamsBlk->mpChilds.push_front( pSpaceCastNode );
	}

	// do an early match to see if there is at least one function call
	// with this name  ..actual match will happen later in resolveFunctionCalls()
	// ..because it's more complex as it needs to resolve ambiguities stemming from
	// freaking function overloading.. which is nice but complicates things
	bool isFuncOp = false;

	// if it doesn't start by "_asm_"
	if NOT( IsAsmFunc( pFuncCallNode->GetTokStr() ) )
	{
		const Function *pFunc =
				getFunctionByName(
						pFuncCallNode->GetTokStr(),
						funcs );

		if NOT( pFunc )
			return;

		// see if it's a funcop
		if ( pFunc->mpRetTypeTok->id == T_KW___funcop )
			isFuncOp = true;
	}

	if ( pCallParamsBlk->GetBlockType() == BLKT_CALL_OR_DELC_PARAMS_FN )
	{
		pCallParamsBlk->SetBlockType( BLKT_CALL_PARAMS_FN );

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

		pCallParamsBlk->Reparent( pFuncCallNode );
		pFuncCallNode->mpChilds.push_back( pCallParamsBlk );

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;

		convertFCallParamsToExpressions( pCallParamsBlk, io_blockCnt );
	}
	else
	if ( isFuncOp )
	{
		// funcops with no params can be called without the brackets
		// ..for example "else()" is a funcop and it can be called as "else"
		// Note: funcops such as "else" and "if" are defined in RSLC_Builtins.sl 

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;

		convertFCallParamsToExpressions( pCallParamsBlk, io_blockCnt );
	}
}

//==================================================================
static void discoverFuncsDecls(
							TokNode *pNode,
							DVec<Function> &funcs,
							u_int &io_blockCnt )
{
	TokNode	*pDeclParamsBlk	= pNode->GetRight();

	// do we have a potential params block
	if ( pDeclParamsBlk )
	{
		// does it look like a params block ?
		switch ( pDeclParamsBlk->GetBlockType() )
		{
		case BLKT_DECL_PARAMS_SH:
		case BLKT_DECL_PARAMS_FN:
		case BLKT_CALL_OR_DELC_PARAMS_FN:
			discoverFuncsDeclarations_sub( pNode, pDeclParamsBlk, funcs, io_blockCnt );
			break;
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		discoverFuncsDecls( pNode->mpChilds[i], funcs, io_blockCnt );
}

//==================================================================
static void discoverFuncsCalls(
							TokNode *pNode,
							const DVec<Function> &funcs,
							u_int &io_blockCnt,
							int &out_parentIdx )
{
	// is it a potential function name ?
	if ( pNode->IsNonTerminal() || pNode->IsDataType() )
	{
		TokNode	*pRightNode	= pNode->GetRight();

		// do we have a potential params block
		if ( pRightNode )
		{
			TokNode	*pCallParamsBlk = NULL;
			TokNode	*pSpaceCastNode = NULL;

			// space cast ?
			if ( pRightNode->GetTokID() == T_VL_STRING )
			{
				pSpaceCastNode = pRightNode;
				pCallParamsBlk = pRightNode->GetRight();
			}
			else
				pCallParamsBlk = pRightNode;

			// does it look like a params block ?
			switch ( pCallParamsBlk->GetBlockType() )
			{
			case BLKT_CALL_PARAMS_FN:
			case BLKT_CALL_OR_DELC_PARAMS_FN:
				discoverFuncsCalls_sub(
							pNode,
							pSpaceCastNode,
							pCallParamsBlk,
							funcs,
							io_blockCnt,
							out_parentIdx );
				break;

			default:
				if (	pNode->mNodeType == TokNode::TYPE_STANDARD
					 && isParamLessFuncop( pNode->GetTokStr(), funcs ) )
				{
					pNode->mNodeType = TokNode::TYPE_FUNCCALL;
				}
				break;
			}
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
		discoverFuncsCalls( pNode->mpChilds[i], funcs, io_blockCnt, i );
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot, u_int &io_blockCnt )
{
	discoverFuncsDecls( pRoot, pRoot->GetFuncs(), io_blockCnt );

	int	idx = 0;
	discoverFuncsCalls( pRoot, pRoot->GetFuncs(), io_blockCnt, idx );
}

//==================================================================
}
