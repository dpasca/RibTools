//==================================================================
/// RSLC_Registers.cpp
///
/// Created by Davide Pasca - 2009/6/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Registers.h"
#include "RSLC_Operators.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void solveTempOperand( TokNode *pOperand, int &io_tempIdx )
{
	while ( pOperand->mBuild_TmpReg.GetRegIdx() == -1 )
	{
		if ( pOperand->mpToken->idType == T_TYPE_TEMPDEST )
		{
			// assign a temporary and get out
			pOperand->mBuild_TmpReg.SetRegIdx( io_tempIdx++ );
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
static void solveOperand( TokNode *pOperand, int &io_tempIdx )
{
	if ( pOperand->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() == 2 );
		DASSERT( pOperand->mpParent->mpChilds[0]->mBuild_TmpReg.GetRegIdx() != -1 );
	}
	else
	if ( pOperand->mpToken->idType == T_TYPE_VALUE || pOperand->mpToken->idType == T_TYPE_NONTERM )
	{
		if ( pOperand->mVarLink.IsValid() && !pOperand->mVarLink.IsGlobal() )
		{
			Variable	*pVar = pOperand->mVarLink.GetVarPtr();

			if NOT( pVar->mBuild_Register.IsValid() )
				pVar->AssignRegister( io_tempIdx );
		}
	}
	else
	{
		solveTempOperand( pOperand, io_tempIdx );
	}
}

/*
//==================================================================
static VarType getOperandType( TokNode *pOperand )
{
	if ( pOperand->mVarLink.IsValid() )
	{
		Variable	*pVar = pOperand->mVarLink.GetVarPtr();

		return pVar->GetVarType();
	}
	else
	{
		if ( pOperand->mpToken->idType == T_TYPE_TEMPDEST )
			return pOperand->mBuild_TmpReg.GetRegIdx();
	}

	DASSERT( 0 );

	return VT_UNKNOWN;
}
*/

/*
//==================================================================
static void assignRegisters_expr_fcall( TokNode *pNode, int &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->mpToken->id == T_OP_COMMA )
			continue;

		solveOperand( pChild, io_tempIdx );
	}
}
*/

//==================================================================
static void assignRegisters_expr( TokNode *pNode, int &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		assignRegisters_expr( pNode->mpChilds[i], io_tempIdx );
	}

	if ( pNode->mpToken->IsBiOp() )
	{
		TokNode *pOperand1 = pNode->GetChildTry( 0 );
		TokNode *pOperand2 = pNode->GetChildTry( 1 );

		// no assignment for function call as it's done above
		VarType	opResVarType;
		bool	opResIsVarying;
		SolveBiOpType( pNode, pOperand1, pOperand2, opResVarType, opResIsVarying );

		DASSERT( pNode->mVarLink.IsValid() == false );

		pNode->mBuild_TmpReg.SetType( opResVarType, opResIsVarying );
	}

	if ( pNode->mVarLink.IsValid() )
	{
		if (
			!pNode->mVarLink.IsGlobal() &&
			!pNode->mVarLink.GetVarPtr()->IsRegisterAssigned() )
		{
			pNode->mVarLink.GetVarPtr()->AssignRegister( io_tempIdx );
		}
	}
	else
	if ( pNode->mBuild_TmpReg.GetVarType() != VT_UNKNOWN )
	{
		//if NOT( pNode->mBuild_TmpReg.IsAssigned() )
			pNode->mBuild_TmpReg.SetRegIdx( io_tempIdx++ );	
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

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			int	tempIdx = 0;

			assignRegisters_expr( pNode, tempIdx );
		}
	}
}

//==================================================================
}
