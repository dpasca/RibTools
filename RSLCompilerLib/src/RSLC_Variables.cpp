//==================================================================
/// RSLC_Variables.cpp
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Variables.h"
#include "RSLC_Exceptions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static VarType varTypeFromToken( Token *pTok )
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
void Variable::BuildSetupRegister( int &io_regIdx )
{
	DASSERT( !mBuild_Register.IsValid() );

	mBuild_Register.mIsVarying	= mIsVarying;
	mBuild_Register.mVarType	= varTypeFromToken( mpDTypeTok );
	mBuild_Register.mRegIdx		= io_regIdx++;
}

//==================================================================
static void AddVariable(
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
		pVar->mInternalName =
				DUT::SSPrintFS( "_%i@_%s",
							pNode->mBlockID,
								pVar->mpDefNameTok->str.c_str() );
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
	}
}

//==================================================================
static bool fndVarDefBeginInBlock(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	* &out_pDTypeNode,
		TokNode	* &out_pDetailNode
		)
{
	if ( pChild0 &&
		 (pChild0->mpToken->idType == T_TYPE_DATATYPE ||
		  pChild0->mpToken->idType == T_TYPE_DETAIL ) )
	{
		// 3 possible cases

		// 1) TYPE NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) )
		{
			out_pDTypeNode	= pChild0;
			out_pDetailNode	= NULL;
			i += 1;
		}
		else	// 2) TYPE DETAIL NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DETAIL) )
		{
			out_pDTypeNode	= pChild0;
			out_pDetailNode	= pChild1;
			i += 2;
		}
		else	// 3) DETAIL TYPE NTERM
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DATATYPE) )
		{
			out_pDTypeNode	= pChild1;
			out_pDetailNode	= pChild0;
			i += 2;
		}
		else
		{
			throw Exception( "Broken variable declaration", pChild0->mpToken );
		}

		return true;
	}

	return false;
}

/*
//==================================================================
static bool fndVarInExpr(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	*pChild2
		)
{
	if ( pChild0->mpToken->idType == T_TYPE_DATATYPE )
	{
		// 2 possible cases
		// 1) DT (+constructor params)
		// 2) DT CAST (+constructor params)

		// always varying..

		TokNode *pSpaceCast = NULL;

		if ((pChild1 && pChild1->mpToken->id == T_VL_STRING) )
		{
			pSpaceCast = pChild1;
			i += 1;
		}

		i += 1;

		AddVariable( pNode, pChild0, NULL, pSpaceCast, NULL );

		return true;
	}

	return false;
}
*/

//==================================================================
static bool iterateNextExpression( size_t &i, TokNode *pNode )
{
	for (++i; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->IsTokenID( T_OP_COMMA ) )
		{
			++i;
			return true;
		}
		else
		if ( pNode->mpChilds[i]->IsTokenID( T_OP_SEMICOL ) ||
			 pNode->mpChilds[i]->IsTokenID( T_OP_RGT_BRACKET ) )
		{
			++i;
			return false;
		}
	}

	//throw Exception( "Missing a ; ?" );

	return false;
}

//==================================================================
static void discoveFunctionParamsDeclaration( TokNode *pNode, size_t &i )
{
	TokNode	*pDTypeNode	= NULL;
	TokNode	*pDetailNode= NULL;
	TokNode	*pOutputNode= NULL;
	bool	prevWasComma = false;

	for (; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pA = pNode->GetChildTry( i + 0 );

		if NOT( pA )
			throw Exception( "Broken definition ?", pA );

		// end of param list
		if ( pA->mpToken->id == T_OP_RGT_BRACKET )
			break;

		if ( pA->mpToken->idType == T_TYPE_DATATYPE )
		{
			if ( pDTypeNode )
				if ( prevWasComma )
				{
					pDTypeNode = NULL;
					pDetailNode= NULL;
					pOutputNode= NULL;
				}
				else
					throw Exception( "Data type specified twice !", pA );

			pDTypeNode = pA;
		}
		else
		if ( pA->mpToken->idType == T_TYPE_DETAIL )
		{
			if ( pDetailNode )
				if ( prevWasComma )
				{
					pDTypeNode = NULL;
					pDetailNode= NULL;
					pOutputNode= NULL;
				}
				else
					throw Exception( "Detail specified twice !", pA );

			pDetailNode = pA;
		}
		else
		if ( pA->mpToken->id == T_KW_output )
		{
			if ( pOutputNode )
				if ( prevWasComma )
				{
					pDTypeNode = NULL;
					pDetailNode= NULL;
					pOutputNode= NULL;
				}
				else
					throw Exception( "'output' specified twice !", pA );

			pOutputNode = pA;
		}
		else
		if ( pA->mpToken->id == T_OP_COMMA )
		{
			// next...
		}
		else
		if ( pA->mpToken->id == T_OP_SEMICOL )
		{
			// reset current type, etc
			pDTypeNode = NULL;
			pDetailNode= NULL;
			pOutputNode= NULL;
		}
		else
		if ( pA->mpToken->idType == T_TYPE_NONTERM )
		{
			// Note: "space cast" only for expressions ?
			AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pA );
		}
		//else
		//	throw Exception( "Broken definition ?", pA );

		prevWasComma = (pA->mpToken->id == T_OP_COMMA);
	}
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
			 blkType == BLKT_FNPARAMS		// $$$ NOT REALLY THE SAME ..but for now, it's ok
			 )
		{
			discoveFunctionParamsDeclaration( pNode, i );
		}
		else
		if ( blkType == BLKT_CODEBLOCK )
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
								pDTypeNode,
								pDetailNode ) )
				{
					for (; i < pNode->mpChilds.size();)
					{
						TokNode	*pVarName = pNode->GetChildTry( i );

						if NOT( pVarName->IsNonTerminal() )
							throw Exception( "Expecting a variable name !" );
						
						// no "space cast" in the declaration in the curl braces
						AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pVarName );

						if NOT( iterateNextExpression( i, pNode ) )
							break;
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
			DiscoverVariablesDeclarations( pNode->mpChilds[i] );
		}
	}
}

/*
//==================================================================
static bool isVarUsedAsLValue( const char *pVarName, TokNode *pNode )
{
	for (TokNode *pNode2 = pNode; pNode2; pNode2 = pNode2->GetNext())
	{
	}

	// recurse on all children that are NOT code blocks (because
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		isVarUsedAsLValue( pVarName, pNode->mpChilds[i] );
	}
}
*/

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
	static const char *sGlobalsDefs[] =
	{
		"color"	,	"varying"	,	"Cs"	,	// also uniform ?
		"color"	,	"varying"	,	"Os"	,	// also uniform ?
		"point"	,	"varying"	,	"P"	,
		//"point"	,	"varying"	,	"Ps"	,
		"point"	,	"varying"	,	"dPdu"	,
		"point"	,	"varying"	,	"dPdv"	,
		"vector",	"varying"	,	"N"	,
		"vector",	"varying"	,	"Ng"	,	// also uniform ?
		"float"	,	"varying"	,	"u"	,
		"float"	,	"varying"	,	"v"	,
		"float"	,	"varying"	,	"du"	,	// also uniform ?
		"float"	,	"varying"	,	"dv"	,	// also uniform ?
		"float"	,	"varying"	,	"s"	,		// also uniform ?
		"float"	,	"varying"	,	"t"	,		// also uniform ?
		"vector",	"varying"	,	"L"	,		// also uniform ?
		"color"	,	"varying"	,	"Cl"	,	// also uniform ?
		"vector",	"varying"	,	"I"	,
		"color"	,	"varying"	,	"Ci"	,
		"color"	,	"varying"	,	"Oi"	,
		"point"	,	"uniform"	,	"E"	,
		//""	,	""	,	"A"	,
	};

	for (size_t i=0; i < _countof(sGlobalsDefs); i += 3)
	{
		AddVariable(
				pNode,
				newVarNode( sGlobalsDefs[i+0] ),
				newVarNode( sGlobalsDefs[i+1] ),
				NULL,
				newVarNode( sGlobalsDefs[i+2] ) );
	}
}

//==================================================================
static void scanWriteVars( FILE *pFile, TokNode *pNode )
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

				fprintf_s( pFile, "\t" );

				fprintf_s( pFile, "%-18s", var.mInternalName.c_str() );

				fprintf_s( pFile, "\t" );

				fprintf_s( pFile, "parameter" );
/*
				switch ( pNode->GetBlockType() )
				{
				case BLKT_SHPARAMS:		fprintf_s( pFile, "parameter" );	break;
				case BLKT_CODEBLOCK:	fprintf_s( pFile, "temporary" );	break;
				case BLKT_EXPRESSION:	fprintf_s( pFile, "temporary" );	break;
				}
*/

				fprintf_s( pFile, "\t" );

				if ( var.mIsVarying )
					fprintf_s( pFile, "varying" );
				else
					fprintf_s( pFile, "uniform" );

				if ( var.mpDTypeTok )
				{
					fprintf_s( pFile, "\t" );
					switch ( var.mpDTypeTok->id )
					{
					case T_DT_float:	fprintf_s( pFile, "float" );	break;
					case T_DT_vector:	fprintf_s( pFile, "vector" );	break;
					case T_DT_point:	fprintf_s( pFile, "point" );	break;
					case T_DT_normal:	fprintf_s( pFile, "normal" );	break;
					case T_DT_color:	fprintf_s( pFile, "color" );	break;
					case T_DT_string:	fprintf_s( pFile, "string" );	break;

					default:
						throw Exception( "Bad type ?!", var.mpDTypeTok );
						break;
					}
				}

				fprintf_s( pFile, "\n" );
			}

			fprintf_s( pFile, "\n" );
		}
	}


	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		scanWriteVars( pFile, pNode->mpChilds[i] );
	}
}

//==================================================================
void WriteVariables( FILE *pFile, TokNode *pNode )
{
	scanWriteVars( pFile, pNode );
}

//==================================================================
}
