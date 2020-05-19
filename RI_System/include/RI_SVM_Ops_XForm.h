//==================================================================
/// RI_SVM_Ops_XForm.h
///
/// Created by Davide Pasca - 2009/10/7
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_XFORM_H
#define RI_SVM_OPS_XFORM_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{

//==================================================================
void Inst_PXFormname_VXV( Context &ctx, u_int blocksN );
void Inst_VXFormname_VXV( Context &ctx, u_int blocksN );
void Inst_NXFormname_VXV( Context &ctx, u_int blocksN );
void Inst_CXFormname_VXV( Context &ctx, u_int blocksN );

//==================================================================
}
//==================================================================
}

#endif
