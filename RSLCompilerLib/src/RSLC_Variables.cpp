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
#include "RSLC_Defs_StdVars.h"
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
	case T_DT_string: return VT_STRING;
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
	case VT_POINT	: return "point"	;
	case VT_COLOR	: return "color"	;
	case VT_STRING	: return "string"	;
	case VT_VECTOR	: return "vector"	;
	case VT_NORMAL	: return "normal"	;
	case VT_MATRIX	: return "matrix"	;
	case VT_BOOL	: return "bool"		;

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
	case VT_POINT:	return 'v'; break;
	case VT_COLOR:	return 'v'; break;
	case VT_STRING:	return 'x'; break;
	case VT_VECTOR:	return 'v'; break;
	case VT_NORMAL:	return 'v'; break;
	case VT_MATRIX:	return 'm'; break;
	case VT_BOOL:	return 'b'; break;

	default:
		DASSERT( 0 );
		return '!';
	}
}

//==================================================================
void Variable::AssignRegister( int regIdx )
{
	DASSERT( !mBuild_Register.IsValid() );

	mBuild_Register.SetType( mVarType, mIsVarying, mIsForcedDetail );
	mBuild_Register.SetRegIdx( regIdx );
}

//==================================================================
bool Variable::IsRegisterAssigned() const
{
	return mBuild_Register.IsAssigned();
}

//==================================================================
RSLC::VarType Variable::GetVarType() const
{
	DASSERT( !mBuild_Register.IsValid() || mBuild_Register.GetVarType() == mVarType );

	return mVarType;
}

//==================================================================
bool Variable::IsVarying() const
{
	DASSERT( !mBuild_Register.IsValid() || mBuild_Register.IsVarying() == mIsVarying );

	return mIsVarying;
}

//==================================================================
bool Variable::IsForcedDetail() const
{
	DASSERT( mBuild_Register.IsForcedDetail() == mIsForcedDetail );

	return mIsForcedDetail;
}

//==================================================================
void Variable::SetVarying( bool varying )
{
	DASSERT( mIsForcedDetail == false );

	mIsVarying = varying;

	if ( mBuild_Register.IsValid() )
	{
		mBuild_Register.SetVarying( varying );
	}
}

//==================================================================
std::string Variable::GetUseName() const
{
	if ( mIsGlobal || mIsSHParam )
		return mpDefNameTok->str;
	else
	if ( mBuild_Register.IsValid() )
	{
		return GetRegName( mBuild_Register );
	}
	else
		return mInternalName;
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
	pNameNode->mVarLink.mVarIdx = pNode->GetVars().size();
	pNameNode->mVarLink.mpNode = pNode;

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
			u_int	tmpID = pNode->GetVars().size() - 1;

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
		u_int	tmpID = pNode->GetVars().size() - 1;

		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_tmp%i", pNode->mBlockID, tmpID );
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
	pNode->mVarLink.mVarIdx = pNode->GetVars().size();
	pNode->mVarLink.mpNode = pNode;

	Variable	*pVar = pNode->GetVars().grow();

	pVar->mVarType			= varType;
	pVar->mIsVarying		= isVarying;
	pVar->mIsForcedDetail	= isDetailForced;
}

//==================================================================
void AddConstVariable( TokNode *pNode, TokNode *pRoot )
{
	DASSERT( pNode->mpToken->idType == T_TYPE_VALUE );

	VarType		vtype;
	float		floatVal = 0;
	const char	*pStrVal = NULL;

	DVec<Variable> &vars = pRoot->GetVars();

	// see if it matches and existing constant..

	if ( pNode->mpToken->id == T_VL_NUMBER )
	{
		vtype = VT_FLOAT;
		floatVal = (float)atof( pNode->GetTokStr() );

		for (size_t i=0; i < vars.size(); ++i)
		{
			if ( vars[i].IsConstant() )
			{
				if ( vars[i].mBaseValNum.size() == 1 &&
					 vars[i].mBaseValNum[0] == floatVal )
				{
					pNode->mVarLink.mVarIdx = i;
					pNode->mVarLink.mpNode = pRoot;
					return;
				}
			}
		}
	}
	else
	if ( pNode->mpToken->id == T_VL_STRING )
	{
		vtype = VT_STRING;
		pStrVal = pNode->GetTokStr();

		for (size_t i=0; i < vars.size(); ++i)
		{
			if ( vars[i].IsConstant() )
			{
				if ( vars[i].mBaseValNum.size() == 1 &&
					 0 == strcmp( vars[i].mBaseValStr.c_str(), pStrVal ) )
				{
					pNode->mVarLink.mVarIdx = i;
					pNode->mVarLink.mpNode = pRoot;
					return;
				}
			}
		}
	}
	else
	{
		DASSERT( 0 );
		return;
	}

	// ..if not found, then need to make it from scratch

	// setup the var link
	pNode->mVarLink.mVarIdx = pRoot->GetVars().size();
	pNode->mVarLink.mpNode = pRoot;

	Variable	*pVar = pRoot->GetVars().grow();

	pVar->mIsVarying		= false;
	pVar->mIsForcedDetail	= false;
	pVar->mHasBaseVal		= true;

	pVar->mInternalName		= DUT::SSPrintFS( "_@K%02i", pNode->mVarLink.mVarIdx );

	pVar->mVarType			= vtype;

	if ( pNode->mpToken->id == T_VL_NUMBER )
	{
		pVar->mBaseValNum.push_back( floatVal );
	}
	else
	if ( pNode->mpToken->id == T_VL_STRING )
	{
		pVar->mBaseValStr = pStrVal;
	}
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
			if ( out_pDTypeNode )
				throw Exception( "Broken variable declaration", pChilds[j] );

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
static bool iterateNextExpression( size_t &i, TokNode *pNode )
{
	for (++i; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->IsTokenID( T_OP_SEMICOL ) ||
			 pNode->mpChilds[i]->IsTokenID( T_OP_RGT_BRACKET ) )
		{
			++i;
			return false;
		}
		else
		if ( pNode->mpChilds[i]->IsTokenID( T_OP_COMMA ) )
		{
			++i;
			return true;
		}
	}

	//throw Exception( "Missing a ; ?" );

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
void DiscoverVariablesDeclarations( TokNode *pNode )
{
	size_t i = 0;

	if ( pNode->mpToken )
	{
		BlockType	blkType = pNode->GetBlockType();

		DASSERT(
			blkType == BLKT_SHPARAMS ||
			blkType == BLKT_FNPARAMS ||
			blkType == BLKT_CODEBLOCK ||
			blkType == BLKT_EXPRESSION
			);

		if ( blkType == BLKT_SHPARAMS ||	// $$$ NOT REALLY THE SAME ..but for now, it's ok
			 blkType == BLKT_FNPARAMS ||
			 blkType == BLKT_CODEBLOCK )
		{
			TokNode	*pDTypeNode	= NULL;
			TokNode	*pDetailNode= NULL;
			TokNode	*pOutputNode= NULL;

			for (; i < pNode->mpChilds.size(); ++i)
			{
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
						if ( pChild->IsNonTerminal() )
						{
							// no "space cast" in the declaration in the curl braces
							Variable *pVar = AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pChild );

							if ( blkType == BLKT_SHPARAMS )
							{
								pVar->mIsSHParam = true;	// mark as shader param
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

					if NOT( i < pNode->mpChilds.size() )
						break;
				}
			}
		}
	}

	for (; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->mpToken->id == T_OP_LFT_CRL_BRACKET ||
			 pNode->mpChilds[i]->mpToken->id == T_OP_LFT_BRACKET )
		{
			if ( i > 0 && 0 == strcmp( "vector", pNode->mpChilds[i-1]->GetTokStr() ) )
			{
				int yoyo = 1;
			}


			DiscoverVariablesDeclarations( pNode->mpChilds[i] );
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
static TokNode *newVarNode( const char *pStr )
{
	return DNEW TokNode( TokenFromDefOrNTerm( pStr ) );
}

//==================================================================
void AddStandardVariables( TokNode *pNode )
{
	for (size_t i=0; i < _countof(_gGlobalsDefs); i += 3)
	{
		Variable *pVar =
					AddVariable(
							pNode,
							newVarNode( _gGlobalsDefs[i+0] ),
							newVarNode( _gGlobalsDefs[i+1] ),
							NULL,
							newVarNode( _gGlobalsDefs[i+2] ) );

		pVar->mIsGlobal = true;
	}
}

//==================================================================
static size_t findStandardVariable( const char *pName )
{
	for (size_t i=0; i < _countof(_gGlobalsDefs); i += 3)
	{
		if ( 0 == strcasecmp( _gGlobalsDefs[i+2], pName ) )
			return i/3;
	}

	return DNPOS;
}

//==================================================================
static void collecedUsedStdVars_sub( TokNode *pNode, DVec<size_t> &io_usedStdVarsList )
{
	const Variable	*pVar = pNode->mVarLink.GetVarPtr();

	if ( pVar && pVar->mIsGlobal )
	{
		size_t	idx = findStandardVariable( pVar->mpDefNameTok->GetStrChar() );

		DASSERT( idx != DNPOS );

		io_usedStdVarsList.find_or_push_back( idx );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		collecedUsedStdVars_sub( pNode->mpChilds[i], io_usedStdVarsList );
	}
}

//==================================================================
void CollectUsedStdVars( TokNode *pRoot, DVec<size_t> &io_usedStdVarsList )
{
	for (size_t i=0; i < pRoot->GetFuncs().size(); ++i)
	{
		const Function	&func = pRoot->GetFuncs()[i];

		if NOT( func.IsShader() )
			continue;

		collecedUsedStdVars_sub( func.mpParamsNode, io_usedStdVarsList );
	}
}

//==================================================================
static void writeVariable( FILE *pFile, const Variable &var )
{
	fprintf_s( pFile, "\t" );

	fprintf_s( pFile, "%-12s\t", var.GetUseName().c_str() );

	const char *pStorage = "ERROR!!!";

	if ( var.mIsGlobal )	pStorage = "global"; else
	if ( var.mIsSHParam )	pStorage = "parameter"; else
	if ( var.mHasBaseVal )	pStorage = "constant";

	fprintf_s( pFile, "%-9s ", pStorage );

	if ( var.mIsVarying )
		fprintf_s( pFile, "varying " );
	else
		fprintf_s( pFile, "uniform " );

	fprintf_s( pFile, "%-7s ", VarTypeToString( var.GetVarType() ) );

	if ( var.mHasBaseVal )
	{
		// should be either numbers or a string.. but enough asserts for now !!

		for (size_t k=0; k < var.mBaseValNum.size(); ++k)
			fprintf_s( pFile, "\t%f", var.mBaseValNum[k] );

		if ( var.mBaseValStr.length() )
			fprintf_s( pFile, "\t\"%s\"", var.mBaseValStr.c_str() );
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
void WriteVariables( FILE *pFile, TokNode *pNode, const DVec<size_t> &usedStdVars )
{
	const DVec<Variable> &vars = pNode->GetVars();

	// write all used globals
	for (size_t i=0; i < usedStdVars.size(); ++i)
	{
		size_t j = usedStdVars[ i ] * 3;

		const Variable	&var = vars[i];

		DASSERT( var.mIsGlobal && !var.mBaseValNum.size() );

		fprintf_s( pFile, "\t%-12s\tglobal\t%s\t%s\n",
							_gGlobalsDefs[j+2],
							_gGlobalsDefs[j+1],
							_gGlobalsDefs[j+0] );
	}

	fprintf_s( pFile, "\n" );

	// write all vars that are in the root but that are not actually globals
	// ..most likely constants !

	for (size_t i=0; i < vars.size(); ++i)
	{
		const Variable	&var = vars[i];

		if ( !var.mIsGlobal && !var.mIsSHParam )
			writeVariable( pFile, var );
	}

	fprintf_s( pFile, "\n" );

	writeShaderParamVariables( pFile, pNode );
}

//==================================================================
}
