//==================================================================
/// RSLC_Variables.h
///
/// Created by Davide Pasca - 2009/5/26
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_VARIABLES_H
#define RSLC_VARIABLES_H

#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
class TokNode;

//==================================================================
class Variable
{
public:
	std::string		mInternalName;
	TokNode			*mpOwnerNode;
	Token			*mpDTypeTok;
	Token			*mpDetailTok;
	Token			*mpSpaceCastTok;
	Token			*mpDefNameTok;
	DVec<Token*>	mpDefValToks;
	bool			mIsVarying;
	bool			mIsLValue;

	Variable() :
		mpOwnerNode(NULL),
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpDefNameTok(NULL),
		mIsVarying(true),
		mIsLValue(false)
	{
	}

	~Variable()
	{
	}

	bool HasDefName() const { return mpDefNameTok != NULL;	}

	const char *GetDefName() const
	{
		if ( mpDefNameTok )
			return mpDefNameTok->str.c_str();
		else
			return NULL;
	}
};

//==================================================================
void DiscoverVariables( TokNode *pNode );
void WriteVariables( FILE *pFile, TokNode *pNode );

//==================================================================
}

#endif
