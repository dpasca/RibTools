//==================================================================
/// RI_SlShader_Ops_Lighting.h
///
/// Created by Davide Pasca - 2009/5/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSHADER_OPS_LIGHTING_H
#define RI_SLSHADER_OPS_LIGHTING_H

//==================================================================
namespace RI
{

class SlRunContext;

//==================================================================
namespace SOP
{

//==================================================================
void Inst_Diffuse( SlRunContext &ctx );
void Inst_Ambient( SlRunContext &ctx );


//==================================================================
}

//==================================================================
}

#endif
