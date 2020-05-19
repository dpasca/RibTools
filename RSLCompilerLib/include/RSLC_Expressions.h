//==================================================================
/// RSLC_Expressions.h
///
/// Created by Davide Pasca - 2009/11/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_EXPRESSIONS_H
#define RSLC_EXPRESSIONS_H

//==================================================================
namespace RSLC
{

void SolveExpressions( class TokNode *pNode, bool processShaderOnly );
void SolveGlobalConstants( TokNode *pRoot );
void SolveVariablesDetail( TokNode *pRoot );

void OptimizeConstantExpressions( TokNode *pNode );

bool IsArrayItemAssignment( const TokNode *pSqBrkNode );

//==================================================================
}

#endif
