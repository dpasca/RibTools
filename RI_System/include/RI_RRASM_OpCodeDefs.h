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

//==================================================================
namespace RI
{
//==================================================================
namespace RRASM
{

//==================================================================
#define OPC_FLG_RIGHTISIMM			1
#define OPC_FLG_UNIFORMOPERS		2
#define OPC_FLG_FUNCOP_BEGIN		4
#define OPC_FLG_FUNCOP_MIDDLE		8
#define OPC_FLG_FUNCOP_END			16
#define OPC_FLG_DIRPOSLIGHT_INSTR	32

//==================================================================
struct OpCodeDef
{
	const char		*pName;
	u_int			OperCnt;
	u_int			Flags;
	SVM::OperTypeID	Types[5];
};

extern OpCodeDef	_gOpCodeDefs[];

//==================================================================
}
//==================================================================
}

#endif
