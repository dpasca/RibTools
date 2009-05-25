//==================================================================
/// RSLCompiler.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DContainers.h"
#include "RSLC_Token.h"
#include "RSLC_Tree.h"
#include "RSLCompiler.h"

//==================================================================
using namespace	RSLC;

//==================================================================
static void AddVariable(
					TokNode *pNode,
					TokNode *pDTypeNode,
					TokNode *pDetailNode,
					TokNode *pSpaceCastTok,
					TokNode *pNameNode )
{
	Variable	*pVar = pNode->GetData().mVariables.grow();

	pVar->mpDTypeTok		= pDTypeNode->mpToken;
	pVar->mpDetailTok		= pDetailNode ? pDetailNode->mpToken : NULL;
	pVar->mpSpaceCastTok	= pSpaceCastTok ? pSpaceCastTok->mpToken : NULL;
	pVar->mpNameTok			= pNameNode ? pNameNode->mpToken : NULL;
}

//==================================================================
static bool fndVarInBlock(
		size_t &i,
		TokNode *pNode,
		TokNode	*pChild0,
		TokNode	*pChild1,
		TokNode	*pChild2
		)
{
	if ( pChild0->mpToken->idType == T_TYPE_DATATYPE ||
		 pChild0->mpToken->idType == T_TYPE_DETAIL )
	{
		// 3 possible cases
		// 1) DT UN
		// 2) DT DE UN
		// 3) DE DT UN

		TokNode	*pDTypeNode = NULL;
		TokNode	*pDetailNode = NULL;
		TokNode	*pNameNode = NULL;

		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_UNKNOWN) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= NULL;
			pNameNode	= pChild1;
			i += 2;
		}
		else
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_UNKNOWN) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild1;
			pNameNode	= pChild2;
			i += 3;
		}
		else
		if ((pChild0 && pChild0->mpToken->idType == T_TYPE_DETAIL) &&
			(pChild1 && pChild1->mpToken->idType == T_TYPE_DATATYPE) &&
			(pChild2 && pChild2->mpToken->idType == T_TYPE_UNKNOWN) )
		{
			pDTypeNode	= pChild0;
			pDetailNode	= pChild2;
			pNameNode	= pChild1;
			i += 3;
		}
		else
		{
			DASSTHROW( 0, ("Broken variable declaration at line %i", pChild0->mpToken->sourceLine ) );
		}

		// no "space cast" in the declaration in the curl braces
		AddVariable( pNode, pDTypeNode, pDetailNode, NULL, pNameNode );

		return true;
	}

	return false;
}

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

//==================================================================
static void FindVariables( TokNode *pNode )
{
	// $$$ Note: should assume root to be one big curl bracket..
	if ( pNode->mpToken &&
		 pNode->mpToken->id != T_OP_LFT_CRL_BRACKET &&
		 pNode->mpToken->id != T_OP_LFT_BRACKET )
	{
		return;
	}

	if ( pNode->mpToken )
	{
		DASSERT(
			pNode->mAreaType == AT_SHPARAMS ||
			pNode->mAreaType == AT_CODEBLOCK ||
			pNode->mAreaType == AT_EXPRESSION
			);

		for (size_t i=0; i < pNode->mpChilds.size();)
		{
			TokNode	*pChild0 = pNode->mpChilds[i];
			TokNode	*pChild1 = NULL;
			TokNode	*pChild2 = NULL;

			if ( (i+1) < pNode->mpChilds.size() ) pChild1 = pNode->mpChilds[i + 1];
			if ( (i+2) < pNode->mpChilds.size() ) pChild2 = pNode->mpChilds[i + 2];

			if ( pNode->mAreaType == AT_CODEBLOCK ||
				 pNode->mAreaType == AT_SHPARAMS	// $$$ NOT REALLY THE SAME ..but for now, it's ok
				 )
			{
				if NOT( fndVarInBlock( i, pNode, pChild0, pChild1, pChild2 ) )
				{
					++i;
					continue;
				}
			}
			else
			if ( pNode->mAreaType == AT_EXPRESSION )
			{
				if NOT( fndVarInExpr( i, pNode, pChild0, pChild1, pChild2 ) )
				{
					++i;
					continue;
				}
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		FindVariables( pNode->mpChilds[i] );
	}
}

//==================================================================
RSLCompiler::RSLCompiler( const char *pSource, size_t sourceSize )
{
	Tokenizer( mTokens, pSource, sourceSize );

	for (size_t i=0; i < mTokens.size(); ++i)
	{
		printf( "%3i) %i - %-12s - %s\n",
				i,
				mTokens[i].isPrecededByWS,
				GetTokenTypeStr( mTokens[i].idType ),
				mTokens[i].str.c_str() );
	}

	mpRoot = MakeTree( mTokens );

	FindVariables( mpRoot );

	TraverseTree( mpRoot, 0 );
}

//==================================================================
RSLCompiler::~RSLCompiler()
{

}

//==================================================================
static void scanSaveVars( FILE *pFile, TokNode *pNode, size_t &blockCnt )
{
	const DVec<Variable> &vars = pNode->GetVars();

	if ( vars.size() )
	{
		fprintf_s( pFile, "\t; -- vars for block %i\n", blockCnt );

		for (size_t i=0; i < vars.size(); ++i)
		{
			const Variable	&var = vars[i];

			fprintf_s( pFile, "\t" );

			switch ( pNode->mAreaType )
			{
			case AT_SHPARAMS:	fprintf_s( pFile, "parameter" );	break;
			case AT_CODEBLOCK:	fprintf_s( pFile, "temporary" );	break;
			case AT_EXPRESSION:	fprintf_s( pFile, "temporary" );	break;
			}

			if ( var.mpDetailTok )
			{
				fprintf_s( pFile, "\t" );
				switch ( var.mpDetailTok->id )
				{
				case T_DE_varying:	fprintf_s( pFile, "varying" );	break;
				case T_DE_uniform:	fprintf_s( pFile, "uniform" );	break;
				case T_UNKNOWN:										break;

				default:
					DASSTHROW( 0, ("Bad detail ?!") );
					break;
				}
			}

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
					DASSTHROW( 0, ("Bad type ?!") );
					break;
				}
			}
			
			if ( var.mpNameTok )
			{
				fprintf_s( pFile, "\t%s", var.mpNameTok->str.c_str() );
			}

			fprintf_s( pFile, "\n" );
		}

		blockCnt += 1;
		fprintf_s( pFile, "\n" );
	}


	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		scanSaveVars( pFile, pNode->mpChilds[i], blockCnt );
	}
}

//==================================================================
void RSLCompiler::SaveASM( const char *pFName )
{
	FILE	*pFile;

	if ( fopen_s( &pFile, pFName, "wb" ) )
	{
		DASSTHROW( 0, ("Failed to save %s", pFName) );
	}

	size_t	blockCnt = 0;
	scanSaveVars( pFile, mpRoot, blockCnt );

	fclose( pFile );
}

