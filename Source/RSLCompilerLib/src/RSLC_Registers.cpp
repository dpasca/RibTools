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
static void assignRegisters_sub( TokNode *pNode, int &io_regIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		assignRegisters_sub( pNode->mpChilds[i], io_regIdx );
	}

	const Variable	*pVar = pNode->GetVarPtr();

	if ( pVar )
	{
		if (!pVar->HasBaseVal() &&
			!pVar->mIsGlobal &&
			!pVar->mIsArray &&
			!pVar->IsRegisterAssigned() )
		{
			pNode->GetVarPtr()->AssignRegister( io_regIdx );
			++io_regIdx;
		}
	}
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
