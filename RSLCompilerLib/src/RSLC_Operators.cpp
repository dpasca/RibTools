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

//==================================================================
namespace RSLC
{

//==================================================================
static void doReparent( TokNode *pOper, size_t &out_parentIdx )
{
	// get leaves on the left and right of the operand
	TokNode	*pLValue = pOper->GetLeft();
	TokNode	*pRValue = pOper->GetRight();

	if ( pRValue && pLValue )
	{
		//	L	=	R

		if ( pRValue )
		{
			// Handle the funky RSL return value typecast. Example:
			// val = float texture( "yoyo" )
			// col = color texture( "yoyo" )

			// if R a data type ? (float, color, etc)
			if ( pRValue->IsDataType() )
			{
				TokNode	*pFuncNode = pRValue->GetRight();

				// is it followed by a function call ?
				if ( pFuncNode && pFuncNode->mNodeType == TokNode::TYPE_FUNCCALL )
				{
					// set the the return value cast data type
					pFuncNode->mFuncCall.mReturnCastVType = VarTypeFromToken( pRValue->mpToken );

					// we can now remove R...
					DASSERT( pRValue->mpChilds.size() == 0 );
					pRValue->UnlinkFromParent();
					DSAFE_DELETE( pRValue );

					// R becomes the function call node ..which we proceed to
					// make it into a child for the operator
					pRValue = pFuncNode;
				}
			}

			pRValue->Reparent( pOper );
			pOper->mpChilds.push_front( pRValue );
		}
		//	L	=
		//			R

		if ( pLValue )
		{
			pLValue->Reparent( pOper );
			pOper->mpChilds.push_front( pLValue );
		}
		//		=
		//	L		R

		DASSERT( out_parentIdx > 0 );
		out_parentIdx -= 1;
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

		if ( pNode->mpParent )
		{
			if ( excludeIfFuncParam )
			{
				BlockType	blkType = pNode->mpParent->GetBlockType();

				if (   blkType == BLKT_CALL_PARAMS_FN
					|| blkType == BLKT_DECL_PARAMS_FN
					|| blkType == BLKT_CALL_OR_DELC_PARAMS_FN )
					dontProcess = true;
			}
		}

		if NOT( dontProcess )
		{
			for (size_t i=0; i < matchIDsN; ++i)
			{
				if ( pMatchingIDs[i] == pNode->GetTokID() )
				{
					doReparent( pNode, out_parentIdx );
					break;
				}
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		reparentBiOperators(
					pNode->mpChilds[i],
					pMatchingIDs,
					matchIDsN,
					i,
					excludeIfFuncParam );
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
		T_OP_LFT_SQ_BRACKET		
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

	reparentBiOperators( pNode, priority0, _countof(priority0), idx, true );

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
