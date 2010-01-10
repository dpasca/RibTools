//==================================================================
/// RI_SVM_OpCodeFuncs.h
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPCODEFUNCS_H
#define RI_SVM_OPCODEFUNCS_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

typedef void (*SlOpCodeFunc)( Context &ctx );

SlOpCodeFunc	_gSlOpCodeFuncs[];

//==================================================================
}
//==================================================================
}

#endif
