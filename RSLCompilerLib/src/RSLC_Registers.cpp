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
