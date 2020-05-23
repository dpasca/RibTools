//==================================================================
/// RSLC_Functions.cpp
///
/// Created by Davide Pasca - 2009/5/30
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RSLC_Tree.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Registers.h"

#include "RSLC_Functions.h"

//==================================================================
namespace RSLC
{

//==================================================================
bool Function::IsAsmFunc() const
{
    return RSLC::IsAsmFunc( mpNameNode->GetTokStr() );
}

//==================================================================
bool Function::HasParams() const
{
    // has parameters if the first child is not a code block (body of the function)
    return
        mpParamsNode
        &&	mpParamsNode->mpChilds.size()
        &&	mpParamsNode->mpChilds[0]->GetBlockType() != BLKT_CODEBLOCK;
}

//==================================================================
}
