//==================================================================
/// RI_SVM_OpcodeList.h
/// Single source of truth for RRASM metadata and shader VM callbacks.
//==================================================================

#ifndef RI_SVM_OPCODELIST_H
#define RI_SVM_OPCODELIST_H

#define RI_SVM_OPCODE_LIST(OPCODE) \
    OPCODE( "ret",             0, OPC_FLG_VM_HANDLED,                              NA,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( nullptr ) ) \
    OPCODE( "mov.ss",          2, OPC_FLG_1STISDEST,                               F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Mov<S, S> ) ) \
    OPCODE( "mov.vs",          2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Mov<V, S> ) ) \
    OPCODE( "mov.vv",          2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Mov<V, V> ) ) \
    OPCODE( "mov.xx",          2, OPC_FLG_1STISDEST,                               STR, STR, NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Mov<X, X> ) ) \
    OPCODE( "mov.bb",          2, OPC_FLG_1STISDEST,                               BL,  BL,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Mov<B, B> ) ) \
    OPCODE( "neg.ss",          2, OPC_FLG_1STISDEST,                               F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Neg<S> ) ) \
    OPCODE( "neg.vv",          2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Neg<V> ) ) \
    OPCODE( "abs.ss",          2, OPC_FLG_1STISDEST,                               F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Abs<S, S> ) ) \
    OPCODE( "abs.vs",          2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Abs<V, S> ) ) \
    OPCODE( "abs.vv",          2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Abs<V, V> ) ) \
    OPCODE( "sign.ss",         2, OPC_FLG_1STISDEST,                               F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Sign<S, S> ) ) \
    OPCODE( "add.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<S, S, S, OBT_ADD> ) ) \
    OPCODE( "add.vvs",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, S, OBT_ADD> ) ) \
    OPCODE( "add.vsv",         3, OPC_FLG_1STISDEST,                               F3,  F1,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, S, V, OBT_ADD> ) ) \
    OPCODE( "add.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, V, OBT_ADD> ) ) \
    OPCODE( "sub.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<S, S, S, OBT_SUB> ) ) \
    OPCODE( "sub.vvs",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, S, OBT_SUB> ) ) \
    OPCODE( "sub.vsv",         3, OPC_FLG_1STISDEST,                               F3,  F1,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, S, V, OBT_SUB> ) ) \
    OPCODE( "sub.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, V, OBT_SUB> ) ) \
    OPCODE( "mul.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<S, S, S, OBT_MUL> ) ) \
    OPCODE( "mul.vvs",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, S, OBT_MUL> ) ) \
    OPCODE( "mul.vsv",         3, OPC_FLG_1STISDEST,                               F3,  F1,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, S, V, OBT_MUL> ) ) \
    OPCODE( "mul.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, V, OBT_MUL> ) ) \
    OPCODE( "div.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<S, S, S, OBT_DIV> ) ) \
    OPCODE( "div.vvs",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, S, OBT_DIV> ) ) \
    OPCODE( "div.vsv",         3, OPC_FLG_1STISDEST,                               F3,  F1,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, S, V, OBT_DIV> ) ) \
    OPCODE( "div.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_2Op<V, V, V, OBT_DIV> ) ) \
    OPCODE( "pow.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_Pow_SSS ) ) \
    OPCODE( "mov.vs3",         4, OPC_FLG_1STISDEST,                               F3,  F1,  F1,  F1,  NA,  RI_SVM_CALLBACK( Inst_MOVVS3<Float3_, Float_> ) ) \
    OPCODE( "dot.svv",         3, OPC_FLG_1STISDEST,                               F1,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_Dot_SVV ) ) \
    OPCODE( "length.sv",       2, OPC_FLG_1STISDEST,                               F1,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Length_SV ) ) \
    OPCODE( "min.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_Min_Max<S, OBT_MIN> ) ) \
    OPCODE( "min.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_Min_Max<V, OBT_MIN> ) ) \
    OPCODE( "max.sss",         3, OPC_FLG_1STISDEST,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_Min_Max<S, OBT_MAX> ) ) \
    OPCODE( "max.vvv",         3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_Min_Max<V, OBT_MAX> ) ) \
    OPCODE( "ld.s",            2, OPC_FLG_1STISDEST | OPC_FLG_RIGHTISIMM,           F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_LD1<S> ) ) \
    OPCODE( "ld.v",            4, OPC_FLG_1STISDEST | OPC_FLG_RIGHTISIMM,           F3,  F1,  F1,  F1,  NA,  RI_SVM_CALLBACK( Inst_LD3<V> ) ) \
    OPCODE( "arrld.sss",       3, OPC_FLG_1STISDEST | OPC_FLG_2NDISARRAY,           F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_ArrayLoad_Float ) ) \
    OPCODE( "arrst.sss",       3, OPC_FLG_2NDISARRAY,                               F1,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_ArrayStore_Float ) ) \
    OPCODE( "cmplt.ssl",       3, OPC_FLG_UNIFORMOPERS,                             F1,  F1,  ADR, NA,  NA,  RI_SVM_CALLBACK( Inst_CMPLT<S> ) ) \
    OPCODE( "setle.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_REL<S, OBT_SETLE> ) ) \
    OPCODE( "setge.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_REL<S, OBT_SETGE> ) ) \
    OPCODE( "setlt.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_REL<S, OBT_SETLT> ) ) \
    OPCODE( "setgt.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_REL<S, OBT_SETGT> ) ) \
    OPCODE( "seteq.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<S, OBT_SETEQ> ) ) \
    OPCODE( "seteq.bvv",       3, OPC_FLG_1STISDEST,                               BL,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<V, OBT_SETEQ> ) ) \
    OPCODE( "seteq.bhh",       3, OPC_FLG_1STISDEST,                               BL,  F4,  F4,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<H, OBT_SETEQ> ) ) \
    OPCODE( "seteq.bmm",       3, OPC_FLG_1STISDEST | OPC_FLG_UNIMPLEMENTED,       BL,  M44, M44, NA,  NA,  RI_SVM_CALLBACK( nullptr ) ) \
    OPCODE( "seteq.bxx",       3, OPC_FLG_1STISDEST,                               BL,  STR, STR, NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ_NoVary<X, OBT_SETEQ> ) ) \
    OPCODE( "seteq.bbb",       3, OPC_FLG_1STISDEST,                               BL,  BL,  BL,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<B, OBT_SETEQ> ) ) \
    OPCODE( "setne.bss",       3, OPC_FLG_1STISDEST,                               BL,  F1,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<S, OBT_SETNEQ> ) ) \
    OPCODE( "setne.bvv",       3, OPC_FLG_1STISDEST,                               BL,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<V, OBT_SETNEQ> ) ) \
    OPCODE( "setne.bhh",       3, OPC_FLG_1STISDEST,                               BL,  F4,  F4,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<H, OBT_SETNEQ> ) ) \
    OPCODE( "setne.bmm",       3, OPC_FLG_1STISDEST | OPC_FLG_UNIMPLEMENTED,       BL,  M44, M44, NA,  NA,  RI_SVM_CALLBACK( nullptr ) ) \
    OPCODE( "setne.bxx",       3, OPC_FLG_1STISDEST,                               BL,  STR, STR, NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ_NoVary<X, OBT_SETNEQ> ) ) \
    OPCODE( "setne.bbb",       3, OPC_FLG_1STISDEST,                               BL,  BL,  BL,  NA,  NA,  RI_SVM_CALLBACK( Inst_SETCMP_EQ<B, OBT_SETNEQ> ) ) \
    OPCODE( "noise.ss",        2, OPC_FLG_1STISDEST,                               F1,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise1<S> ) ) \
    OPCODE( "noise.sv2",       2, OPC_FLG_1STISDEST,                               F1,  F2,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise1<Float2_> ) ) \
    OPCODE( "noise.sv",        2, OPC_FLG_1STISDEST,                               F1,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise1<V> ) ) \
    OPCODE( "noise.vs",        2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise3<S> ) ) \
    OPCODE( "noise.vv2",       2, OPC_FLG_1STISDEST,                               F3,  F2,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise3<Float2_> ) ) \
    OPCODE( "noise.vv",        2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Noise3<V> ) ) \
    OPCODE( "xcomp.sv",        2, OPC_FLG_1STISDEST,                               F1,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_GetVComp<0> ) ) \
    OPCODE( "ycomp.sv",        2, OPC_FLG_1STISDEST,                               F1,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_GetVComp<1> ) ) \
    OPCODE( "zcomp.sv",        2, OPC_FLG_1STISDEST,                               F1,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_GetVComp<2> ) ) \
    OPCODE( "setxcomp.vs",     2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_SetVComp<0> ) ) \
    OPCODE( "setycomp.vs",     2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_SetVComp<1> ) ) \
    OPCODE( "setzcomp.vs",     2, OPC_FLG_1STISDEST,                               F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_SetVComp<2> ) ) \
    OPCODE( "pxformname.vxv",  3, OPC_FLG_1STISDEST,                               F3,  STR, F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_PXFormname_VXV ) ) \
    OPCODE( "vxformname.vxv",  3, OPC_FLG_1STISDEST,                               F3,  STR, F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_VXFormname_VXV ) ) \
    OPCODE( "nxformname.vxv",  3, OPC_FLG_1STISDEST,                               F3,  STR, F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_NXFormname_VXV ) ) \
    OPCODE( "cxformname.vxv",  3, OPC_FLG_1STISDEST,                               F3,  STR, F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_CXFormname_VXV ) ) \
    OPCODE( "normalize",       2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Normalize ) ) \
    OPCODE( "faceforward",     3, OPC_FLG_1STISDEST,                               F3,  F3,  F3,  NA,  NA,  RI_SVM_CALLBACK( Inst_Faceforward ) ) \
    OPCODE( "ambient",         1, OPC_FLG_1STISDEST,                               F3,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Ambient ) ) \
    OPCODE( "calculatenormal", 2, OPC_FLG_1STISDEST,                               F3,  F3,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_CalculateNormal ) ) \
    OPCODE( "solarbegin",      0, OPC_FLG_DIRPOSLIGHT | OPC_FLG_FUNCOP_BEGIN,       NA,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Solar<false> ) ) \
    OPCODE( "solarbegin.vs",   2, OPC_FLG_DIRPOSLIGHT | OPC_FLG_FUNCOP_BEGIN,       F3,  F1,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Solar<true> ) ) \
    OPCODE( "illuminance.v",   1, OPC_FLG_DIRPOSLIGHT | OPC_FLG_FUNCOP_BEGIN,       F3,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Illuminance<false> ) ) \
    OPCODE( "illuminance.vvs", 3, OPC_FLG_DIRPOSLIGHT | OPC_FLG_FUNCOP_BEGIN,       F3,  F3,  F1,  NA,  NA,  RI_SVM_CALLBACK( Inst_Illuminance<true> ) ) \
    OPCODE( "funcopend",       0, OPC_FLG_FUNCOP_END,                              NA,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_FuncopEnd ) ) \
    OPCODE( "iftrue.b",        1, OPC_FLG_FUNCOP_BEGIN,                            BL,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_IfTrue ) ) \
    OPCODE( "orelse",          0, OPC_FLG_FUNCOP_MIDDLE,                           NA,  NA,  NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_OrElse ) ) \
    OPCODE( "texture.sx",      2, OPC_FLG_1STISDEST,                               F1,  STR, NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Texture<S, 0> ) ) \
    OPCODE( "texture.vx",      2, OPC_FLG_1STISDEST,                               F3,  STR, NA,  NA,  NA,  RI_SVM_CALLBACK( Inst_Texture<V, 0> ) ) \
    OPCODE( "texture.sxss",    4, OPC_FLG_1STISDEST,                               F1,  STR, F1,  F1,  NA,  RI_SVM_CALLBACK( Inst_Texture<S, 1> ) ) \
    OPCODE( "texture.vxss",    4, OPC_FLG_1STISDEST,                               F3,  STR, F1,  F1,  NA,  RI_SVM_CALLBACK( Inst_Texture<V, 1> ) )

#endif
