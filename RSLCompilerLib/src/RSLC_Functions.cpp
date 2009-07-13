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

//==================================================================
namespace RSLC
{

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

		//if ( pRightNode->mpToken->id == T_OP_LFT_BRACKET )
		{
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

			pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
		}
/*
		else
		{
			throw Exception( "Expecting a left bracket !", pRightNode->mpToken );
		}
*/
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
			for (size_t i=0; i < pParamsNode->mpChilds.size(); ++i)
			{
				if ( pParamsNode->mpChilds[i]->mpToken->id == T_OP_COMMA )
					continue;

				// $$$ need to get the function params types from definition here..

				insertAssignToTemp(
						pParamsNode,
						i,
						VT_VECTOR,
						true );

				//i += 2;
			}
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		instrumentFuncsCallsParams( pNode->mpChilds[i], i );
	}
}

//==================================================================
static void instrumentFuncsCallsReturn( TokNode *pNode, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		DASSERT( pNode->mpParent != NULL );

		//if ( pNode->mpParent->mpToken->idType == T_TYPE_OPERATOR )
		if ( pNode->mpParent->mpToken->IsBiOp() )
		{
			int	ownIdx	= (pNode->mpParent->mpChilds[0] == pNode ? 0 : 1);
			int	otherIdx= ownIdx ^ 1;

			DASSERT( pNode->mpParent->mpChilds[ownIdx] == pNode );

			if ( pNode->mpParent->mpChilds[otherIdx]->mpToken->idType != T_TYPE_TEMPDEST )
			{
				// $$$ need to get the function return values types from definition here..

				insertAssignToTemp(
						pNode->mpParent,
						ownIdx,
						VT_VECTOR,
						true );
			}
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		instrumentFuncsCallsReturn( pNode->mpChilds[i], i );
	}
}

//==================================================================
void InstrumentFunctionCalls( TokNode *pRoot )
{
	int	parentIdx = 0;
	instrumentFuncsCallsParams( pRoot, parentIdx );

	parentIdx = 0;
	instrumentFuncsCallsReturn( pRoot, parentIdx );
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
					printf( "Param %i, is '%s' tmp register\n",
								j,
								VarTypeToString( pParamVal->mBuild_TmpReg.GetVarType() )
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
		if ( _stricmp(
				pFCallNode->mpToken->GetStrChar(),
				funcs[i].mpNameNode->GetTokStr()
				) )
			continue;

		return &funcs[i];
	}

	return NULL;
}

//==================================================================
static bool resolveFunctionCalls( TokNode *pNode, const DVec<Function> &funcs, size_t &parentIdx )
{
	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		const Function	*pFunc = matchFunctionByParams( pNode, funcs );
		
		if ( pFunc )
		{
			TokNode	*pClonedParams = cloneBranch( pFunc->mpParamsNode );

			pClonedParams->ReplaceNode( pNode );

			AddVariable(
				pClonedParams->mpParent,
				DNEW TokNode( DNEW Token( *pFunc->mpRetTypeTok ) ),
				DNEW TokNode( DNEW Token( "varying", T_DE_varying, T_TYPE_DETAIL ) ),	// %%% forced varying for now !
				NULL,
				pClonedParams );

/*
			pClonedParams->mBuild_TmpReg.SetType(
								VarTypeFromToken( pFunc->mpRetTypeTok ),
								true );	// force varying for now..
*/

			DSAFE_DELETE( pNode );
			return true;
		}
	}

	bool resolvedSomething = false;

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		resolvedSomething = resolveFunctionCalls( pNode->mpChilds[i], funcs, i );
	}

	return resolvedSomething;
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
static void instrumentFCallReturn( TokNode *pReturnNode, const VarLink	&destVLink )
{
	TokNode	*pRetNode = pReturnNode->GetRight();

	TokNode	*pAssign = insertAssignToNode( destVLink, pRetNode );

	pAssign->ReplaceNode( pReturnNode );
	DSAFE_DELETE( pReturnNode );

	pRetNode->UnlinkFromParent();
	DSAFE_DELETE( pRetNode );
}

//==================================================================
static void resolveFunctionReturns( TokNode *pReturnNode, DVec<TokNode *> &pAssignOpsToRemove )
{
	if ( pReturnNode->mpToken && pReturnNode->mpToken->id == T_KW_return )
	{
		TokNode	*pFnParams = pReturnNode;
		while ( pFnParams )
		{
			if ( pFnParams->GetBlockType() == BLKT_FNPARAMS )
			{
				break;
			}
			pFnParams = pFnParams->mpParent;
		}
		DASSERT( pFnParams->GetBlockType() == BLKT_FNPARAMS );
/*

		if ( pFnParams->mpParent->GetBlockType() != BLKT_ROOT )
		{
			TokNode	*pAssignOp = pFnParams->mpParent;

			if ( pAssignOp->mpToken->IsAssignOp() )
			{
				pAssignOpsToRemove.push_back( pAssignOp );
				instrumentFCallReturn( pReturnNode, pAssignOp->GetChildTry(0) );
				return;
			}
			else
			{
				// Could just delete the branch here.. maybe.. if there is no other form of output..
			}
		}
*/
		instrumentFCallReturn( pReturnNode, pFnParams->mVarLink );
		return;
	}

	for (size_t i=0; i < pReturnNode->mpChilds.size(); ++i)
	{
		resolveFunctionReturns( pReturnNode->mpChilds[i], pAssignOpsToRemove );
	}
}

/*
	=
		b
		(
			{
				return
				+
					1
					2

	=
		b
		(
			{
				=
					b
					+
						1
						2

	(
		{
			=
				b
				+
					1
					2
*/
//==================================================================
void ResolveFunctionCalls( TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	bool	resolvedCalls;

	DVec<TokNode *>	pAssignOpsToRemove;
	//do {

		size_t	curIdx = 0;
		resolvedCalls = resolveFunctionCalls( pNode, funcs, curIdx );

		pAssignOpsToRemove.clear();
		resolveFunctionReturns( pNode, pAssignOpsToRemove );

		/*	pAssignOpsToRemove[i]	( = )
				pOldDest
				pFnParams
					1
					2
		*/
#if 0
		for (size_t i=0; i < pAssignOpsToRemove.size(); ++i)
		{
			TokNode	*pOldDest = pAssignOpsToRemove[i]->GetChildTry( 0 );
			TokNode	*pFnParams = pAssignOpsToRemove[i]->GetChildTry( 1 );

			DASSERT( pFnParams->GetBlockType() == BLKT_FNPARAMS );

			pOldDest->UnlinkFromParent();
			DSAFE_DELETE( pOldDest );

			/*	pAssignOpsToRemove[i]	( = )
					pFnParams
						1
						2
			*/

			pFnParams->ReplaceNode( pAssignOpsToRemove[i] );

			pAssignOpsToRemove[i]->mpChilds.clear();
			DSAFE_DELETE( pAssignOpsToRemove[i] );

			/*	pFnParams	( <- pAssignOpsToRemove[i]	( = ) )
					1
					2
			*/
		}
#endif
	//} while ( resolvedCalls );
}

//==================================================================
static void getRegName( const Register &reg, char *pOutBuff, size_t maxOutSize )
{
	char	regBase[16] = {0};

	switch ( reg.GetVarType() )
	{
	case VT_FLOAT:	regBase[0] = 's'; break;
	case VT_POINT:	regBase[0] = 'v'; break;
	case VT_COLOR:	regBase[0] = 'v'; break;
	case VT_STRING:	regBase[0] = 's'; DASSERT( 0 ); break;
	case VT_VECTOR:	regBase[0] = 'v'; break;
	case VT_NORMAL:	regBase[0] = 'v'; break;
	case VT_MATRIX:	regBase[0] = 'm'; DASSERT( 0 ); break;
	case VT_BOOL:	regBase[0] = 'b'; break;
	default:
		strcpy_s( regBase, "UNK_" );
		//DASSERT( 0 );
		break;
	}

	if NOT( reg.IsVarying() )
		strcat_s( regBase, "u" );

	sprintf_s( pOutBuff, maxOutSize, "$%s%i", regBase, reg.GetRegIdx() );
}

//==================================================================
static const char *getOperand( TokNode *pOperand, char *pOutBuff, size_t maxOutSize )
{
	//DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() >= 1 );
	while ( pOperand )
	{
		Register	reg = pOperand->BuildGetRegister();

		if ( reg.IsValid() )
		{
			getRegName( reg, pOutBuff, maxOutSize );
			return pOutBuff;
		}
		else
		if ( pOperand->mpToken->idType == T_TYPE_VALUE || pOperand->mpToken->idType == T_TYPE_NONTERM )
			return pOperand->GetTokStr();

		pOperand = pOperand->GetChildTry( 0 );
	}

	//DASSERT( 0 );

	return "WHAT ?!";
}

//==================================================================
static void writeFuncParams( FILE *pFile, TokNode *pNode )
{
	char	op1NameBuff[32];

	if ( pNode->mpParent &&
		 pNode->mpParent->mpParent &&
		 pNode->mpParent->mpParent->mpToken->IsAssignOp() )
	{
		TokNode *pChild = pNode->mpParent->mpParent->GetChildTry( 0 );

		Register	reg = pChild->BuildGetRegister();

		getRegName( reg, op1NameBuff, _countof(op1NameBuff) );

		fprintf_s( pFile, "\t%s", op1NameBuff );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->mpToken->id == T_OP_COMMA )
			continue;

		const char	*pOStr = getOperand( pChild, op1NameBuff, _countof(op1NameBuff) );

		fprintf_s( pFile, "\t%s", pOStr );
	}

	fprintf_s( pFile, "\n\n" );
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
		//pNode->mpToken->id == T_OP_ASSIGN
		size_t	nextChild = 0;
		TokNode *pChild = pNode->GetChildTry( nextChild );
		if ( pChild && pChild->mpToken->id == T_VL_STRING )
		{
			fprintf_s( pFile, "\t%s\t%s", pNode->GetTokStr(), pChild->GetTokStr() );
			pChild = NULL;
			++nextChild;
		}
		else
			fprintf_s( pFile, "\t%s", pNode->GetTokStr() );

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
			char	op1NameBuff[32];
			char	op2NameBuff[32];

			const char	*pO1Str = getOperand( pOperand1, op1NameBuff, _countof(op1NameBuff) );
			const char	*pO2Str = getOperand( pOperand2, op2NameBuff, _countof(op2NameBuff) );

			bool	doAssign = pNode->mpToken->IsAssignOp();

			if ( doAssign )
			{
				if ( pNode->mpToken->id == T_OP_ASSIGN )
					fprintf_s( pFile, "\tmov\t%s\t%s\n", pO1Str, pO2Str );
				else
					fprintf_s( pFile, "\t%s\t%s\t%s\t%s\n",
								pNode->GetTokStr(),
									pO1Str,
										pO1Str,
											pO2Str );
			}
			else
			{
				char regName[32];
		
				Register	reg = pNode->BuildGetRegister();
				getRegName( reg, regName, _countof(regName) );

				fprintf_s( pFile, "\t%s\t%s\t%s\t%s\n",
								pNode->GetTokStr(),
									regName,
										pO1Str,
											pO2Str );
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
			fprintf_s( pFile, "%s %s\n",
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
