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
	Variable		*mpVarDef;	// this is the variable definition
								// in case this node is variable usage

public:
	TokNode( Token *pObj ) :
		mpToken(pObj),
		mpParent(NULL),
		mBlockType(BLKT_UNKNOWN),
		mBlockID(0),
		mpVarDef(NULL)
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

	const char *GetTokStr() const { return mpToken->str.c_str(); }

	TokNode	*GetLeft();
	TokNode	*GetRight();
	TokNode	*GetPrev();
	TokNode	*GetNext();

	const TokNode *GetLeft()	const {	return ((const TokNode *)this)->GetLeft();	}
	const TokNode *GetRight()	const {	return ((const TokNode *)this)->GetRight();	}
	const TokNode *GetPrev()	const {	return ((const TokNode *)this)->GetPrev();	}
	const TokNode *GetNext()	const {	return ((const TokNode *)this)->GetNext();	}

	TokNode *GetChildTry( size_t i ) const { return i < mpChilds.size() ? mpChilds[i] : NULL; }

		  NodeData &GetData()		{ return mData;	}
	const NodeData &GetData() const	{ return mData;	}

		  DVec<Variable> &GetVars()			{ return mData.mVariables;	}
	const DVec<Variable> &GetVars() const	{ return mData.mVariables;	}

	Variable *FindVariableByDefName( const char *pName );

	bool IsCodeBlock() const			{ return mBlockType == BLKT_CODEBLOCK; }
	bool IsExpressionBlock() const		{ return mBlockType == BLKT_EXPRESSION; }
	bool IsNonTerminal() const			{ return mpToken ? mpToken->idType == T_TYPE_NONTERM : NULL; }
	bool IsTokenID( TokenID id ) const	{ return mpToken ? mpToken->id == id : false; }
};

//==================================================================
TokNode *MakeTree( DVec<Token> &tokens );
void TraverseTree( TokNode *pNode, int depth );

//==================================================================
}

#endif
