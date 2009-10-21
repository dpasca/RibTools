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
#include "RSLC_Functions.h"
#include "RSLC_VarLink.h"

//==================================================================
namespace RSLC
{

//==================================================================
class TokNode
{
#ifdef _DEBUG
	static size_t	sUIDCnt;

	size_t			mUIDCnt;
#endif

public:
	enum Type
	{
		TYPE_STANDARD,
		TYPE_BLOCK,
		TYPE_FUNCDEF,
		TYPE_FUNCCALL,
		TYPE_FUNCOP,
		TYPE_N
	};
public:
	Token			*mpToken;

private:
	DVec<Variable>	mVariables;
	DVec<Function>	mFunctions;

public:
	TokNode			*mpParent;
	Type			mNodeType;

private:
	BlockType		mBlockType;

public:
	u_int			mBlockID;
	DVec<TokNode*>	mpChilds;

	VarLink			mVarLink;	// this is the variable definition
								// in case this node is variable usage

#ifdef _DEBUG
	DVec<VarLink*>	mpReferringVarLinks;
#endif

public:
	TokNode( Token *pObj );

	TokNode( const char *pTokStr, TokenID tokId, TokenIDType tokIdType );

	TokNode( const TokNode &from );

	~TokNode();

	void SetBlockType( BlockType blockType )
	{
		DASSERT( mBlockType == BLKT_UNKNOWN && mNodeType == TYPE_STANDARD );

		mBlockType	= blockType;
		mNodeType	= TYPE_BLOCK;
	}

	void UpdateBlockTypeToFuncCall()
	{
		DASSERT( mBlockType == BLKT_EXPRESSION && mNodeType == TYPE_BLOCK );

		mBlockType	= BLKT_FUNCCALL;
	}

	void UpdateBlockTypeToFuncOpExpression()
	{
		DASSERT( mBlockType == BLKT_EXPRESSION && mNodeType == TYPE_BLOCK );

		mBlockType	= BLKT_FUNCOPEXPR;
	}

	BlockType GetBlockType() const
	{
		return mBlockType;
	}

	u_int GetBlockID() const
	{
		return mBlockID;
	}

	TokNode *AddChild( TokNode *pNode )
	{
		pNode->mpParent = this;
		mpChilds.push_back( pNode );
		return pNode;
	}

	TokNode *AddChildFront( TokNode *pNode )
	{
		pNode->mpParent = this;
		mpChilds.push_front( pNode );
		return pNode;
	}

	TokNode *AddNewChild( Token *pObj )
	{
		return AddChild( DNEW TokNode( pObj ) );
	}

	TokNode *AddNewChildFront( Token *pObj )
	{
		return AddChildFront( DNEW TokNode( pObj ) );
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

		  DVec<Variable> &GetVars()			{ return mVariables;	}
	const DVec<Variable> &GetVars() const	{ return mVariables;	}

		  DVec<Function> &GetFuncs()		{ return mFunctions;	}
	const DVec<Function> &GetFuncs() const	{ return mFunctions;	}

	VarLink FindVariableByDefName( const char *pName );

	bool IsCodeBlock() const			{ return mBlockType == BLKT_CODEBLOCK; }
	bool IsExpressionBlock() const		{ return mBlockType == BLKT_EXPRESSION; }
	bool IsNonTerminal() const			{ return mpToken ? mpToken->idType == T_TYPE_NONTERM : false; }
	bool IsStdFunction() const			{ return mpToken ? mpToken->idType == T_TYPE_STDFUNC : false; }
	bool IsFuncOp() const				{ return mpToken ? mpToken->idType == T_TYPE_FUNCOP : false; }
	bool IsDataType() const				{ return mpToken ? mpToken->idType == T_TYPE_DATATYPE : false; }
	bool IsTokenID( TokenID id ) const	{ return mpToken ? mpToken->id == id : false; }

	bool IsParentRoot() const			{ return mpParent && mpParent->mpParent == NULL;	}

	void UnlinkFromParent();

	void Reparent( TokNode *pNewParent )
	{
		UnlinkFromParent();
		mpParent = pNewParent;
	}

	void ReplaceNode( TokNode *pNode );

	const	Variable *GetVarPtr() const	{	return mVarLink.GetVarPtr();	}
			Variable *GetVarPtr()		{	return mVarLink.GetVarPtr();	}

	Register GetRegister() const;

	VarType GetVarType() const;

	bool IsVarying() const;
};

//==================================================================
void MakeTree( TokNode *pNode, DVec<Token> &tokens );
void RemoveClosingBrackets( TokNode *pNode, int *pParentScanIdx=NULL );
void RemoveSemicolons( TokNode *pNode, int *pParentScanIdx=NULL );
void TraverseTree( TokNode *pNode, int depth );

//==================================================================
}

#endif
