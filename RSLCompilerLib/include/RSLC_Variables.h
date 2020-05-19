//==================================================================
/// RSLC_Variables.h
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_VARIABLES_H
#define RSLC_VARIABLES_H

#include "RSLC_Token.h"
#include "RSLC_Variable.h"

//==================================================================
namespace RSLC
{

//==================================================================
class TokNode;
class Function;

//==================================================================
Variable *AddVariable(
			TokNode *pNode,
			TokNode *pDTypeNode,
			TokNode *pDetailNode,
			TokNode *pSpaceCastTok,
			TokNode *pNameNode,
			bool	isArray,
			bool	isOutput );

void AddSelfVariable(
			TokNode *pNode,
			VarType	varType,
			bool	isVarying,
			bool	isDetailForced );

//==================================================================
VarType VarTypeFromToken( const Token *pTok );
const char *VarTypeToString( VarType type );
char VarTypeToLetter( VarType type );
bool IsSameVarType( VarType vta, VarType vtb, bool laxCheck );

//==================================================================
void MarkUsedVariables( TokNode *pRoot );
void DiscoverVariablesDeclarations( TokNode *pNode );
void DiscoverVariablesUsage( TokNode *pNode );

//==================================================================
}

#endif
