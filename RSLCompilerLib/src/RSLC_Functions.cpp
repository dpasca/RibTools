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
void DiscoverDefaultParamValues( TokNode *pNode )
{
	if (
		pNode->mNodeType == TokNode::TYPE_BLOCK &&
		pNode->GetBlockType() == BLKT_SHPARAMS )
	{
		for (size_t i=0; i < pNode->mpChilds.size(); ++i)
		{
			TokNode *pChild = pNode->mpChilds[i];

			if ( pChild->mpToken->id == T_OP_ASSIGN )
			{
				//int yoyo = 1;
			}
		}
	}

	for (size_t i=0; i < pNode->mpChilds.size(); ++i)
	{
		DiscoverDefaultParamValues( pNode->mpChilds[i] );
	}
}

//==================================================================
}
