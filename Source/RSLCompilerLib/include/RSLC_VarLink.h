//==================================================================
/// RSLC_VarLink.h
///
/// Created by Davide Pasca - 2009/10/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_VARLINK_H
#define RSLC_VARLINK_H

#include "DSystem/include/DTypes.h"

//==================================================================
namespace RSLC
{

class TokNode;
class Variable;

//==================================================================
class VarLink
{
	TokNode		*mpNode;
	size_t		mVarIdx;

public:
	VarLink();
	VarLink( const VarLink &from );
	VarLink &operator=( const VarLink &from );

	~VarLink();

	void Setup( TokNode *pNode, size_t idx );
	void ReplaceNode( TokNode *pNewNode );

	bool IsValid() const { return mVarIdx != DNPOS; }

	const TokNode	*GetNode() const	{	return mpNode;	}
	size_t			GetVarIdx() const	{	return mVarIdx; }

		  Variable *GetVarPtr();
	const Variable *GetVarPtr() const;

#ifdef _DEBUG
	bool IsNodeValid() const;
#endif

private:
	void addRef();
	void subRef();
};

//==================================================================
}

#endif
