//==================================================================
/// RI_SlAsm_OpCodeDefs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_SlAsm_OpCodeDefs.h"

//==================================================================
namespace RI
{

#define NA	OPRTYPE_NA

//==================================================================
// NOTE: this table must match the order of callbacks in sInstructionTable in RI_SlShader.cpp
// NOTE: make sure that the allowed types satisfy the rules in
//       RSLC_Operators_SolveBiOpType_RulesTable.h
SlAsmOpCodeDef	_gSlAsmOpCodeDefs[] =
{
	"ret"			,	0,			0,	NA,	NA, NA, NA, NA,

	"mov.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,	NA,
	"mov.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,	NA,
	"mov.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,	NA,
																					   
	"mov.xx"		,	2,			0,	OPRTYPE_STR,OPRTYPE_STR,NA,	NA,	NA,
																					   
	"abs.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,	NA,
	"abs.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,	NA,
	"abs.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,	NA,

	"sign.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,	NA,

	"add.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"add.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,
	"add.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	NA,	NA,
	"add.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,
																					   
	"sub.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"sub.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,
	"sub.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	NA,	NA,
	"sub.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,
																					   
	"mul.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"mul.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,
	"mul.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	NA,	NA,
	"mul.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,
																					   
	"div.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"div.vvs"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F1,	NA,	NA,
	"div.vsv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F3,	NA,	NA,
	"div.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,

	"pow.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,

	"mov.vs3"		,	4,			0,	OPRTYPE_F3,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,

	"dot.svv"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,

	"min.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"min.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,

	"max.sss"		,	3,			0,	OPRTYPE_F1,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"max.vvv"		,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3,	NA,	NA,

	"ld.s"			,	2,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F1,	NA,	NA,	NA,	NA,
	"ld.v"			,	4,			OPC_FLG_RIGHTISIMM,		OPRTYPE_F3,	NA,	NA,	NA,	NA,

	"cmplt.ssl"		,	3,			OPC_FLG_UNIFORMOPERS,	OPRTYPE_F1,	OPRTYPE_F1,OPRTYPE_ADDR,NA,NA,

	"setle.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"setge.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"setlt.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,
	"setgt.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1,	OPRTYPE_F1,	NA,	NA,

	"seteq.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1	, OPRTYPE_F1	,	NA,	NA,
	"seteq.bvv"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F3	, OPRTYPE_F3	,	NA,	NA,
	"seteq.bhh"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F4	, OPRTYPE_F4	,	NA,	NA,
	"seteq.bmm"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_M44	, OPRTYPE_M44	,	NA,	NA,
	"seteq.bxx"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_STR	, OPRTYPE_STR	,	NA,	NA,
	"seteq.bbb"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_BL	, OPRTYPE_BL	,	NA,	NA,

	"setne.bss"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F1	, OPRTYPE_F1	,	NA,	NA,
	"setne.bvv"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F3	, OPRTYPE_F3	,	NA,	NA,
	"setne.bhh"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_F4	, OPRTYPE_F4	,	NA,	NA,
	"setne.bmm"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_M44	, OPRTYPE_M44	,	NA,	NA,
	"setne.bxx"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_STR	, OPRTYPE_STR	,	NA,	NA,
	"setne.bbb"		,	3,			0,	OPRTYPE_BL,	OPRTYPE_BL	, OPRTYPE_BL	,	NA,	NA,

	"noise.ss"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F1, NA,	NA,	NA,
	"noise.sv2"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F2, NA,	NA,	NA,
	"noise.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, NA,	NA,	NA,

	"noise.vs"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F1, NA,	NA,	NA,
	"noise.vv2"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F2, NA,	NA,	NA,
	"noise.vv"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, NA,	NA,	NA,

	"xcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, NA, NA, NA,
	"ycomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, NA, NA, NA,
	"zcomp.sv"		,	2,			0,	OPRTYPE_F1,	OPRTYPE_F3, NA, NA, NA,
	"setxcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	NA,	NA,	NA,
	"setycomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	NA,	NA,	NA,
	"setzcomp.vs"	,	2,			0,	OPRTYPE_F3, OPRTYPE_F1,	NA,	NA,	NA,

	"pxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, NA, NA,
	"vxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, NA, NA,
	"nxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, NA, NA,
	"cxformname.vxv",	3,			0,	OPRTYPE_F3,OPRTYPE_STR, OPRTYPE_F3, NA, NA,

	"normalize"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, NA, NA, NA,
	"faceforward"	,	3,			0,	OPRTYPE_F3,	OPRTYPE_F3,	OPRTYPE_F3, NA, NA,
	"diffuse"		,	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, NA, NA, NA,
	"ambient"		,	1,			0,	OPRTYPE_F3,	NA, NA,	NA,	NA,
	"calculatenormal",	2,			0,	OPRTYPE_F3,	OPRTYPE_F3, NA,	NA,	NA,

	"solarbegin",		0,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, NA, NA, NA,	NA,	NA,
	"solarbegin.vs",	2,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F1, NA,	NA,	NA,
	"illuminance.v",	1,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, NA, NA,	NA,	NA,
	"illuminance.vvs",	3,			OPC_FLG_DIRPOSLIGHT_INSTR | OPC_FLG_FUNCOP_BEGIN, OPRTYPE_F3, OPRTYPE_F3, OPRTYPE_F1,	NA,	NA,
	"funcopend"		,	0,			OPC_FLG_FUNCOP_END,	  NA, NA, NA,	NA,	NA,

	NULL			,	0,			0,	NA, NA, NA, NA, NA
};

#undef NA

//==================================================================
}
