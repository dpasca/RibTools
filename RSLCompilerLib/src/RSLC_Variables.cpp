///==================================================================
/// RSLC_Variables.cpp
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <stdlib.h>
#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

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
Variable *AddVariable(
			TokNode *pNode,
			TokNode *pDTypeNode,
			TokNode *pDetailNode,
			TokNode *pSpaceCastTok,
			TokNode *pNameNode )
{
	// setup the var link
	if ( pNameNode )
	{
		pNameNode->mVarLink.Setup( pNode, pNode->GetVars().size() );
	}

	Variable	*pVar = pNode->GetVars().grow();

	pVar->mpDTypeTok		= pDTypeNode->mpToken;
	//pVar->mpOwnerNode		= pNode;
	pVar->mpDetailTok		= pDetailNode ? pDetailNode->mpToken : NULL;
	pVar->mpSpaceCastTok	= pSpaceCastTok ? pSpaceCastTok->mpToken : NULL;
	pVar->mpDefNameTok		= pNameNode ? pNameNode->mpToken : NULL;

	if ( pVar->mpDefNameTok )
	{
		if ( 0 == strcmp( pVar->mpDefNameTok->str.c_str(), "(" ) )
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
									pVar->mpDefNameTok->str.c_str() );
		}
	}
	else
	{
		u_int	tmpID = (u_int)pNode->GetVars().size() - 1;

		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_tmp%i", pNode->mBlockID, tmpID );
	}

	if ( pVar->mpDetailTok )
	{
		if ( pVar->mpDetailTok->idType != T_TYPE_DETAIL )
			throw Exception( "Bad detail type !", pVar->mpDetailTok );

/*
		// ensure that it's uniform for root parent
		if ( pNode->GetBlockType() == BLKT_ROOT )
			if ( pVar->mpDetailTok->id != T_DE_uniform )
				throw Exception( "Globals must be uniform !", pVar->mpDetailTok );
*/

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
			// local variables are varying by default, params uniform by default
			// ..see RenderMan Companion p.297
			if ( pNode->GetBlockType() == BLKT_CODEBLOCK || pNode->GetBlockType() == BLKT_EXPRESSION )
				pVar->mIsVarying = true;
			else
			if ( pNode->GetBlockType() == BLKT_SHPARAMS || pNode->GetBlockType() == BLKT_FNPARAMS )
				pVar->mIsVarying = false;
			else
				pVar->mIsVarying = true;

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

//==================================================================
static bool fndVarDefBeginInBlock(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	*pChild2,
		TokNode	* &out_pDTypeNode,
		TokNode	* &out_pDetailNode,
		TokNode	* &out_pOutputNode
		)
{
	out_pDTypeNode	= NULL;
	out_pDetailNode	= NULL;
	out_pOutputNode	= NULL;

	TokNode	*pChilds[3] = { pChild0, pChild1, pChild2 };

	for (size_t j=0; j < 3; ++j)
	{
		if NOT( pChilds[j] )
			break;

		if ( pChilds[j]->mpToken->idType == T_TYPE_DATATYPE )
		{
			// TEMP - allow point point()
			//if ( out_pDTypeNode )
			//	throw Exception( "Broken variable declaration", pChilds[j] );

			out_pDTypeNode = pChilds[j];
			++i;
		}
		else
		if ( pChilds[j]->mpToken->idType == T_TYPE_DETAIL )
		{
			if ( out_pDetailNode )
				throw Exception( "Broken variable declaration", pChilds[j] );

			out_pDetailNode = pChilds[j];
			++i;
		}
		else
		if ( pChilds[j]->mpToken->id == T_KW_output )
		{
			if ( out_pOutputNode )
				throw Exception( "Broken variable declaration", pChilds[j] );

			out_pOutputNode = pChilds[j];
			++i;
		}
	}

	if ( out_pDTypeNode || out_pDetailNode || out_pOutputNode )
		return true;
	else
		return false;
}

//==================================================================
static Function *findFunctionByNameNode( DVec<Function> &funcs, TokNode *pFindNode )
{
	for (size_t i=0; i < funcs.size(); ++i)
	{
		if ( funcs[i].mpNameNode == pFindNode )
			return &funcs[i];
	}

	return NULL;
}

//==================================================================
static size_t discoverVariablesDeclarations_sub2( TokNode *pNode, size_t i )
{
	BlockType	blkType = pNode->GetBlockType();

	TokNode	*pDTypeNode	= NULL;
	TokNode	*pDetailNode= NULL;
	TokNode	*pOutputNode= NULL;

	while ( fndVarDefBeginInBlock(
					i,
					pNode,
					pNode->GetChildTry( i + 0 ),
					pNode->GetChildTry( i + 1 ),
					pNode->GetChildTry( i + 2 ),
					pDTypeNode,
					pDetailNode,
					pOutputNode ) )
	{
		if ( pOutputNode && blkType == BLKT_CODEBLOCK )
			throw Exception( "Keyword 'output' can be specified only in function and shader parameters declaration", pOutputNode );

		bool	isFunctionName = false;

		for (; i < pNode->mpChilds.size(); ++i)
		{
			TokNode	*pChild = pNode->GetChildTry( i );

			if ( pChild->IsTokenID( T_OP_SEMICOL ) ||
				 pChild->IsTokenID( T_OP_RGT_BRACKET ) )
			{
				++i;
				break;
			}
			else
			if ( pChild->IsTokenID( T_OP_COMMA ) )
			{
				continue;
			}
			else
			{
				// it's a variable declaration if it's non terminal..
				// ..and it's not followed by an opening bracket
				// ..or followed by no sibling.. which is the end of params in
				// a function/shader params block (ehm ?)
				if ( pChild->IsNonTerminal() )
				{
					TokNode	*pChild2 = pNode->GetChildTry( i+1 );

					isFunctionName = (pChild2 && pChild2->IsTokenID( T_OP_LFT_BRACKET ));
					if ( isFunctionName )
						break;

					if ( !isFunctionName )
					{
						if NOT( pDTypeNode )
							throw Exception( "Missing type for definition ?", pChild );

						// no "space cast" in the declaration in the curl braces
						Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pChild );

						if ( blkType == BLKT_SHPARAMS )
						{
							pVar->mIsSHParam = true;	// mark as shader param
						}
					}
				}
				else
				{
					if ( blkType == BLKT_SHPARAMS || blkType == BLKT_FNPARAMS )
					{
						// functions and shader params are allowed to change type after
						// a comma !

						if ( pChild->mpToken->idType == T_TYPE_DATATYPE )
							pDTypeNode = pChild;
						else
						if ( pChild->mpToken->idType == T_TYPE_DETAIL )
							pDetailNode = pChild;
					}
					//else
					//	throw Exception( "Expecting a variable name !" );
				}
			}
		}

		if ( i >= pNode->mpChilds.size() || isFunctionName )
			break;
	}

	return i;
}

// TODO: functions and shader params are allowed to change type after a comma !
//==================================================================
static size_t discoverVariablesDeclarations_rootBlock( TokNode *pNode, size_t i )
{
	TokNode	*pDTypeNode	= NULL;
	TokNode	*pDetailNode= NULL;
	TokNode	*pOutputNode= NULL;
	TokNode *pLastNonTerm=NULL;

	int		rndBracketCnt = 0;
	int		crlBracketCnt = 0;
	bool	intoRValue = false;

	BlockType	blkType = pNode->GetBlockType();

	for (; i < pNode->mpChilds.size(); ++i)
	{
		TokNode	*pChild = pNode->mpChilds[i];

		if ( pChild->mpToken->id == T_OP_LFT_BRACKET )
		{
			++rndBracketCnt;
			continue;
		}
		if ( pChild->mpToken->id == T_OP_LFT_CRL_BRACKET )
		{
			++crlBracketCnt;
			continue;
		}
		else
		if ( pChild->mpToken->id == T_OP_RGT_BRACKET )
		{
			--rndBracketCnt;
			continue;
		}
		if ( pChild->mpToken->id == T_OP_RGT_CRL_BRACKET )
		{
			--crlBracketCnt;
			continue;
		}
		else
		{
			// no variable declarations possible inside any expression (round brackets)
			if ( rndBracketCnt )
				continue;

			// only inside code block, curl bracket allow for variables declaration
			if ( crlBracketCnt )
			{
				if ( blkType != BLKT_CODEBLOCK )
					continue;
			}
		}

		// NOTE: a valid variable declaration must be followed by any of the following symbols
		// ',', ';', '='

		if ( pChild->mpToken->id == T_OP_ASSIGN )
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
		if ( pChild->mpToken->id == T_OP_COMMA )
		{
			// comma brings out of "right value" state.
			// e.g. float a = 4, c ...
			intoRValue = false;

			if ( pDTypeNode && pLastNonTerm )
			{
				Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm );
				if ( blkType == BLKT_SHPARAMS )
				{
					pVar->mIsSHParam = true;	// mark as shader param
				}
			}
		}
		else
		if ( pChild->mpToken->id == T_OP_SEMICOL )
		{
			// semi-colon brings out of "right value" state and resets
			// all types, detail..
			// e.g. float a = 4, c;
			intoRValue = false;

			if ( pDTypeNode && pLastNonTerm )
			{
				Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm );
				if ( blkType == BLKT_SHPARAMS )
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
			if ( pChild->mpToken->idType == T_TYPE_DATATYPE )
			{
				pDTypeNode = pChild;
			}
			else
			if ( pChild->mpToken->idType == T_TYPE_DETAIL )
			{
				if ( pDetailNode )
					throw Exception( "Broken variable declaration", pChild );

				pDetailNode = pChild;
			}
			else
			if ( pChild->mpToken->id == T_KW_output )
			{
				if ( pOutputNode && (blkType != BLKT_SHPARAMS && blkType != BLKT_FNPARAMS) )
					throw Exception( "Keyword 'output' can be specified only in function and shader parameters declaration", pChild );

				pOutputNode = pChild;
			}
			else
			if ( pChild->IsNonTerminal() )
			{
				pLastNonTerm = pChild;
			}
		}
	}

	// check for last declaration in case of shadow or function params
	if ( (blkType == BLKT_SHPARAMS || blkType == BLKT_FNPARAMS) && pLastNonTerm && pDTypeNode )
	{
		Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pLastNonTerm );		
		if ( blkType == BLKT_SHPARAMS )
		{
			pVar->mIsSHParam = true;	// mark as shader param
		}
	}

	return i;
}

//==================================================================
static size_t discoverVariablesDeclarations_sub( TokNode *pNode, size_t i )
{
	for (; i < pNode->mpChilds.size(); ++i)
	{
		TokNode	*pTokNode = pNode->GetChildTry( i + 0 );

		if (pTokNode->mpToken->idType != T_TYPE_DATATYPE &&
			pTokNode->mpToken->idType != T_TYPE_DETAIL &&
			pTokNode->mpToken->idType != T_KW_output )
		{
			continue;
		}

		if ( pNode->GetBlockType() == BLKT_ROOT )
			i = discoverVariablesDeclarations_rootBlock( pNode, i );
		else
			i = discoverVariablesDeclarations_rootBlock( pNode, i );
			//i = discoverVariablesDeclarations_sub2( pNode, i );
	}

	return i;
}

//==================================================================
void DiscoverVariablesDeclarations( TokNode *pNode )
{
	for (size_t i = 0; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->mpToken->id == T_OP_LFT_CRL_BRACKET ||
			 pNode->mpChilds[i]->mpToken->id == T_OP_LFT_BRACKET )
		{
			DiscoverVariablesDeclarations( pNode->mpChilds[i] );
		}
	}

	BlockType	blkType = pNode->GetBlockType();

	if (	blkType == BLKT_SHPARAMS	// $$$ NOT REALLY THE SAME ..but for now, it's ok
		 || blkType == BLKT_FNPARAMS
		 || blkType == BLKT_CODEBLOCK
		 || blkType == BLKT_ROOT )
	{
		for (size_t i=0; i < pNode->mpChilds.size();)
		{
			i = discoverVariablesDeclarations_sub( pNode, i );
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
	{
		DiscoverVariablesUsage( pNode->mpChilds[i] );
	}
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

		if NOT( pNode->mpToken->id == T_OP_ASSIGN )
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
			DASSERT(
				pDestVar->mBaseVal.mNumVec.size() == 0 &&
				pExprVar->mBaseVal.mNumVec.size() == 1
				);

			pDestVar->mBaseVal.mNumVec.push_back(
						pExprVar->mBaseVal.mNumVec[0] );
			pDestVar->mBaseVal.mUse = true;
		}
		else
		{
			throw Exception( "Sorry constant float values only !", pExpr );
		}
	}
}

//==================================================================
static void writeVariable( FILE *pFile, const Variable &var )
{
	fprintf_s( pFile, "\t" );

	fprintf_s( pFile, "%-12s\t", var.GetUseName().c_str() );

	//--- write storage
	const char *pStorage = "ERROR!!!";

	if ( var.mBaseVal.mUse )	pStorage = "temporary"; else
	if ( var.mIsGlobal )	pStorage = "global"; else
	if ( var.mIsSHParam )	pStorage = "parameter"; else
	{
		DASSERT( 0 );
	}

	fprintf_s( pFile, "%-9s ", pStorage );

	//--- write class (constant, uniform, varying (no "vertex" for now))
	const char *pClass;
	if ( var.mIsVarying )
	{
		DASSTHROW( var.mBaseVal.mUse == false,
				("Internal error ! Variable '%s' is varying but has a base value !",
					var.GetUseName().c_str()) );

		pClass = "varying";
	}
	else
	{
		if ( var.mBaseVal.mUse )
			pClass = "constant";
		else
			pClass = "uniform";
	}
	fprintf_s( pFile, "%-8s ", pClass );

	//--- write the name
	VarType	varType = var.GetVarType();

	fprintf_s( pFile, "%-7s ", VarTypeToString( varType ) );

	//--- write the eventual constant value
	if ( var.mBaseVal.mUse )
	{
		fprintf_s( pFile, "\t" );

		// should be either bool, numbers or a string..
		if ( varType == VT_BOOL )
		{
			fprintf_s( pFile, "%i", var.mBaseVal.mBool ? 1 : 0 );
		}
		else
		if ( varType == VT_STRING )
		{
			if ( var.mBaseVal.mStr.length() )
				fprintf_s( pFile, "\"%s\"", var.mBaseVal.mStr.c_str() );
		}
		else
		{
			for (size_t k=0; k < var.mBaseVal.mNumVec.size(); ++k)
				fprintf_s( pFile, "%f", var.mBaseVal.mNumVec[k] );
		}
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
// $$$ doesn't really need to be recursive..
static void writeShaderParamVariables( FILE *pFile, TokNode *pNode )
{
	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		//fprintf_s( pFile, "\t; -- vars for block %i\n", pNode->GetBlockID() );

		if ( pNode->GetBlockType() == BLKT_SHPARAMS )
		{
			for (size_t i=0; i < vars.size(); ++i)
			{
				const Variable	&var = vars[i];

				writeVariable( pFile, var );
			}
		}
	}


	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		writeShaderParamVariables( pFile, pNode->mpChilds[i] );
	}
}

//==================================================================
void WriteVariables( FILE *pFile, TokNode *pNode )
{
	const DVec<Variable> &vars = pNode->GetVars();

	// write all used global vars and constants

	for (size_t i=0; i < vars.size(); ++i)
	{
		const Variable	&var = vars[i];

		//if ( !var.mIsSHParam && ((var.mIsGlobal && var.mIsUsed) || !var.mIsGlobal) )
		if ( !var.mIsSHParam && var.mIsUsed )
			writeVariable( pFile, var );
	}

	fprintf_s( pFile, "\n" );

	writeShaderParamVariables( pFile, pNode );
}

//==================================================================
}
