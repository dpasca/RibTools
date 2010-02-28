//==================================================================
/// RSLC_Functions.h
///
/// Created by Davide Pasca - 2009/5/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_FUNCTIONS_H
#define RSLC_FUNCTIONS_H

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

class TokNode;

//==================================================================
inline bool IsAsmFunc( const char *pFuncName )
{
	return pFuncName == strstr( pFuncName, "_asm_" );
}

//==================================================================
class Function
{
public:
	TokNode			*mpParamsNode;
	TokNode			*mpCodeBlkNode;
	Token			*mpRetTypeTok;
	TokNode			*mpNameNode;
	VarType			mRetVarType;
	DVec<VarType>	mParamsVarTypes;

	Function() :
		mpParamsNode(NULL),
		mpCodeBlkNode(NULL),
		mpRetTypeTok(NULL),
		mpNameNode(NULL),
		mRetVarType(VT_UNKNOWN)
	{
	}

	~Function()
	{
	}

	bool IsShader() const
	{
		return mpRetTypeTok->idType == T_TYPE_SHADERTYPE;
	}

	bool IsAsmFunc() const;

	bool IsFuncOp() const	{	return mpRetTypeTok->id == T_KW___funcop; }

	bool HasParams() const;
};

//==================================================================
void DiscoverFunctions( TokNode *pRoot, u_int &io_blockCnt );

const Function *MatchFunctionByParams( TokNode *pFCallNode, const DVec<Function> &funcs );

void ResolveFunctionCalls( TokNode *pNode );

void CloseFuncOps( TokNode *pNode );

//==================================================================
}

#endif
