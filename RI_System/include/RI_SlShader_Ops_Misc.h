//==================================================================
/// RI_SlShader_Ops_Misc.h
///
/// Created by Davide Pasca - 2009/10/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_MISC_H
#define RI_SLSHADER_OPS_MISC_H

//==================================================================
namespace RI
{

class SlRunContext;

//==================================================================
namespace SOP
{

void Inst_Faceforward( SlRunContext &ctx );
void Inst_Normalize( SlRunContext &ctx );
void Inst_CalculateNormal( SlRunContext &ctx );

//==================================================================
}

//==================================================================
}

#endif
