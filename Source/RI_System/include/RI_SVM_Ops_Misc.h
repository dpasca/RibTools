//==================================================================
/// RI_SVM_Ops_Misc.h
///
/// Created by Davide Pasca - 2009/10/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_OPS_MISC_H
#define RI_SVM_OPS_MISC_H

//==================================================================
namespace RI
{
//==================================================================
namespace SVM
{
class Context;

void Inst_Faceforward( Context &ctx, u_int blocksN );
void Inst_Normalize( Context &ctx, u_int blocksN );
void Inst_CalculateNormal( Context &ctx, u_int blocksN );

//==================================================================
}
//==================================================================
}

#endif
