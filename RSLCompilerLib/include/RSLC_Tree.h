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

//==================================================================
namespace RSLC
{

//==================================================================
template <class _T>
class Node
{
public:
	_T			*mpObj;
	Node		*mpParent;
	DVec<Node*>	mpChilds;

public:
	Node( _T *pObj ) : mpObj(pObj), mpParent(NULL)
	{
	}

	~Node()
	{
		for (size_t i=0; i < mpChilds.size(); ++i)
			DSAFE_DELETE( mpChilds[i] );
	}

	Node *AddChild( Node *pNode )
	{
		pNode->mpParent = this;
		mpChilds.push_back( pNode );
		return pNode;
	}

	Node *AddNewChild( _T *pObj )
	{
		return AddChild( DNEW Node( pObj ) );
	}
};

//==================================================================
typedef Node<Token>	TokNode;

//==================================================================
TokNode *MakeTree( DVec<Token> &tokens );
void TraverseTree( TokNode *pNode, int depth );

//==================================================================
}

#endif
