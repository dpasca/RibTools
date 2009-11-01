//==================================================================
/// RSLC_Expressions.cpp
///
/// Created by Davide Pasca - 2009/11/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Operators.h"
#include "RSLC_Expressions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void solveExpressions_sub_BiOp( TokNode *pNode, bool mustSucceed )
{
	TokNode *pOperand1 = pNode->GetChildTry( 0 );
	TokNode *pOperand2 = pNode->GetChildTry( 1 );

	// scan down to find a variable
	while ( pOperand1 && !pOperand1->GetVarPtr() )
		pOperand1 = pOperand1->GetChildTry( 0 );

	while ( pOperand2 && !pOperand2->GetVarPtr() )
		pOperand2 = pOperand2->GetChildTry( 0 );

	if NOT( pOperand1 )
	{
		if ( mustSucceed )
			throw Exception( "Unknown 1st operand", pNode );
		else
			return;
	}

	if NOT( pOperand2 )
	{
		if ( mustSucceed )
			throw Exception( "Unknown 2nd operand", pNode );
		else
			return;
	}

	// no assignment for function call as it's done above
	VarType	opResVarType;
	bool	opResIsVarying;

	if NOT( SolveBiOpType(
					pNode,
					pOperand1,
					pOperand2,
					opResVarType,
					opResIsVarying,
					mustSucceed ) )
		return;

	// $$$ was here for a purpose !(?) - DASSERT( pNode->mVarLink.IsValid() == false );

	// is this an assignment ?
	if ( pNode->mpToken->IsAssignOp() )
	{
		// ..are we dealing with varying data at all ?
		if ( opResIsVarying )
		{
			Variable	*pOp1Var = pOperand1->mVarLink.GetVarPtr();

			// is the destination not varying ?
			if NOT( pOp1Var->IsVarying() )
			{
				if ( pOp1Var->IsForcedDetail() )
				{
					// this is the end of it..
					throw Exception( "Cannot assign a varying variable to an 'uniform' destination", pNode );
				}
				else
				{
					pOp1Var->SetVarying( opResIsVarying );
				}
			}
		}			
	}
	else
	{
		if NOT( pNode->mpToken->idType == T_TYPE_VALUE )
			AddSelfVariable( pNode, opResVarType, opResIsVarying, false );
	}
}

//==================================================================
static void solveExpressions_sub( TokNode *pNode, bool mustSucceed )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		solveExpressions_sub( pNode->mpChilds[i], mustSucceed );

	if ( pNode->mpToken && pNode->mpToken->IsBiOp() )
		solveExpressions_sub_BiOp( pNode, mustSucceed );
}

//==================================================================
void SolveExpressions( TokNode *pNode, bool mustSucceed, bool processShaderOnly )
{
	if ( processShaderOnly )
	{
		const DVec<Function> &funcs = pNode->GetFuncs();

		for (size_t i=0; i < funcs.size(); ++i)
		{
			const Function	&func = funcs[i];

			// pre-pass ?
			if ( processShaderOnly )
				if NOT( func.IsShader() )
					continue;

			if ( func.mpParamsNode )
				solveExpressions_sub( func.mpParamsNode, mustSucceed );
			else
				solveExpressions_sub( func.mpCodeBlkNode, mustSucceed );
		}
	}
	else
	{
		solveExpressions_sub( pNode, mustSucceed );
	}
}

//==================================================================
}
