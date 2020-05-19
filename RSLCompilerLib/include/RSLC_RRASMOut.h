//==================================================================
/// RSLC_RRASMOut.h
///
/// Created by Davide Pasca - 2010/1/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_RRASMOUT_H
#define RSLC_RRASMOUT_H

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
namespace RRASMOut
{

void WriteVariables( FILE *pFile, TokNode *pNode );
void WriteFunctions( FILE *pFile, TokNode *pNode );

}

//==================================================================
}

#endif
