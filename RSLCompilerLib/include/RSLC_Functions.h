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
class Function
{
public:
	TokNode			*mpParamsNode;
	TokNode			*mpCodeBlkNode;
	Token			*mpRetTypeTok;
	TokNode			*mpNameNode;
	DVec<VarType>	mParamsVarTypes;

	Function() :
		mpParamsNode(NULL),
		mpCodeBlkNode(NULL),
		mpRetTypeTok(NULL),
		mpNameNode(NULL)
	{
	}

	~Function()
	{
	}

	bool IsShader() const
	{
		return mpRetTypeTok->idType == T_TYPE_SHADERTYPE;
	}

};

//==================================================================
void DiscoverFunctions( TokNode *pRoot );

void DiscoverFuncopsUsage( TokNode *pRoot );
void ReparentFuncopsStatements( TokNode *pRoot );

void ResolveFunctionCalls( TokNode *pNode );

void WriteFunctions( FILE *pFile, TokNode *pNode );

//==================================================================
}

#endif
