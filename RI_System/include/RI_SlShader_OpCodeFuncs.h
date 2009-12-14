//==================================================================
/// RI_SlShader_OpCodeFuncs.h
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPCODEFUNCS_H
#define RI_SLSHADER_OPCODEFUNCS_H

//==================================================================
namespace RI
{

typedef void (*SlOpCodeFunc)( SlRunContext &ctx );

SlOpCodeFunc	_gSlOpCodeFuncs[];

//==================================================================
}

#endif
