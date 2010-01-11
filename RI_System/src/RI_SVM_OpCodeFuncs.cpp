//==================================================================
/// RI_SVM_OpCodeFuncs.cpp
///
/// Created by Davide Pasca - 2009/11/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Noise.h"
#include "RI_SVM_Ops_Base.h"
#include "RI_SVM_Ops_Lighting.h"
#include "RI_SVM_Ops_Noise.h"
#include "RI_SVM_Ops_XForm.h"
#include "RI_SVM_Ops_Misc.h"
#include "RI_SVM_Ops_Compare.h"
#include "RI_SVM_Ops_Texture.h"
#include "RI_SVM_OpCodeFuncs.h"

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
//==================================================================
#define B VecNMask
#define S Float_
#define V Float3_
#define C Float3_
#define H Float4_
#define M Matrix44
#define X SlStr

//#define MATRIX	Matrix44

//==================================================================
SlOpCodeFunc	_gSlOpCodeFuncs[] =
{
	NULL,	// ret

	Inst_Mov<S,S>,
	Inst_Mov<V,S>,
	Inst_Mov<V,V>,
	// M ?
	Inst_Mov<X,X>,
	Inst_Mov<B,B>,

	Inst_Abs<S,S>,
	Inst_Abs<V,S>,
	Inst_Abs<V,V>,

	Inst_Sign<S,S>,

	Inst_2Op<S,S,S,OBT_ADD>,
	Inst_2Op<V,V,S,OBT_ADD>,
	Inst_2Op<V,S,V,OBT_ADD>,
	Inst_2Op<V,V,V,OBT_ADD>,

	Inst_2Op<S,S,S,OBT_SUB>,
	Inst_2Op<V,V,S,OBT_SUB>,
	Inst_2Op<V,S,V,OBT_SUB>,
	Inst_2Op<V,V,V,OBT_SUB>,

	Inst_2Op<S,S,S,OBT_MUL>,
	Inst_2Op<V,V,S,OBT_MUL>,
	Inst_2Op<V,S,V,OBT_MUL>,
	Inst_2Op<V,V,V,OBT_MUL>,

	Inst_2Op<S,S,S,OBT_DIV>,
	Inst_2Op<V,S,V,OBT_DIV>,
	Inst_2Op<V,V,S,OBT_DIV>,
	Inst_2Op<V,V,V,OBT_DIV>,

	Inst_Pow_SSS,

	Inst_MOVVS3<Float3_,Float_>,

	Inst_Dot_SVV,

	Inst_Min_Max<S,OBT_MIN>,
	Inst_Min_Max<V,OBT_MIN>,

	Inst_Min_Max<S,OBT_MAX>,
	Inst_Min_Max<V,OBT_MAX>,

	Inst_LD1<S>,
	Inst_LD3<V>,

	Inst_CMPLT<S>,	// cmplt

	Inst_SETCMP_REL<S,OBT_SETLE>,
	Inst_SETCMP_REL<S,OBT_SETGE>,
	Inst_SETCMP_REL<S,OBT_SETLT>,
	Inst_SETCMP_REL<S,OBT_SETGT>,

	Inst_SETCMP_EQ <S,OBT_SETEQ>,
	Inst_SETCMP_EQ <V,OBT_SETEQ>,
	Inst_SETCMP_EQ <H,OBT_SETEQ>,
	NULL, //Inst_SETCMP_EQ <M,OBT_SETEQ>,
	Inst_SETCMP_EQ_NoVary <X,OBT_SETEQ>,
	Inst_SETCMP_EQ <B,OBT_SETEQ>,

	Inst_SETCMP_EQ <S,OBT_SETNEQ>,
	Inst_SETCMP_EQ <V,OBT_SETNEQ>,
	Inst_SETCMP_EQ <H,OBT_SETNEQ>,
	NULL, //Inst_SETCMP_EQ <M,OBT_SETNEQ>,
	Inst_SETCMP_EQ_NoVary <X,OBT_SETNEQ>,
	Inst_SETCMP_EQ <B,OBT_SETNEQ>,

	Inst_Noise1<S>,
	Inst_Noise1<Float2_>,
	Inst_Noise1<V>,

	Inst_Noise3<S>,
	Inst_Noise3<Float2_>,
	Inst_Noise3<V>,

	Inst_GetVComp<0>,
	Inst_GetVComp<1>,
	Inst_GetVComp<2>,
	Inst_SetVComp<0>,
	Inst_SetVComp<1>,
	Inst_SetVComp<2>,

	Inst_PXFormname_VXV,
	Inst_VXFormname_VXV,
	Inst_NXFormname_VXV,
	Inst_CXFormname_VXV,

	Inst_Normalize,
	Inst_Faceforward,
	Inst_Ambient,
	Inst_CalculateNormal,

	Inst_Solar<false	>,
	Inst_Solar<true		>,

	Inst_Illuminance<false >,
	Inst_Illuminance<true  >,

	Inst_FuncopEnd,

	Inst_IfTrue,
	Inst_OrElse,

	Inst_Texture<S,0>	,
	Inst_Texture<V,0>	,
	Inst_Texture<S,1>	,
	Inst_Texture<V,1>	,
};

#undef S
#undef V

//==================================================================
}
//==================================================================
}
