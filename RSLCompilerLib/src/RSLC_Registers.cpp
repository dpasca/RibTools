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

//==================================================================
namespace RSLC
{

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

//==================================================================
static void realizeConstants_rec( TokNode *pNode, TokNode *pRoot )
{
	if ( pNode->mpToken && pNode->mpToken->idType == T_TYPE_VALUE )
	{
		AddConstVariable( pNode, pRoot );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		realizeConstants_rec( pNode->mpChilds[i], pRoot );
	}
}

//==================================================================
void RealizeConstants( TokNode *pRoot )
{
	const DVec<Function> &funcs = pRoot->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		if NOT( func.IsShader() )
			continue;

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			int	tempIdx = 0;

			realizeConstants_rec( pNode, pRoot );
		}
	}
}

//==================================================================
static void assignRegisters_expr_BiOp( TokNode *pNode )
{
	TokNode *pOperand1 = pNode->GetChildTry( 0 );
	TokNode *pOperand2 = pNode->GetChildTry( 1 );

	// scan down to find a variable
	while ( pOperand1 && !pOperand1->GetVarPtr() )
		pOperand1 = pOperand1->GetChildTry( 0 );

	while ( pOperand2 && !pOperand2->GetVarPtr() )
		pOperand2 = pOperand2->GetChildTry( 0 );

	DASSERT( pOperand1 && pOperand2 );

	// no assignment for function call as it's done above
	VarType	opResVarType;
	bool	opResIsVarying;
	SolveBiOpType( pNode, pOperand1, pOperand2, opResVarType, opResIsVarying );

	DASSERT( pNode->mVarLink.IsValid() == false );

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
static void assignRegisters_expr( TokNode *pNode, int &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		assignRegisters_expr( pNode->mpChilds[i], io_tempIdx );
	}

	if ( pNode->mpToken->IsBiOp() )
	{
		assignRegisters_expr_BiOp( pNode );
	}

	const Variable	*pVar = pNode->GetVarPtr();

	if ( pVar )
	{
		if (!pVar->mHasBaseVal &&
			!pVar->mIsGlobal &&
			!pVar->IsRegisterAssigned() )
		{
			pNode->GetVarPtr()->AssignRegister( io_tempIdx++ );
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

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			int	tempIdx = 0;

			assignRegisters_expr( pNode, tempIdx );
		}
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
	case VT_STRING:	regBase[0] = 'x'; DASSERT( 0 ); break;
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

	return DUT::SSPrintF( "$%s%i", regBase, reg.GetRegIdx() );
}

//==================================================================
}
