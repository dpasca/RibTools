//==================================================================
/// RI_RRASM_OpCodeDefs.cpp
///
/// Created by Davide Pasca - 2010/1/10
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include "RI_RRASM_OpCodeDefs.h"
#include "RI_SVM_OpcodeList.h"
#include "RI_Symbol.h"

//==================================================================
namespace RI
{
//==================================================================
namespace RRASM
{

#define NA  Symbol::TYP_UNKNOWN
#define F1  Symbol::TYP_FLOAT
#define F2  Symbol::TYP_FLOAT2
#define F3  Symbol::TYP_POINT
#define F4  Symbol::TYP_HPOINT
#define M44 Symbol::TYP_MATRIX
#define STR Symbol::TYP_STRING
#define BL  Symbol::TYP_BOOL
#define ADR Symbol::TYP_ADDR

#define RI_SVM_CALLBACK(...) nullptr
#define RI_RRASM_OPCODE_DEF(_NAME_, _COUNT_, _FLAGS_, _T1_, _T2_, _T3_, _T4_, _T5_, _CALLBACK_) \
    { _NAME_, _COUNT_, _FLAGS_, { _T1_, _T2_, _T3_, _T4_, _T5_ } },

// The corresponding VM callback is generated from the same list in
// RI_SVM_OpCodeFuncs.cpp.
OpCodeDef _gOpCodeDefs[] =
{
    RI_SVM_OPCODE_LIST( RI_RRASM_OPCODE_DEF )
    { nullptr, 0, 0, { NA, NA, NA, NA, NA } }
};

//==================================================================
size_t GetOpCodeDefCount()
{
    return sizeof(_gOpCodeDefs) / sizeof(_gOpCodeDefs[0]) - 1;
}

#undef RI_RRASM_OPCODE_DEF
#undef RI_SVM_CALLBACK
#undef ADR
#undef BL
#undef STR
#undef M44
#undef F4
#undef F3
#undef F2
#undef F1
#undef NA

//==================================================================
}
//==================================================================
}
