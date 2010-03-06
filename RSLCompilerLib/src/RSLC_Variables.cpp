///==================================================================
/// RSLC_Variables.cpp
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdlib.h>
#include <math.h>
#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"
#include "RSLC_Expressions.h"

//==================================================================
namespace RSLC
{

//==================================================================
VarType VarTypeFromToken( const Token *pTok )
{
	DASSERT( pTok->idType == T_TYPE_DATATYPE );

	switch ( pTok->id )
	{
	case T_DT_float	: return VT_FLOAT;
	case T_DT_vector: return VT_VECTOR;
	case T_DT_point	: return VT_POINT;
	case T_DT_normal: return VT_NORMAL;
	case T_DT_color	: return VT_COLOR;
	case T_DT_matrix: return VT_MATRIX;
	case T_DT_string: return VT_STRING;
	case T_DT_bool:	  return VT_BOOL;
	case T_DT_void:	  return VT_VOID;
	}

	DASSERT( 0 );
	return VT_FLOAT;
}

//==================================================================
const char *VarTypeToString( VarType type )
{
	switch ( type )
	{
	case VT_FLOAT	: return "float"	;
	case VT_VECTOR	: return "vector"	;
	case VT_POINT	: return "point"	;
	case VT_NORMAL	: return "normal"	;
	case VT_COLOR	: return "color"	;
	case VT_MATRIX	: return "matrix"	;
	case VT_STRING	: return "string"	;
	case VT_BOOL	: return "bool"		;
	case VT_VOID	: return "void"	;

	default:
	case VT_UNKNOWN	: return "UNKNOWN"	;
	}
}

//==================================================================
char VarTypeToLetter( VarType type )
{
	switch ( type )
	{
	case VT_FLOAT:	return 's'; break;
	case VT_VECTOR:	return 'v'; break;
	case VT_POINT:	return 'v'; break;
	case VT_NORMAL:	return 'v'; break;
	case VT_COLOR:	return 'v'; break;
	case VT_MATRIX:	return 'm'; break;
	case VT_STRING:	return 'x'; break;
	case VT_BOOL:	return 'b'; break;

	default:
		DASSERT( 0 );
		return '!';
	}
}

//==================================================================
bool IsSameVarType( VarType vta, VarType vtb, bool laxTypes )
{
	if ( laxTypes )
	{
		switch ( vta )
		{
		//case VT_VECTOR:
		case VT_POINT:
		case VT_NORMAL:
			vta = VT_VECTOR;
		}

		switch ( vtb )
		{
		//case VT_VECTOR:
		case VT_POINT:
		case VT_NORMAL:
			vtb = VT_VECTOR;
		}
	}

	return vta == vtb;
}

//==================================================================
Variable *AddVariable(
			TokNode *pNode,
			TokNode *pDTypeNode,
			TokNode *pDetailNode,
			TokNode *pSpaceCastTok,
			TokNode *pNameNode,
			bool	isArray )
{
	// setup the var link
	if ( pNameNode )
	{
		pNameNode->mVarLink.Setup( pNode, pNode->GetVars().size() );
	}

	DASSERT( pNameNode->mpToken != NULL );

	Variable	*pVar = pNode->GetVars().grow();

	pVar->mpDTypeTok		= pDTypeNode->mpToken;
	//pVar->mpOwnerNode		= pNode;
	pVar->mpDetailTok		= pDetailNode ? pDetailNode->mpToken : NULL;
	pVar->mpSpaceCastTok	= pSpaceCastTok ? pSpaceCastTok->mpToken : NULL;
	pVar->mpDefNameNode		= pNameNode;
	pVar->mIsArray			= isArray;

	if ( 0 == strcmp( pVar->mpDefNameNode->GetTokStr(), "(" ) )
	{
		u_int	tmpID = (u_int)pNode->GetVars().size() - 1;

		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_brkt%i", pNode->mBlockID, tmpID );
	}
	else
	{
		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_%s",
							pNode->mBlockID,
								pVar->mpDefNameNode->GetTokStr() );
	}

	if ( pVar->mpDetailTok )
	{
		if ( pVar->mpDetailTok->idType != T_TYPE_DETAIL )
			throw Exception( "Bad detail type !", pVar->mpDetailTok );

		if ( pVar->mpDetailTok->id == T_DE_varying )
			pVar->mIsVarying = true;
		else
			pVar->mIsVarying = false;

		pVar->mIsForcedDetail = true;	// detail was specified
	}
	else
	{
		// special case for root parent
		if ( pNode->GetBlockType() == BLKT_ROOT )
		{
			pVar->mIsVarying = false;
			pVar->mIsForcedDetail = true;
		}
		else
		{
			BlockType	blkType = pNode->GetBlockType();
			// local variables are varying by default, params uniform by default
			// ..see RenderMan Companion p.297
			switch ( blkType )
			{
			case BLKT_CODEBLOCK:
			case BLKT_EXPRESSION:
				pVar->mIsVarying = true;
				break;

			case BLKT_DECL_PARAMS_SH:
			case BLKT_DECL_PARAMS_FN:
			case BLKT_CALL_OR_DELC_PARAMS_FN:
				pVar->mIsVarying = false;
				break;

			default:
				pVar->mIsVarying = true;
				break;
			}

			pVar->mIsForcedDetail = false;	// detail temporarily assumed
		}
	}

	// if the parent is root, then it's a global
	if ( pNode->GetBlockType() == BLKT_ROOT )
		pVar->mIsGlobal = true;

	pVar->mVarType = VarTypeFromToken( pVar->mpDTypeTok );

	return pVar;
}

//==================================================================
void AddSelfVariable(
			TokNode *pNode,
			VarType	varType,
			bool	isVarying,
			bool	isDetailForced )
{
	//DASSERT( pNode->GetVars().size() == 0 );

	// setup the var link
	pNode->mVarLink.Setup( pNode, pNode->GetVars().size() );

	Variable	*pVar = pNode->GetVars().grow();

	pVar->mVarType			= varType;
	pVar->mIsVarying		= isVarying;
	pVar->mIsForcedDetail	= isDetailForced;
}

// TODO: functions and shader params are allowed to change type after a comma !
//==================================================================
static size_t discoverVariablesDeclarations_sub( TokNode *pNode, size_t i )
{
	TokNode	*pDTypeNode	= NULL;
	TokNode	*pDetailNode= NULL;
	TokNode	*pOutputNode= NULL;
	TokNode *pLastNonTerm=NULL;

	bool	intoRValue = false;
	bool	isArray = false;

	BlockType	blkType = pNode->GetBlockType();

	for (; i < pNode->mpChilds.size(); ++i)
	{
		TokNode	*pChild = pNode->mpChilds[i];

		// NOTE: a valid variable declaration must be followed by any of the following symbols
		// ',', ';', '='

		if ( pChild->GetTokID() == T_OP_ASSIGN )
		{
			// except in cases like: float a = (b = 3)
			if ( intoRValue )
				throw Exception( "Broken variable declaration", pChild );

			intoRValue = true;

			if ( pDTypeNode )
			{
				DASSERT( !!pLastNonTerm );
			}
		}
		else
		if ( pChild->GetTokID() == T_OP_COMMA )
		{
			// comma brings out of "right value" state.
			// e.g. float a = 4, c ...
			intoRValue = false;

			if ( pDTypeNode && pLastNonTerm )
			{
				Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm, isArray );
				if ( blkType == BLKT_DECL_PARAMS_SH )
				{
					pVar->mIsSHParam = true;	// mark as shader param
				}
			}
		}
		else
		if ( pChild->GetTokID() == T_OP_SEMICOL )
		{
			// semi-colon brings out of "right value" state and resets
			// all types, detail..
			// e.g. float a = 4, c;
			intoRValue = false;

			if ( pDTypeNode && pLastNonTerm )
			{
				Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm, isArray );
				if ( blkType == BLKT_DECL_PARAMS_SH )
				{
					pVar->mIsSHParam = true;	// mark as shader param
				}
			}

			pDTypeNode	= NULL;
			pDetailNode	= NULL;
			pOutputNode	= NULL;
		}
		else
		if NOT( intoRValue )
		{
			if ( pChild->GetTokIDType() == T_TYPE_DATATYPE )
			{
				pDTypeNode = pChild;
			}
			else
			if ( pChild->GetTokIDType() == T_TYPE_DETAIL )
			{
				if ( pDetailNode )
					throw Exception( "Broken variable declaration", pChild );

				pDetailNode = pChild;
			}
			else
			if ( pChild->GetTokID() == T_KW_output )
			{
				if ( pOutputNode &&
					(	blkType != BLKT_DECL_PARAMS_SH
					 && blkType != BLKT_DECL_PARAMS_FN
					 && blkType != BLKT_CALL_OR_DELC_PARAMS_FN) )
				{
					throw Exception(
							"Keyword 'output' can be specified only in "
							"function and shader parameters declaration", pChild );
				}

				pOutputNode = pChild;
			}
			else
			if ( pChild->IsNonTerminal() )
			{
				pLastNonTerm = pChild;

				const TokNode	*pChildChild = pChild->GetChildTry( 0 );

				// is this maybe an array declaration ?
				if ( pChildChild && pChildChild->GetTokID() == T_OP_LFT_SQ_BRACKET )
					isArray = true;
			}
		}
	}

	// check for last declaration in case of shader or function params
	if (
		(	blkType == BLKT_DECL_PARAMS_SH
		 || blkType == BLKT_DECL_PARAMS_FN
		 || blkType == BLKT_CALL_OR_DELC_PARAMS_FN )

		 && pLastNonTerm
		 && pDTypeNode )
	{
		Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm, isArray );		
		if ( blkType == BLKT_DECL_PARAMS_SH )
		{
			pVar->mIsSHParam = true;	// mark as shader param
		}
	}

	return i;
}

//==================================================================
void DiscoverVariablesDeclarations( TokNode *pNode )
{
	for (size_t i = 0; i < pNode->mpChilds.size(); ++i)
		DiscoverVariablesDeclarations( pNode->mpChilds[i] );

	BlockType	blkType = pNode->GetBlockType();

	if (	blkType == BLKT_DECL_PARAMS_SH
		 || blkType == BLKT_DECL_PARAMS_FN
		 || blkType == BLKT_CALL_OR_DELC_PARAMS_FN
		 || blkType == BLKT_CODEBLOCK
		 || blkType == BLKT_ROOT )
	{
		for (size_t i=0; i < pNode->mpChilds.size();)
		{
			TokNode	*pTokNode = pNode->GetChildTry( i );
			TokenIDType	idType = pTokNode->GetTokIDType();

			if (idType == T_TYPE_DATATYPE ||
				idType == T_TYPE_DETAIL ||
				idType == T_KW_output )
			{
				i = discoverVariablesDeclarations_sub( pNode, i );
			}
			else
				++i;
		}
	}
}

//==================================================================
void DiscoverVariablesUsage( TokNode *pNode )
{
	// for every non-terminal, and that doesn't have a var link (declarations would already..)
	if ( pNode->IsNonTerminal() && !pNode->mVarLink.IsValid() )
	{
		// scan backward and up, looking for a variable definition matching the name
		for (TokNode *pVarDefBlock = pNode->mpParent; pVarDefBlock; pVarDefBlock = pVarDefBlock->mpParent)
		{
			// do we have variables declaration ?
			if ( pVarDefBlock->GetVars().size() )
			{
				VarLink varLink = pVarDefBlock->FindVariableByDefName( pNode->GetTokStr() );
				if ( varLink.IsValid() )
				{
					pNode->mVarLink = varLink;
					break;
				}
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		DiscoverVariablesUsage( pNode->mpChilds[i] );
}

//==================================================================
void RealizeArraysSizes( TokNode *pNode )
{
	DVec<Variable>	&vars = pNode->GetVars();
	for (size_t i=0; i < vars.size(); ++i)
	{
	    if NOT( vars[i].mIsArray )
			continue;

		TokNode *pSqBracket = vars[i].mpDefNameNode->GetChildTry( 0 );
		DASSERT( pSqBracket != NULL );

		TokNode	*pArrSizeNode = pSqBracket->GetChildTry( 0 );
		DASSERT( pArrSizeNode != NULL );

		double	arrSize = pArrSizeNode->mpToken->ConstNum;
		U64		arrSizeInt = (U64)ceil( arrSize );

		// if it's negative or if it's not an integral value
		if ( arrSize < 0 || arrSizeInt != arrSize )
			throw Exception( pSqBracket, "Array size must be positive integral value" );

		// enforce a "large enough" maximum size
		if ( arrSize > (double)(1 << 24) )
			throw Exception( pSqBracket, "Array size is currently limited to %i !", 1 << 24 );

		// finally set the array size
		vars[i].mArraySize = (U32)arrSizeInt;
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		RealizeArraysSizes( pNode->mpChilds[i] );
}

//==================================================================
static void markUsedVariables_sub( TokNode *pNode )
{
	Variable	*pVar = pNode->mVarLink.GetVarPtr();

	if ( pVar )
	{
		pVar->mIsUsed = true;
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		markUsedVariables_sub( pNode->mpChilds[i] );
	}
}

//==================================================================
void MarkUsedVariables( TokNode *pRoot )
{
	for (size_t i=0; i < pRoot->GetFuncs().size(); ++i)
	{
		const Function	&func = pRoot->GetFuncs()[i];

		if NOT( func.IsShader() )
			continue;

		markUsedVariables_sub( func.mpParamsNode );
	}
}

//==================================================================
void SolveGlobalConstants( TokNode *pRoot )
{
	for (size_t i=0; i < pRoot->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pRoot->mpChilds[i];

		if NOT( pNode->GetTokID() == T_OP_ASSIGN )
			continue;

		TokNode	*pDest = pNode->GetChildTry( 0 );
		TokNode	*pExpr = pNode->GetChildTry( 1 );

		DASSERT( pDest->mVarLink.IsValid() );
		DASSERT( pExpr->mVarLink.IsValid() );

		// TODO: support constants defined by actual constant expression
		// solvable at compile time !

		if ( pExpr->mpChilds.size() )
			throw Exception( "Sorry constant values only, no expressions for now !", pExpr );

		const Variable *pExprVar = pExpr->mVarLink.GetVarPtr();
			  Variable *pDestVar = pDest->mVarLink.GetVarPtr();

		if ( pDestVar->GetVarType() == VT_FLOAT )
		{
			pDestVar->mBaseVal.Set( pExprVar->GetBaseValFloat() );
		}
		else
		{
			throw Exception( "Sorry constant float values only !", pExpr );
		}
	}
}

//==================================================================
}
