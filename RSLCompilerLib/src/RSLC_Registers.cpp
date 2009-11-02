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
}
