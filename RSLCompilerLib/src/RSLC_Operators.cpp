//==================================================================
/// RSLC_Operators.cpp
///
/// Created by Davide Pasca - 2009/6/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Operators.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Defs_OpConvert.h"

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
void SolveBiOpType(
					const TokNode *pOperator,
					const TokNode *pOperand1,
					const TokNode *pOperand2,
					VarType &out_varType,
					bool &out_isVarying )
{
	out_varType		= VT_UNKNOWN;
	out_isVarying	= false;

	DASSERT( pOperator->mpToken->IsBiOp() );

	VarType	vt1 = pOperand1->GetVarType();
	VarType	vt2 = pOperand2->GetVarType();

	DASSERT( vt1 != VT_UNKNOWN && vt2 != VT_UNKNOWN );

	if ( (vt1 == VT_STRING) != (vt2 == VT_STRING) )
		throw Exception( "Strings can only operate with other strings !", pOperator );

	if ( vt1 == VT_STRING )
	{
		// strings are always uniform !!!
		DASSERT( pOperand1->IsVarying() == false );
		DASSERT( pOperand2->IsVarying() == false );

		if ( pOperator->mpToken->id != T_OP_ASSIGN ||
			 pOperator->mpToken->id != T_OP_EQ ||
			 pOperator->mpToken->id != T_OP_NEQ
			 )
			throw Exception( "Invalid operator between strings !", pOperator );
	}

	// varying is easy.. out is varying if even one of the two is
	out_isVarying = (pOperand1->IsVarying() || pOperand2->IsVarying() );

	// handle comparison operators
	if ( pOperator->mpToken->IsCmpOp() )
	{
		if NOT( areTypesCompatible( vt1, vt2 ) )
			throw Exception( "Incompatible types in comparison", pOperator );

		out_varType = VT_BOOL;
	}
	else
	{
		for (size_t i=0; i < _countof(_gBiOpConvertRules); ++i)
		{
			if NOT( pOperator->mpToken->id == _gBiOpConvertRules[i].mOper )
				continue;

			if NOT(
				areTypesCompatible( _gBiOpConvertRules[i].mLType, vt1 ) &&
				areTypesCompatible( _gBiOpConvertRules[i].mRType, vt2 )
				)
				continue;

			out_varType = _gBiOpConvertRules[i].mResType;
		}

		if ( out_varType == VT_UNKNOWN )
		{
			throw Exception(
				DUT::SSPrintFS(
						"Could not resolve operation. Incompatible types [ '%s' %s '%s' ] ?",
							VarTypeToString( vt1 ),
							pOperator->GetTokStr(),
							VarTypeToString( vt2 ) )
						, pOperator );
		}
	}
}

//==================================================================
static void reparentBiOperators(
						TokNode *pNode,
						const TokenID *pMatchingIDs,
						size_t matchIDsN,
						size_t &out_parentIdx,
						bool excludeIfFuncParam=false )
{
	if ( pNode->mpToken )
	{
		bool	dontProcess = false;
		if ( excludeIfFuncParam && pNode->mpParent )
		{
			BlockType	blkType = pNode->mpParent->GetBlockType();

			if ( blkType == BLKT_FUNCCALL || blkType == BLKT_FNPARAMS )
				dontProcess = true;
		}

		if NOT( dontProcess )
		{
			for (size_t i=0; i < matchIDsN; ++i)
			{
				if ( pMatchingIDs[i] == pNode->mpToken->id )
				{
					TokNode	*pLValue = pNode->GetLeft();
					TokNode	*pRValue = pNode->GetRight();

					//if NOT( pLValue )
					//	throw Exception( "Missing left value in expression assignment.", pNode->mpToken );

					if ( pRValue && pLValue )
					{
						//	L	=	R

						if ( pRValue )
						{
							pRValue->Reparent( pNode );
							pNode->mpChilds.push_front( pRValue );
						}
						//	L	=
						//			R

						if ( pLValue )
						{
							pLValue->Reparent( pNode );
							pNode->mpChilds.push_front( pLValue );
						}
						//		=
						//	L		R

						DASSERT( out_parentIdx > 0 );
						out_parentIdx -= 1;
					}

					break;
				}
			}
		}
	}

	//if ( pNode->GetBlockType() != BLKT_UNKNOWN )
		for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		{
			reparentBiOperators( pNode->mpChilds[i], pMatchingIDs, matchIDsN, i, excludeIfFuncParam );
		}
}

/*

binop:
	.
	/	*
	^
	+	-

relop:
	>	>=	<	<=
	==	!=

logop:
	&&
	||

tri:
	?	:

asgnop:
		=	+=	-=	*=	/=

	(
		0
		,a
		(
			1
			,b
			2
		)
		,c
		3
	)

	(
		,a
			0
			(
				1
				,b
				2
			)
		,c
		3
	)

	(
		,c
			,a
				0
				(
					,b
						1
						2
				)
			3
	)

*/

//==================================================================
void ReparentOperators( TokNode *pNode )
{
	// T_OP_COLON	,
	// T_OP_SEMICOL	,

	static TokenID	priority0[] = {
		T_OP_COMMA		,
	};
	static TokenID	priority1[] = {
		T_OP_DOT		,
	};
	static TokenID	priority2[] = {
		T_OP_DIV		,
		T_OP_MUL		,
	};
	static TokenID	priority3[] = {
		T_OP_POW		,
	};
	static TokenID	priority4[] = {
		T_OP_MINUS		,
		T_OP_PLUS		,
	};
	static TokenID	priority5[] = {
		T_OP_GRTH		,
		T_OP_GEEQ		,
		T_OP_LSTH		,
		T_OP_LSEQ		,
	};
	static TokenID	priority6[] = {
		T_OP_EQ			,
		T_OP_NEQ		,
	};
	static TokenID	priority7[] = {
		T_OP_LOGIC_AND	,
		T_OP_LOGIC_OR	,
	};
	static TokenID	priority8[] = {
		T_OP_ASSIGN		,
		T_OP_PLUSASS	,
		T_OP_MINUSASS	,
		T_OP_MULASS		,
		T_OP_DIVASS		,
	};

	size_t	idx;

	reparentBiOperators( pNode, priority0, _countof(priority0),	idx, true );

	reparentBiOperators( pNode, priority1, _countof(priority1),	idx );
	reparentBiOperators( pNode, priority2, _countof(priority2),	idx );
	reparentBiOperators( pNode, priority3, _countof(priority3),	idx );
	reparentBiOperators( pNode, priority4, _countof(priority4),	idx );
	reparentBiOperators( pNode, priority5, _countof(priority5),	idx );
	reparentBiOperators( pNode, priority6, _countof(priority6),	idx );
	reparentBiOperators( pNode, priority7, _countof(priority7),	idx );
	reparentBiOperators( pNode, priority8, _countof(priority8),	idx );
}

//==================================================================
}
