//==================================================================
/// RSLC_Registers.cpp
///
/// Created by Davide Pasca - 2009/6/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Registers.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void solveTempOperand( TokNode *pOperand, size_t &io_tempIdx )
{
	while ( pOperand->mBuild_TmpReg.mRegIdx == -1 )
	{
		if ( pOperand->mpToken->idType == T_TYPE_TEMPDEST )
		{
			// assign a temporary and get out
			pOperand->mBuild_TmpReg.mRegIdx = io_tempIdx++;
			break;
		}

		if ( pOperand->mpChilds.size() == 0 )
		{
			return;	// "BADDDD";
		}

		// expect at least one children
		DASSERT( pOperand->mpChilds.size() > 0 );
		pOperand = pOperand->mpChilds[0];
	}
}

//==================================================================
static void solveOperand( TokNode *pOperand, size_t &io_tempIdx )
{
	if ( pOperand->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() == 2 );
		DASSERT( pOperand->mpParent->mpChilds[0]->mBuild_TmpReg.mRegIdx != -1 );
	}
	else
	if ( pOperand->mpToken->idType == T_TYPE_VALUE || pOperand->mpToken->idType == T_TYPE_NONTERM )
	{
	}
	else
	{
		solveTempOperand( pOperand, io_tempIdx );
	}
}

//==================================================================
static void assignRegisters_expr_fcall( TokNode *pNode, size_t &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->mpToken->id == T_OP_COMMA )
			continue;

		solveOperand( pChild, io_tempIdx );
	}
}

//==================================================================
static void assignRegisters_expr( TokNode *pNode, size_t &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		assignRegisters_expr( pNode->mpChilds[i], io_tempIdx );
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		//pNode->mpToken->id == T_OP_ASSIGN
		size_t	nextChild = 0;
		TokNode *pChild = pNode->GetChildTry( nextChild );
		if ( pChild && pChild->mpToken->id == T_VL_STRING )
		{
			pChild = NULL;
			++nextChild;
		}

		TokNode *pBracket = pNode->GetChildTry( nextChild );

		DASSERT( pBracket->mpToken->id == T_OP_LFT_BRACKET );

		if ( pNode->mpParent &&
			 pNode->mpParent->mpToken->IsAssignOp() )
		{
			TokNode *pDestTemp = pNode->mpParent->GetChildTry( 0 );

			DASSERT( pDestTemp->mpToken->idType == T_TYPE_TEMPDEST );
			DASSERT( pDestTemp->mBuild_TmpReg.mRegIdx == -1 );

			pDestTemp->mBuild_TmpReg.mRegIdx	= io_tempIdx++;
		}

		assignRegisters_expr_fcall( pBracket, io_tempIdx );
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
			solveOperand( pOperand1, io_tempIdx );
			solveOperand( pOperand2, io_tempIdx );

			if NOT( pNode->mpToken->IsAssignOp() )
			{
				// a one time assignment only..
				DASSERT( pNode->mBuild_TmpReg.mRegIdx == -1 );
				pNode->mBuild_TmpReg.mRegIdx = io_tempIdx++;
			}
		}
		else
		{
			//fprintf_s( pFile, "\nWEIRDDD !! ( %s )\n", pNode->GetTokStr() );
		}
	}
}

//==================================================================
void AssignRegisters( TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		if NOT( func.IsShader() )
			continue;

		for (size_t i=0; i < func.mpCodeBlkNode->mpChilds.size(); ++i)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[i];

			size_t	tempIdx = 0;

			assignRegisters_expr( pNode, tempIdx );
		}
	}
}

//==================================================================
}
