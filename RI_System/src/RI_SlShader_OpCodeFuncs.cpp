//==================================================================
/// RI_SlShader_OpCodeFuncs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Noise.h"
#include "RI_SlShader_Ops_Base.h"
#include "RI_SlShader_Ops_Lighting.h"
#include "RI_SlShader_Ops_Noise.h"
#include "RI_SlShader_Ops_XForm.h"
#include "RI_SlShader_Ops_Misc.h"
#include "RI_SlShader_Ops_Compare.h"

#include "RI_SlShader_OpCodeFuncs.h"

//==================================================================
namespace RI
{

//==================================================================
//==================================================================
#define B VecNMask
#define S Float_
#define V Float3_
#define H Float4_
#define M Matrix44
#define X SlStr

//#define MATRIX	Matrix44

//==================================================================
SlOpCodeFunc	_gSlOpCodeFuncs[] =
{
	NULL,	// ret

	SOP::Inst_1Op<S,S,OBT_MOV>,
	SOP::Inst_1Op<V,S,OBT_MOV>,
	SOP::Inst_1Op<V,V,OBT_MOV>,

	SOP::Inst_MovXX,

	SOP::Inst_1Op<S,S,OBT_ABS>,
	SOP::Inst_1Op<V,S,OBT_ABS>,
	SOP::Inst_1Op<V,V,OBT_ABS>,

	SOP::Inst_Sign<S,S>,

	SOP::Inst_2Op<S,S,S,OBT_ADD>,
	SOP::Inst_2Op<V,V,S,OBT_ADD>,
	SOP::Inst_2Op<V,S,V,OBT_ADD>,
	SOP::Inst_2Op<V,V,V,OBT_ADD>,

	SOP::Inst_2Op<S,S,S,OBT_SUB>,
	SOP::Inst_2Op<V,V,S,OBT_SUB>,
	SOP::Inst_2Op<V,S,V,OBT_SUB>,
	SOP::Inst_2Op<V,V,V,OBT_SUB>,

	SOP::Inst_2Op<S,S,S,OBT_MUL>,
	SOP::Inst_2Op<V,V,S,OBT_MUL>,
	SOP::Inst_2Op<V,S,V,OBT_MUL>,
	SOP::Inst_2Op<V,V,V,OBT_MUL>,

	SOP::Inst_2Op<S,S,S,OBT_DIV>,
	SOP::Inst_2Op<V,S,V,OBT_DIV>,
	SOP::Inst_2Op<V,V,S,OBT_DIV>,
	SOP::Inst_2Op<V,V,V,OBT_DIV>,

	SOP::Inst_Pow_SSS,

	SOP::Inst_MOVVS3<Float3_,Float_>,

	SOP::Inst_Dot_SVV,

	SOP::Inst_Min_Max<S,OBT_MIN>,
	SOP::Inst_Min_Max<V,OBT_MIN>,

	SOP::Inst_Min_Max<S,OBT_MAX>,
	SOP::Inst_Min_Max<V,OBT_MAX>,

	SOP::Inst_LD1<S>,
	SOP::Inst_LD3<V>,

	SOP::Inst_CMPLT<S>,	// cmplt

	SOP::Inst_SETCMP_REL<S,OBT_SETLE>,
	SOP::Inst_SETCMP_REL<S,OBT_SETGE>,
	SOP::Inst_SETCMP_REL<S,OBT_SETLT>,
	SOP::Inst_SETCMP_REL<S,OBT_SETGT>,

	SOP::Inst_SETCMP_EQ <S,OBT_SETEQ>,
	SOP::Inst_SETCMP_EQ <V,OBT_SETEQ>,
	SOP::Inst_SETCMP_EQ <H,OBT_SETEQ>,
	NULL, //SOP::Inst_SETCMP_EQ <M,OBT_SETEQ>,
	SOP::Inst_SETCMP_EQ_NoVary <X,OBT_SETEQ>,
	SOP::Inst_SETCMP_EQ <B,OBT_SETEQ>,

	SOP::Inst_SETCMP_EQ <S,OBT_SETNEQ>,
	SOP::Inst_SETCMP_EQ <V,OBT_SETNEQ>,
	SOP::Inst_SETCMP_EQ <H,OBT_SETNEQ>,
	NULL, //SOP::Inst_SETCMP_EQ <M,OBT_SETNEQ>,
	SOP::Inst_SETCMP_EQ_NoVary <X,OBT_SETNEQ>,
	SOP::Inst_SETCMP_EQ <B,OBT_SETNEQ>,

	SOP::Inst_Noise1<Float_>,
	SOP::Inst_Noise1<Float2_>,
	SOP::Inst_Noise1<Float3_>,

	SOP::Inst_Noise3<Float_>,
	SOP::Inst_Noise3<Float2_>,
	SOP::Inst_Noise3<Float3_>,

	SOP::Inst_GetVComp<0>,
	SOP::Inst_GetVComp<1>,
	SOP::Inst_GetVComp<2>,
	SOP::Inst_SetVComp<0>,
	SOP::Inst_SetVComp<1>,
	SOP::Inst_SetVComp<2>,

	SOP::Inst_PXFormname_VXV,
	SOP::Inst_VXFormname_VXV,
	SOP::Inst_NXFormname_VXV,
	SOP::Inst_CXFormname_VXV,

	SOP::Inst_Normalize,
	SOP::Inst_Faceforward,
	SOP::Inst_Diffuse,
	SOP::Inst_Ambient,
	SOP::Inst_CalculateNormal,

	SOP::Inst_Solar<false	>,
	SOP::Inst_Solar<true		>,

	SOP::Inst_Illuminance<false >,
	SOP::Inst_Illuminance<true  >,

	SOP::Inst_FuncopEnd,
};

#undef S
#undef V

//==================================================================
}
