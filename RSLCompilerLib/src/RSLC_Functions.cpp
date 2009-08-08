//==================================================================
/// RSLC_Functions.cpp
///
/// Created by Davide Pasca - 2009/5/30
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <map>
#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

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
	}
}

//==================================================================
static void discoverFuncsUsageSub( TokNode *pFuncCallNode, int &out_parentIdx )
{
	bool	isDataType = pFuncCallNode->IsDataType();

	const char *pStr = pFuncCallNode->GetTokStr();

	TokNode *pRightNode = pFuncCallNode->GetRight();

	if ( pRightNode && pRightNode->mpToken->id == T_VL_STRING )
	{
		if ( isDataType )
		{
			// ok, it's just a space cast..

			// ..reparent the cast string as child of the function call node
			out_parentIdx -= 1;

			pRightNode->Reparent( pFuncCallNode );
			pFuncCallNode->mpChilds.push_back( pRightNode );

			// ..next should be a bracket
			pRightNode = pRightNode->GetRight();
		}
		else
		{
			throw Exception( "Unknown usage, why a string ?",  pFuncCallNode );
		}
	}

	if ( pRightNode && pRightNode->mpToken->id == T_OP_LFT_BRACKET )
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

			..the loop above will skip one, so we need to backpedal..
		*/

		out_parentIdx -= 1;

		pRightNode->Reparent( pFuncCallNode );
		pFuncCallNode->mpChilds.push_back( pRightNode );

		removeCommasFromParams( pRightNode );

		pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
	}
}

//==================================================================
static void discoverFuncsUsage( TokNode *pNode, const DVec<Function> &funcs, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

	if ( !pNode->IsParentRoot() &&  // parent cannot be root !!
		 (pNode->IsNonTerminal() ||
		  pNode->IsStdFunction() ||
		  isDataType) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		discoverFuncsUsageSub( pNode, out_parentIdx );
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		discoverFuncsUsage( pNode->mpChilds[i], funcs, i );
	}
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot );

	int	idx = 0;
	discoverFuncsUsage( pRoot, pRoot->GetFuncs(), idx );
}

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
static const std::string getOperand(
					TokNode *pOperand,
					VarType &out_varType )
{
	out_varType = VT_UNKNOWN;

	//DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() >= 1 );
	while ( pOperand )
	{
		const Variable	*pVar = pOperand->GetVarPtr();

		if ( pVar )
		{
			// this should be the common (if not unique ?) case
			out_varType = pVar->GetVarType();
			return pVar->GetUseName();
		}
		else
		if ( pOperand->mpToken->idType == T_TYPE_VALUE )
		{
			out_varType = VT_FLOAT;
			return pOperand->GetTokStr();
		}
/*
		else
		{
			DASSERT( 0 );
			Register	reg = pOperand->GetRegister();

			if ( reg.IsValid() )
			{
				out_varType = reg.GetVarType();

				return GetRegName( reg );
			}
		}
*/

		pOperand = pOperand->GetChildTry( 0 );
	}

	//DASSERT( 0 );

	return "WHAT ?!";
}

//==================================================================
static void writeFuncParams( FILE *pFile, TokNode *pNode )
{
	if ( pNode->mpParent &&
		 pNode->mpParent->mpParent &&
		 pNode->mpParent->mpParent->mpToken->IsAssignOp() )
	{
		TokNode *pChild = pNode->mpParent->mpParent->GetChildTry( 0 );

		Register	reg = pChild->GetRegister();

		std::string regName = GetRegName( reg );

		fprintf_s( pFile, "\t%s", regName.c_str() );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->mpToken->id == T_OP_COMMA )
			continue;

		VarType	varType;
		std::string opStr = getOperand( pChild, varType );

		fprintf_s( pFile, "\t%-6s", opStr.c_str() );
	}

	fprintf_s( pFile, "\n\n" );
}

//==================================================================
static const char *asmOpCodeFromOpToken( const Token *pTok )
{
	switch ( pTok->id )
	{
	case T_OP_MUL:		return "mul";
	case T_OP_DIV:		return "div";
	case T_OP_PLUS:		return "add";
	case T_OP_MINUS:	return "sub";
	case T_OP_POW:		return "pow";
	case T_OP_DOT:		return "dot";

	default:
		return pTok->str.c_str();
	}
}

//==================================================================
static std::string resolveIntrinsics( const char *pIntrName )
{
	static size_t		sBaseLen;
	static const char	*spBaseStr = "_asm_";

	if ( sBaseLen == 0 )
		sBaseLen = strlen( "_asm_" );

	if ( pIntrName == strstr( pIntrName, spBaseStr ) )
	{
		return pIntrName + sBaseLen;
	}
	else
		return "UNKNOWN_INTRINSIC";
}

//==================================================================
static void buildExpression( FILE *pFile, TokNode *pNode )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		//if ( pNode->mpChilds[i]->mpToken->idType == T_TYPE_OPERATOR )
		{
			buildExpression( pFile, pNode->mpChilds[i] );
		}
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		size_t	nextChild = 0;
		TokNode *pChild = pNode->GetChildTry( nextChild );

		std::string	asmName = resolveIntrinsics( pNode->GetTokStr() );

		if ( pChild && pChild->mpToken->id == T_VL_STRING )
		{
			fprintf_s( pFile, "\t%s\t%s", asmName.c_str(), pChild->GetTokStr() );
			pChild = NULL;
			++nextChild;
		}
		else
			fprintf_s( pFile, "\t%s", asmName.c_str() );

		TokNode *pBracket = pNode->GetChildTry( nextChild );

		DASSERT( pBracket->mpToken->id == T_OP_LFT_BRACKET );

		writeFuncParams( pFile, pBracket );
	}
	else
	if ( pNode->mpToken->IsBiOp() )
	{
		TokNode *pOperand1 = pNode->GetChildTry( 0 );
		TokNode *pOperand2 = pNode->GetChildTry( 1 );

		// no assignment for function call as it's done above
		if ( pNode->mpToken->id == T_OP_ASSIGN &&
			 pOperand1->mNodeType == TokNode::TYPE_FUNCCALL )
			return;

		if ( pOperand1 && pOperand2 )
		{
			VarType	varType1;
			VarType	varType2;

			std::string	o1Str = getOperand( pOperand1, varType1 );
			std::string	o2Str = getOperand( pOperand2, varType2 );

			bool	doAssign = pNode->mpToken->IsAssignOp();

			char l1 = VarTypeToLetter(varType1);
			char l2 = VarTypeToLetter(varType2);

			if ( doAssign )
			{
				if ( pNode->mpToken->id == T_OP_ASSIGN )
				{
					// build either a load instruction or a move
/*
					if ( isValue2 )
						fprintf_s(
								pFile,
								"\tld%c\t%s\t%s\n",
								l1,
								o1Str.c_str(),
								o2Str.c_str() );
					else
*/
						fprintf_s(
								pFile,
								"\tmov%c%c\t%-6s\t%-6s\n",
								l1,
								l2,
								o1Str.c_str(),
								o2Str.c_str() );
				}
				else
					fprintf_s(
							pFile,
							"\t%s%c%c\t%-6s\t%-6s\t%-6s\n",
							asmOpCodeFromOpToken( pNode->mpToken ),
							l1,
							l2,
							o1Str.c_str(),
							o1Str.c_str(),
							o2Str.c_str() );
			}
			else
			{
				Register	reg = pNode->GetRegister();
				std::string regName = GetRegName( reg );

				fprintf_s(
						pFile,
						"\t%s%c%c\t%-6s\t%-6s\t%-6s\n",
						asmOpCodeFromOpToken( pNode->mpToken ),
						l1,
						l2,
						regName.c_str(),
						o1Str.c_str(),
						o2Str.c_str() );
			}
		}
	}
}

//==================================================================
void WriteFunctions( FILE *pFile, TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		if NOT( func.IsShader() )
			continue;

		fprintf_s( pFile, "\n;====================================\n" );

		if ( func.mpRetTypeTok->idType == T_TYPE_SHADERTYPE )
		{
			fprintf_s( pFile, "%s ;== %s\n",
							func.mpRetTypeTok->GetStrChar(),
								func.mpNameNode->GetTokStr() );
		}
		else
		{
			fprintf_s( pFile, "function %s\n", func.mpNameNode->GetTokStr() );
		}

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			buildExpression( pFile, pNode );
		}

		fprintf_s( pFile, "\n\tret\n" );
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
}
