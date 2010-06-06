//==================================================================
/// RSLC_Operators_SolveBiOpType.cpp
///
/// Created by Davide Pasca - 2010/1/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Operators.h"
#include "RSLC_Operators_SolveBiOpType_RulesTable.h"

//==================================================================
namespace RSLC
{

//==================================================================
static bool areTypesCompatible( VarType vt1, VarType vt2 )
{
	if ( vt1 == vt2 )
		return true;

	if (
		(vt1 == VT_POINT	&& vt2 == VT_VECTOR	) ||
		(vt1 == VT_POINT	&& vt2 == VT_NORMAL	) ||
		(vt1 == VT_VECTOR	&& vt2 == VT_POINT	)||
		(vt1 == VT_VECTOR	&& vt2 == VT_NORMAL	) ||
		(vt1 == VT_NORMAL	&& vt2 == VT_POINT	) ||
		(vt1 == VT_NORMAL	&& vt2 == VT_VECTOR	)
		)
		return true;

	return false;
}

//==================================================================
void SolveBiOpType(	const TokNode *pOperator,
					VarType	vt1,
					VarType	vt2,
					VarType &out_varType )
{
	out_varType		= VT_UNKNOWN;

	DASSERT( pOperator->mpToken->IsBiOp() );

	DASSERT( vt1 != VT_UNKNOWN && vt2 != VT_UNKNOWN );

	// search for a rule that matches
	for (size_t i=0; i < _countof(_sBiOpConvertRules); ++i)
	{
		if NOT( pOperator->GetTokID() == _sBiOpConvertRules[i].mOper )
			continue;

		if NOT(
			areTypesCompatible( _sBiOpConvertRules[i].mLType, vt1 ) &&
			areTypesCompatible( _sBiOpConvertRules[i].mRType, vt2 )
			)
			continue;

		out_varType = _sBiOpConvertRules[i].mResType;
		break;
	}

	if ( out_varType == VT_UNKNOWN )
	{
		throw Exception(
			DUT::SSPrintFS(
					"Incompatible operation for the given types [ '%s' %s '%s' ]",
						VarTypeToString( vt1 ),
						pOperator->GetTokStr(),
						VarTypeToString( vt2 ) )
					, pOperator );
	}
}

//==================================================================
}
