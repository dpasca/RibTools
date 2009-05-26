//==================================================================
/// RSLC_Tree.h
///
/// Created by Davide Pasca - 2009/5/24
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_TREE_H
#define RSLC_TREE_H

#include "DSystem/include/DContainers.h"
#include "RSLC_Token.h"
#include "RSLC_Variables.h"

//==================================================================
namespace RSLC
{

//==================================================================
class NodeData
{
public:
	DVec<Variable>	mVariables;
};


//==================================================================
class TokNode
{
public:
	Token			*mpToken;
private:
	NodeData		mData;
public:
	TokNode			*mpParent;
	BlockType		mBlockType;
	u_int			mBlockID;
	DVec<TokNode*>	mpChilds;

public:
	TokNode( Token *pObj ) :
		mpToken(pObj),
		mpParent(NULL),
		mBlockType(BLKT_UNKNOWN),
		mBlockID(0)
	{
	}

	~TokNode()
	{
		for (size_t i=0; i < mpChilds.size(); ++i)
			DSAFE_DELETE( mpChilds[i] );
	}

	TokNode *AddChild( TokNode *pNode )
	{
		pNode->mpParent = this;
		mpChilds.push_back( pNode );
		return pNode;
	}

	TokNode *AddNewChild( Token *pObj )
	{
		return AddChild( DNEW TokNode( pObj ) );
	}

		  NodeData &GetData()		{ return mData;	}
	const NodeData &GetData() const	{ return mData;	}

		  DVec<Variable> &GetVars()			{ return mData.mVariables;	}
	const DVec<Variable> &GetVars() const	{ return mData.mVariables;	}
};

//==================================================================
TokNode *MakeTree( DVec<Token> &tokens );
void TraverseTree( TokNode *pNode, int depth );

//==================================================================
}

#endif
