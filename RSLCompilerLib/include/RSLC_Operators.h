//==================================================================
/// RSLC_Operators.h
///
/// Created by Davide Pasca - 2009/6/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_OPERATORS_H
#define RSLC_OPERATORS_H

//==================================================================
namespace RSLC
{

class TokNode;

void ReparentOperators( TokNode *pNode );

void SolveBiOpType(
					const TokNode *pOperator,
					const TokNode *pOperand1,
					const TokNode *pOperand2,
					VarType &out_varType,
					bool &out_isVarying );

//==================================================================
}

#endif
