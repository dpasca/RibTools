//==================================================================
/// RSLC_Functions.cpp
///
/// Created by Davide Pasca - 2009/5/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"

//==================================================================
namespace RSLC
{

//==================================================================
static void discoverFuncsDeclarations( TokNode *pRoot )
{
	DVec<Function>	&funcs = pRoot->GetFuncs();

	for (size_t i=0; i < pRoot->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pRoot->mpChilds[i];

		if NOT( pNode->IsCodeBlock() )
			continue;

		TokNode	*pParamsBlk	= NULL;
		TokNode	*pFuncName	= NULL;
		TokNode	*pRetType	= NULL;

		if ( !(pParamsBlk	= pRoot->GetChildTry(i-2)) ||
			 !(pFuncName	= pRoot->GetChildTry(i-3)) ||
			 !(pRetType		= pRoot->GetChildTry(i-4)) )
		{
			throw Exception( "Broken function definition",  pNode->mpToken );
		}

		Function	*pFunc = funcs.grow();

		pFunc->mpCodeBlkNode	= pNode;
		pFunc->mpNameTok		= pFuncName->mpToken;
		pFunc->mpRetTypeTok		= pRetType->mpToken;
	}
}

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot );
}

//==================================================================
static const char *getOper( TokNode *pOperand )
{
	if ( pOperand->mpToken->idType == T_TYPE_OPERATOR )
		return "$t0";
	else
	{
		if ( pOperand->mpVarDef )
			return pOperand->mpVarDef->mInternalName.c_str();
		else
			return pOperand->GetTokStr();
	}
}

//==================================================================
static void buildExpression( FILE *pFile, const TokNode *pNode )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->mpToken->idType == T_TYPE_OPERATOR )
			buildExpression( pFile, pNode->mpChilds[i] );
	}

	if NOT( pNode->mpToken->idType == T_TYPE_OPERATOR )
		return;

	TokNode *pOperand1 = pNode->GetChildTry( 0 );
	TokNode *pOperand2 = pNode->GetChildTry( 1 );

	if ( pOperand1 && pOperand2 )
	{
		const char	*pO1Str = getOper( pOperand1 );
		const char	*pO2Str = getOper( pOperand2 );

		if ( pNode->mpToken->IsAssignOp() )
		{
			if ( pNode->mpToken->id == T_OP_ASSIGN )
				fprintf_s( pFile, "\tmov\t%s\t%s\n", pO1Str, pO2Str );
			else
				fprintf_s( pFile, "\t%s\t%s\t%s\t%s\n",
							pNode->GetTokStr(),
								pO1Str,
									pO1Str,
										pO2Str );

			fprintf_s( pFile, "\n" );
		}
		else
		if ( pNode->mpToken->id == T_OP_ASSIGN )
		{
			fprintf_s( pFile, "\tmov\t%s\t%s\n", pO1Str, pO2Str );
		}
		else
		{
			fprintf_s( pFile, "\t%s\t$t0\t%s\t%s\n",
							pNode->GetTokStr(),
								pO1Str,
									pO2Str );
		}
	}
	else
	{
		fprintf_s( pFile, "\nWEIRDDD !! ( %s )\n", pNode->GetTokStr() );
	}

}

//==================================================================
static void generateExpressions( FILE *pFile, const TokNode *pCodeBlkNode )
{
	for (size_t i=0; i < pCodeBlkNode->mpChilds.size(); ++i)
	{
		const TokNode	*pNode = pCodeBlkNode->mpChilds[i];

		// are we assigning something 
		if ( pNode->mpToken->IsAssignOp() )
			buildExpression( pFile, pNode );
	}
}

//==================================================================
void WriteFunctions( FILE *pFile, TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		fprintf_s( pFile, "\n;====================================\n" );

		if ( func.mpRetTypeTok->idType == T_TYPE_SHADERTYPE )
		{
			fprintf_s( pFile, "%s %s\n",
							func.mpRetTypeTok->GetStrChar(),
								func.mpNameTok->GetStrChar() );
		}
		else
		{
			fprintf_s( pFile, "function %s\n", func.mpNameTok->GetStrChar() );
		}

		generateExpressions( pFile, func.mpCodeBlkNode );

		fprintf_s( pFile, "\n\tret\n" );
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
}
