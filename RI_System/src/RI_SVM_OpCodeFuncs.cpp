//==================================================================
/// RI_SVM_OpCodeFuncs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "stdafx.h"
#include "RI_Noise.h"
#include "RI_SVM_Ops_Array.h"
#include "RI_SVM_Ops_Base.h"
#include "RI_SVM_Ops_Lighting.h"
#include "RI_SVM_Ops_Noise.h"
#include "RI_SVM_Ops_XForm.h"
#include "RI_SVM_Ops_Misc.h"
#include "RI_SVM_Ops_Compare.h"
#include "RI_SVM_Ops_Texture.h"
#include "RI_SVM_OpCodeFuncs.h"
#include "RI_SVM_OpcodeList.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

#define B VecNMask
#define S Float_
#define V Float3_
#define H Float4_
#define X SlStr

#define RI_SVM_CALLBACK(...) __VA_ARGS__
#define RI_SVM_OPCODE_CALLBACK(_NAME_, _COUNT_, _FLAGS_, _T1_, _T2_, _T3_, _T4_, _T5_, _CALLBACK_) \
    _CALLBACK_,

SlOpCodeFunc _gSlOpCodeFuncs[] =
{
    RI_SVM_OPCODE_LIST( RI_SVM_OPCODE_CALLBACK )
};

//==================================================================
size_t GetSlOpCodeFuncCount()
{
    return sizeof(_gSlOpCodeFuncs) / sizeof(_gSlOpCodeFuncs[0]);
}

#undef RI_SVM_OPCODE_CALLBACK
#undef RI_SVM_CALLBACK
#undef X
#undef H
#undef V
#undef S
#undef B

//==================================================================
}
//==================================================================
}
