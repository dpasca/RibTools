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
#include "RSLC_Exceptions.h"

#define PROCESS_ALL_FUNCS

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

	if ( pNode->mpToken->IsBiOp() )
		solveExpressions_sub_BiOp( pNode, mustSucceed );
}

//==================================================================
void SolveExpressions( TokNode *pNode, bool mustSucceed, bool processShaderOnly )
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

//==================================================================
static bool assignRegisters_sub( TokNode *pNode, int regIdx )
{
	int runRegIdx = regIdx;

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		if ( assignRegisters_sub( pNode->mpChilds[i], runRegIdx ) )
			runRegIdx += 1;
	}

	const Variable	*pVar = pNode->GetVarPtr();

	if ( pVar )
	{
		if (!pVar->mHasBaseVal &&
			!pVar->mIsGlobal &&
			!pVar->IsRegisterAssigned() )
		{
			pNode->GetVarPtr()->AssignRegister( regIdx );
		}
	}

	return pNode->mpToken->IsBiOp();
}

//==================================================================
void AssignRegisters( TokNode *pNode, int regIdx )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		if NOT( func.IsShader() )
			continue;

		if ( func.mpParamsNode )
			assignRegisters_sub( func.mpParamsNode, regIdx );
		else
			assignRegisters_sub( func.mpCodeBlkNode, regIdx );
	}
}

//==================================================================
std::string GetRegName( const Register &reg )
{
	char	regBase[16] = {0};

	switch ( reg.GetVarType() )
	{
	case VT_FLOAT:	regBase[0] = 's'; break;
	case VT_POINT:	regBase[0] = 'v'; break;
	case VT_COLOR:	regBase[0] = 'v'; break;
	case VT_STRING:	regBase[0] = 'x'; break;
	case VT_VECTOR:	regBase[0] = 'v'; break;
	case VT_NORMAL:	regBase[0] = 'v'; break;
	case VT_MATRIX:	regBase[0] = 'm'; break;
	case VT_BOOL:	regBase[0] = 'b'; break;
	default:
		strcpy_s( regBase, "UNK_" );
		//DASSERT( 0 );
		break;
	}

	if NOT( reg.IsVarying() )
		strcat_s( regBase, "u" );

	return DUT::SSPrintFS( "$%s%i", regBase, reg.GetRegIdx() );
}

//==================================================================
}
