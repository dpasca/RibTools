//==================================================================
/// RSLC_Registers.h
///
/// Created by Davide Pasca - 2009/6/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_REGISTERS_H
#define RSLC_REGISTERS_H

//==================================================================
namespace RSLC
{

//==================================================================
void AssignRegisters( class TokNode *pNode );
std::string GetRegName( const Register &reg );

//==================================================================
}

#endif
