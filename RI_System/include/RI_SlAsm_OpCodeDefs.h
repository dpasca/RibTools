//==================================================================
/// RI_SlAsm_OpCodeDefs.h
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLASM_OPCODEDEFS_H
#define RI_SLASM_OPCODEDEFS_H

#include "RI_SlShader.h"

//==================================================================
namespace RI
{

//==================================================================
#define OPC_FLG_RIGHTISIMM		1
#define OPC_FLG_UNIFORMOPERS	2
#define OPC_FLG_FUNCOP_BEGIN	4
#define OPC_FLG_FUNCOP_END		8

//==================================================================
struct SlAsmOpCodeDef
{
	const char	*pName;
	u_int		OperCnt;
	u_int		Flags;
	OperTypeID	Types[5];
};

extern SlAsmOpCodeDef	_gSlAsmOpCodeDefs[];

//==================================================================
}

#endif
