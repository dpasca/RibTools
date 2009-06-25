//==================================================================
/// RSLC_Functions.cpp
///
/// Created by Davide Pasca - 2009/5/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

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

		pFunc->mpCodeBlkNode	= pNode;
		pFunc->mpNameTok		= pFuncName->mpToken;
		pFunc->mpRetTypeTok		= pRetType->mpToken;

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

	if ( (pNode->IsNonTerminal() ||
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
static void insertAssignToTemp( TokNode *pNode, size_t childIdx )
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

	pAssgnNode->AddNewChild( pDestRegTok );

	pAssgnNode->AddChild( pOldNode );
}

//==================================================================
static void instrumentFuncsParams( TokNode *pNode, int &out_parentIdx )
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

				insertAssignToTemp( pParamsNode, i );

				//i += 2;
			}
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		instrumentFuncsParams( pNode->mpChilds[i], i );
	}
}

//==================================================================
static void instrumentFuncsReturn( TokNode *pNode, int &out_parentIdx )
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

			if ( pNode->mpParent->mpChilds[otherIdx]->mpToken->id != T_TD_TEMPDEST )
			{
				insertAssignToTemp( pNode->mpParent, ownIdx );
			}
		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		instrumentFuncsReturn( pNode->mpChilds[i], i );
	}
}

//==================================================================
void InstrumentFunctionCalls( TokNode *pRoot )
{
	int	parentIdx = 0;
	instrumentFuncsParams( pRoot, parentIdx );

	parentIdx = 0;
	instrumentFuncsReturn( pRoot, parentIdx );
}

//==================================================================
static const char *getTempOperand( TokNode *pOperand, char *pOutBuff, size_t maxOutSize, size_t &io_tempIdx )
{
	while ( pOperand->mTempRegIdx == -1 )
	{
		if ( pOperand->mpToken->idType == T_TYPE_TEMPDEST )
		{
			// assign a temporary and get out
			pOperand->mTempRegIdx = io_tempIdx++;
			break;
		}

		if ( pOperand->mpChilds.size() == 0 )
			return "BADDDD";

		// expect at least one children
		DASSERT( pOperand->mpChilds.size() > 0 );
		pOperand = pOperand->mpChilds[0];
	}

	sprintf_s( pOutBuff, maxOutSize, "$t%i", pOperand->mTempRegIdx );

	return pOutBuff;
}

//==================================================================
static const char *getOperand( TokNode *pOperand, char *pOutBuff, size_t maxOutSize, size_t &io_tempIdx )
{
	if ( pOperand->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() == 2 );
		DASSERT( pOperand->mpParent->mpChilds[0]->mTempRegIdx != -1 );

		sprintf_s( pOutBuff, maxOutSize, "$t%i", pOperand->mpParent->mpChilds[0]->mTempRegIdx );

		return pOutBuff;
	}
	else
	if ( pOperand->mpToken->idType == T_TYPE_VALUE || pOperand->mpToken->idType == T_TYPE_NONTERM )
		return pOperand->GetTokStr();
	else
	if ( pOperand->mpToken->idType == T_TYPE_STDFUNC )
	{
		return getTempOperand( pOperand, pOutBuff, maxOutSize, io_tempIdx );
	}
	else
//	if ( pOperand->mpToken->idType == T_TYPE_OPERATOR )
	{
		return getTempOperand( pOperand, pOutBuff, maxOutSize, io_tempIdx );
	}
//	else
//	{
//	}
}

/*

*
	+
		a
		b

	+
		c
		d

	$t0 = + a b
	$t1 = + c d

$t2 = +	$t0	$t1

*/

//==================================================================
static void writeFuncParams( FILE *pFile, TokNode *pNode, size_t &io_tempIdx )
{
	char	op1NameBuff[32];

	if ( pNode->mpParent &&
		 pNode->mpParent->mpParent &&
		 pNode->mpParent->mpParent->mpToken->IsAssignOp() )
	{
		TokNode *pChild = pNode->mpParent->mpParent->GetChildTry( 0 );

		sprintf_s( op1NameBuff, _countof(op1NameBuff), "$t%i", pChild->mTempRegIdx );

		fprintf_s( pFile, "\t%s", op1NameBuff );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->mpToken->id == T_OP_COMMA )
			continue;

		const char	*pOStr = getOperand( pChild, op1NameBuff, _countof(op1NameBuff), io_tempIdx );

		fprintf_s( pFile, "\t%s", pOStr );
	}

	fprintf_s( pFile, "\n\n" );
}

//==================================================================
static void buildExpression( FILE *pFile, TokNode *pNode, size_t &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		//if ( pNode->mpChilds[i]->mpToken->idType == T_TYPE_OPERATOR )
		{
			buildExpression( pFile, pNode->mpChilds[i], io_tempIdx );
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

		if ( pNode->mpParent &&
			 pNode->mpParent->mpToken->IsAssignOp() )
		{
			TokNode *pDestTemp = pNode->mpParent->GetChildTry( 0 );

			DASSERT( pDestTemp->mpToken->idType == T_TYPE_TEMPDEST );
			DASSERT( pDestTemp->mTempRegIdx == -1 );

			pDestTemp->mTempRegIdx = io_tempIdx++;
		}

		writeFuncParams( pFile, pBracket, io_tempIdx );
	}
	else
	if ( pNode->mpToken->IsBiOp() )
	{
		TokNode *pOperand1 = pNode->GetChildTry( 0 );
		TokNode *pOperand2 = pNode->GetChildTry( 1 );

		// no assignment for function call as it's done above
		if ( pNode->mpToken->id == T_OP_ASSIGN &&
			(pOperand1->mNodeType == TokNode::TYPE_FUNCCALL ||
			 pOperand2->mNodeType == TokNode::TYPE_FUNCCALL ) )
			return;

		if ( pOperand1 && pOperand2 )
		{
			char	op1NameBuff[32];
			char	op2NameBuff[32];

			const char	*pO1Str = getOperand( pOperand1, op1NameBuff, _countof(op1NameBuff), io_tempIdx );
			const char	*pO2Str = getOperand( pOperand2, op2NameBuff, _countof(op2NameBuff), io_tempIdx );

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

				//fprintf_s( pFile, "\n" );
			}
			else
			{
				// a one time assignment only..
				DASSERT( pNode->mTempRegIdx == -1 );
				pNode->mTempRegIdx = io_tempIdx++;

				fprintf_s( pFile, "\t%s\t$t%i\t%s\t%s\n",
								pNode->GetTokStr(),
									pNode->mTempRegIdx,
										pO1Str,
											pO2Str );
			}
		}
		else
		{
			//fprintf_s( pFile, "\nWEIRDDD !! ( %s )\n", pNode->GetTokStr() );
		}
	}
}

//==================================================================
static void generateExpressions( FILE *pFile, TokNode *pCodeBlkNode )
{
	for (size_t i=0; i < pCodeBlkNode->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pCodeBlkNode->mpChilds[i];

		size_t	tempIdx = 0;
		// are we assigning something 
		//if ( pNode->mpToken->IsAssignOp() )
			buildExpression( pFile, pNode, tempIdx );
	}
}

//==================================================================
void WriteFunctions( FILE *pFile, TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		fprintf_s( pFile, "\n;====================================\n" );

		if ( func.mpRetTypeTok->idType == T_TYPE_SHADERTYPE )
		{
			fprintf_s( pFile, "%s %s\n",
							func.mpRetTypeTok->GetStrChar(),
								func.mpNameTok->GetStrChar() );
		}
		else
		{
			fprintf_s( pFile, "function %s\n", func.mpNameTok->GetStrChar() );
		}

		generateExpressions( pFile, func.mpCodeBlkNode );

		fprintf_s( pFile, "\n\tret\n" );
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
}
