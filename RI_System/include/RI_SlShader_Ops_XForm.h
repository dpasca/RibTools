//==================================================================
/// RI_SlShader_Ops_XForm.h
///
/// Created by Davide Pasca - 2009/10/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_XFORM_H
#define RI_SLSHADER_OPS_XFORM_H

//==================================================================
namespace RI
{

//==================================================================
namespace SOP
{

//==================================================================
void Inst_PXFormname_VXV( SlRunContext &ctx );
void Inst_VXFormname_VXV( SlRunContext &ctx );
void Inst_NXFormname_VXV( SlRunContext &ctx );
void Inst_CXFormname_VXV( SlRunContext &ctx );

//==================================================================
}

//==================================================================
}

#endif
