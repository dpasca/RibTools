//==================================================================
/// RSLC_RRASMOut_WriteFunctions.cpp
///
/// Created by Davide Pasca - 2009/9/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Functions.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

//==================================================================
namespace RRASMOut
{

//==================================================================
static const std::string getOperand(
					TokNode *pOperand,
					VarType &out_varType )
{
	out_varType = VT_UNKNOWN;

	//DASSERT( pOperand->mpParent && pOperand->mpParent->mpChilds.size() >= 1 );
	while ( pOperand )
	{
		const Variable	*pVar = pOperand->GetVarPtr();

		if ( pVar )
		{
			// this should be the common (if not unique ?) case
			out_varType = pVar->GetVarType();
			return pVar->GetUseName();
		}
		else
		if ( pOperand->GetTokIDType() == T_TYPE_VALUE )
		{
			out_varType = VT_FLOAT;
			return pOperand->GetTokStr();
		}
/*
		else
		{
			DASSERT( 0 );
			Register	reg = pOperand->GetRegister();

			if ( reg.IsValid() )
			{
				out_varType = reg.GetVarType();

				return GetRegName( reg );
			}
		}
*/

		pOperand = pOperand->GetChildTry( 0 );
	}

	//DASSERT( 0 );

	return "WHAT ?!";
}

//==================================================================
static void writeFuncParams( FILE *pFile, TokNode *pNode )
{
	if ( pNode->mpParent &&
		 pNode->mpParent->mpParent &&
		 pNode->mpParent->mpParent->mpToken->IsAssignOp() )
	{
		TokNode *pChild = pNode->mpParent->mpParent->GetChildTry( 0 );

		Register	reg = pChild->GetRegister();

		std::string regName = reg.GetName();

		fprintf_s( pFile, " %s", regName.c_str() );
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		TokNode *pChild = pNode->mpChilds[ i ];

		if ( pChild->GetTokID() == T_OP_COMMA )
			continue;

		VarType	varType;
		std::string opStr = getOperand( pChild, varType );

		fprintf_s( pFile, " %-6s", opStr.c_str() );
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
static const char *asmOpCodeFromOpToken( const Token *pTok )
{
	switch ( pTok->id )
	{
	case T_OP_MUL:		return "mul";
	case T_OP_DIV:		return "div";
	case T_OP_PLUS:		return "add";
	case T_OP_MINUS:	return "sub";
	case T_OP_POW:		return "pow";
	case T_OP_DOT:		return "dot";

	case T_OP_LSEQ	:	return "setle";
	case T_OP_GEEQ	:	return "setge";
	case T_OP_LSTH	:	return "setlt";
	case T_OP_GRTH	:	return "setgt";
	case T_OP_EQ	:	return "seteq";
	case T_OP_NEQ	:	return "setne";

	default:
		return pTok->str.c_str();
	}
}

//==================================================================
static std::string resolveIntrinsics( const char *pIntrName )
{
	static size_t		sBaseLen;
	static const char	*spBaseStr = "_asm_";

	if ( sBaseLen == 0 )
		sBaseLen = strlen( "_asm_" );

	if ( pIntrName == strstr( pIntrName, spBaseStr ) )
	{
		std::string	str = pIntrName + sBaseLen;

		for (size_t i=0; i < str.length(); ++i)
		{
			if ( str[i] == '_' )
				str[i] = '.';
		}

		return str; 
	}
	else
		return "UNKNOWN_INTRINSIC";
}

//==================================================================
static void buildExpression_biOp( FILE *pFile, TokNode *pNode )
{
	TokNode *pOperand1 = pNode->GetChildTry( 0 );
	TokNode *pOperand2 = pNode->GetChildTry( 1 );

	// no assignment for function call as it's done above
	if ( pNode->GetTokID() == T_OP_ASSIGN &&
		 pOperand1->mNodeType == TokNode::TYPE_FUNCCALL )
		return;

	if ( pOperand1 && pOperand2 )
	{
		VarType	varType1;
		VarType	varType2;

		std::string	o1Str = getOperand( pOperand1, varType1 );
		std::string	o2Str = getOperand( pOperand2, varType2 );

		bool	doAssign = pNode->mpToken->IsAssignOp();

		char l1 = VarTypeToLetter(varType1);
		char l2 = VarTypeToLetter(varType2);

		char	instrBuff[256];

		if ( doAssign )
		{
			if ( pNode->GetTokID() == T_OP_ASSIGN )
			{
				// build either a load instruction or a move
/*
				if ( isValue2 )
					fprintf_s(
							pFile,
							"\tld%c\t%s\t%s\n",
							l1,
							o1Str.c_str(),
							o2Str.c_str() );
				else
*/
				// rudimentary form of optimization !
				// WARNING: careful with jumps ?
				if NOT( l1 == l2 && o1Str == o2Str )
				{
					sprintf_s( instrBuff, "mov.%c%c", l1, l2 );

					fprintf_s(
							pFile,
							"\t%-14s %-6s\t%-6s\n",
							instrBuff,
							o1Str.c_str(),
							o2Str.c_str() );
				}
			}
			else
			{
				sprintf_s( instrBuff, "%s.%c%c", asmOpCodeFromOpToken( pNode->mpToken ), l1, l2 );

				fprintf_s(
						pFile,
						"\t%-14s %-6s\t%-6s\t%-6s\n",
						instrBuff,
						o1Str.c_str(),
						o1Str.c_str(),
						o2Str.c_str() );
			}
		}
		else
		{
			Register	reg = pNode->GetRegister();
			std::string regName = reg.GetName();

			char		lreg = VarTypeToLetter( reg.GetVarType() );

			sprintf_s( instrBuff, "%s.%c%c%c", asmOpCodeFromOpToken( pNode->mpToken ), lreg, l1, l2 );

			fprintf_s(
					pFile,
					"\t%-14s %-6s\t%-6s\t%-6s\n",
					instrBuff,
					regName.c_str(),
					o1Str.c_str(),
					o2Str.c_str() );
		}
	}
}

//==================================================================
static void buildExpression( FILE *pFile, TokNode *pNode )
{
	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		buildExpression( pFile, pNode->mpChilds[i] );
	}

	if ( pNode->mNodeType == TokNode::TYPE_FUNCCALL )
	{
		size_t	nextChild = 0;
		TokNode *pChild = pNode->GetChildTry( nextChild );

		std::string	asmName = resolveIntrinsics( pNode->GetTokStr() );

		if ( pChild && pChild->GetTokID() == T_VL_STRING )
		{
			fprintf_s( pFile, "\t%-14s %s", asmName.c_str(), pChild->GetTokStr() );
			pChild = NULL;
			++nextChild;
		}
		else
			fprintf_s( pFile, "\t%-14s", asmName.c_str() );

		TokNode *pBracket = pNode->GetChildTry( nextChild );

		// get the params bracket.. if any !
		if ( pBracket )
		{
			DASSERT( pBracket->GetTokID() == T_OP_LFT_BRACKET );
			writeFuncParams( pFile, pBracket );
		}
		else
			fprintf_s( pFile, "\n" );
	}
	else
	if ( pNode->mpToken->IsBiOp() )
	{
		buildExpression_biOp( pFile, pNode );
	}
}

//==================================================================
static void writeDefParams( FILE *pFile, const Function &func )
{
	fprintf_s( pFile, "\n\t;==== Begin Def Params\n" );

	for (size_t j=0; j < func.mpParamsNode->mpChilds.size(); ++j)
	{
		TokNode	*pParamNode = func.mpParamsNode->mpChilds[j];

		// are we assigning something ?
		if ( pParamNode->mpToken->IsAssignOp() )
		{
			// write the sub-routine for assignment..

			// what node we assigning to ?
			TokNode	*pDestAssignNode = pParamNode->GetChildTry( 0 );
			DASSERT( pDestAssignNode != NULL );

			// what variable are we assigning to ?
			const Variable	*pLVar = pDestAssignNode->GetVarPtr();
			DASSTHROW( pLVar != NULL, ("Default parameter broken ?!") );

			// write the label with the variable's name
			fprintf_s( pFile, "__defparam_%s:\n", pLVar->GetUseName().c_str() );

			// write the assignment expression
			buildExpression( pFile, pParamNode );

			// ..and the return command
			fprintf_s( pFile, "\tret\n" );
		}
	}

	fprintf_s( pFile, "\t;==== End Def Params\n\n" );
}

//==================================================================
void WriteFunctions( FILE *pFile, TokNode *pNode )
{
	const DVec<Function> &funcs = pNode->GetFuncs();

	for (size_t i=0; i < funcs.size(); ++i)
	{
		const Function	&func = funcs[i];

		if NOT( func.IsShader() )
			continue;

		DASSERT( func.mpRetTypeTok->idType == T_TYPE_SHADERTYPE );

		fprintf_s( pFile, "\n;====================================\n" );

		fprintf_s( pFile, "%s ;== %s\n",
						func.mpRetTypeTok->GetStrChar(),
							func.mpNameNode->GetTokStr() );

		// write eventual code for default parameters
		if ( func.mpParamsNode )
		{
			writeDefParams( pFile, func );
		}

		// write the entry point finally !
		fprintf_s( pFile, "__main:\n" );

		for (size_t j=0; j < func.mpCodeBlkNode->mpChilds.size(); ++j)
		{
			TokNode	*pNode = func.mpCodeBlkNode->mpChilds[j];

			buildExpression( pFile, pNode );
		}

		fprintf_s( pFile, "\n\tret\n" );
	}

	fprintf_s( pFile, "\n" );
}

//==================================================================
}

//==================================================================
}
