//==================================================================
/// RSLC_Arrays.h
///
/// Created by Davide Pasca - 2010/3/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_ARRAYS_H
#define RSLC_ARRAYS_H

//==================================================================
namespace RSLC
{

class TokNode;

void RealizeArraysSizes( TokNode *pNode );

void GenerateArrayAssignOperators( TokNode *pNode );

//==================================================================
}

#endif
