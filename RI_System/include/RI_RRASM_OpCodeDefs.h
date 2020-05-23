//==================================================================
/// RI_RRASM_OpCodeDefs.h
///
/// Created by Davide Pasca - 2010/1/10
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_RRASM_OPCODEDEFS_H
#define RI_RRASM_OPCODEDEFS_H

#include "RI_SVM_Shader.h"
#include "RI_Symbol.h"

//==================================================================
namespace RI
{
//==================================================================
namespace RRASM
{

//==================================================================
static const u_int OPC_FLG_1STISDEST		= 1	;	// first param is destination value
static const u_int OPC_FLG_RIGHTISIMM		= 2	;	// 2nd and above params are immediate
static const u_int OPC_FLG_UNIFORMOPERS		= 4	;	// all ops must be uniform
static const u_int OPC_FLG_FUNCOP_BEGIN		= 8	;	// it's a funcop begin instruction
static const u_int OPC_FLG_FUNCOP_MIDDLE	= 16;	// it's a funcop middle instruction
static const u_int OPC_FLG_FUNCOP_END		= 32;	// it's a funcop end instruction
static const u_int OPC_FLG_DIRPOSLIGHT		= 64;	// it's dealing with positional and directional light

//==================================================================
struct OpCodeDef
{
    const char		*pName;
    u_int			OperCnt;
    u_int			Flags;
    Symbol::Type	Types[5];
};

extern OpCodeDef	_gOpCodeDefs[];

//==================================================================
}
//==================================================================
}

#endif
