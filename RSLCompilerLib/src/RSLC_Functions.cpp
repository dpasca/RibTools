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

		pFuncName->mNodeType = TokNode::TYPE_FUNCDEF;	// mark the node as a function definition
	}
}

//==================================================================
static void discoverFuncsUsage( TokNode *pNode, const DVec<Function> &funcs, int &out_parentIdx )
{
	bool	isDataType = pNode->IsDataType();

	if ( (pNode->IsNonTerminal() ||
		  pNode->IsStdFunction() ||
		  isDataType) &&
			pNode->mNodeType == TokNode::TYPE_STANDARD )
	{
		TokNode *pFuncCallNode = pNode;

		const char *pStr = pFuncCallNode->GetTokStr();

/*
		for (size_t i=0; i < funcs.size(); ++i)
		{
			if ( strcmp( pStr, funcs[i].mpNameTok->GetStrChar() ) )
				continue;
*/

			TokNode *pLeftBracket = pFuncCallNode->GetRight();

			if ( pLeftBracket && pLeftBracket->mpToken->id == T_VL_STRING )
			{
				if ( isDataType )
				{
					// ok, it's just a space specifier ..next should be a bracket
					pLeftBracket = pLeftBracket->GetRight();
				}
				else
				{
					throw Exception( "Unknown usage, why a string ?",  pNode );
				}
			}

			if ( pLeftBracket && pLeftBracket->mpToken->id == T_OP_LFT_BRACKET )
			{
				// set the block type as a function call
				pLeftBracket->UpdateBlockTypeToFuncCall();

				//if ( pLeftBracket->mpToken->id == T_OP_LFT_BRACKET )
				{
					/*
						b
						(
						)

						..becomes..

						b
							(
							)

						..the loop above will skip one, so we need to backpedal..
					*/

					out_parentIdx -= 1;

					pLeftBracket->Reparent( pFuncCallNode );
					pFuncCallNode->mpChilds.push_back( pLeftBracket );

					pFuncCallNode->mNodeType = TokNode::TYPE_FUNCCALL;
				}
/*
				else
				{
					throw Exception( "Expecting a left bracket !", pLeftBracket->mpToken );
				}
*/
			}
//		}
	}

	for (int i=0; i < (int)pNode->mpChilds.size(); ++i)
	{
		discoverFuncsUsage( pNode->mpChilds[i], funcs, i );
	}
}

/*

	a( b( 1 ), 2 )

	$p0 = 1
	$t0 = b

	$p0 = $t0
	$p1 = 2

	call a

*/

//==================================================================
void DiscoverFunctions( TokNode *pRoot )
{
	discoverFuncsDeclarations( pRoot );

	int	idx = 0;
	discoverFuncsUsage( pRoot, pRoot->GetFuncs(), idx );
}

//==================================================================
static const char *getOper( TokNode *pOperand, char *pOutBuff, size_t maxOutSize )
{
	if ( pOperand->mpToken->idType == T_TYPE_OPERATOR )
	{
		// drill down if necessary
		while ( pOperand->mTempRegIdx == -1 )
		{
			if ( pOperand->mpChilds.size() == 0 )
				return "BADDDD";

			// expect at least one children
			DASSERT( pOperand->mpChilds.size() > 0 );
			pOperand = pOperand->mpChilds[0];
		}

		sprintf_s( pOutBuff, maxOutSize, "$t%i", pOperand->mTempRegIdx );

		return pOutBuff;
	}
	else
	{
		if ( pOperand->mpVarDef )
			return pOperand->mpVarDef->mInternalName.c_str();
		else
			return pOperand->GetTokStr();
	}
}

/*

*
	+
		a
		b

	+
		c
		d

	$t0 = + a b
	$t1 = + c d

$t2 = +	$t0	$t1

*/

//==================================================================
static void buildExpression( FILE *pFile, TokNode *pNode, size_t &io_tempIdx )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		if ( pNode->mpChilds[i]->mpToken->idType == T_TYPE_OPERATOR )
		{
			buildExpression( pFile, pNode->mpChilds[i], io_tempIdx );
		}
	}

	if NOT( pNode->mpToken->IsBiOp() )
		return;

	TokNode *pOperand1 = pNode->GetChildTry( 0 );
	TokNode *pOperand2 = pNode->GetChildTry( 1 );

	if ( pOperand1 && pOperand2 )
	{
		char	op1NameBuff[32];
		char	op2NameBuff[32];

		const char	*pO1Str = getOper( pOperand1, op1NameBuff, _countof(op1NameBuff) );
		const char	*pO2Str = getOper( pOperand2, op2NameBuff, _countof(op2NameBuff) );

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
		{
			// a one time assignment only..
			DASSERT( pNode->mTempRegIdx == -1 );
			pNode->mTempRegIdx = io_tempIdx++;

			fprintf_s( pFile, "\t%s\t$t%i\t%s\t%s\n",
							pNode->GetTokStr(),
								pNode->mTempRegIdx,
									pO1Str,
										pO2Str );
		}
	}
	else
	{
		//fprintf_s( pFile, "\nWEIRDDD !! ( %s )\n", pNode->GetTokStr() );
	}
}

//==================================================================
static void generateExpressions( FILE *pFile, TokNode *pCodeBlkNode )
{
	for (size_t i=0; i < pCodeBlkNode->mpChilds.size(); ++i)
	{
		TokNode	*pNode = pCodeBlkNode->mpChilds[i];

		size_t	tempIdx = 0;
		// are we assigning something 
		//if ( pNode->mpToken->IsAssignOp() )
			buildExpression( pFile, pNode, tempIdx );
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
