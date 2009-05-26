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
	Token			*mpDTypeTok;
	Token			*mpDetailTok;
	Token			*mpSpaceCastTok;
	Token			*mpNameTok;
	DVec<Token*>	mpDefValToks;		
	bool			mIsVarying;

	Variable() :
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpNameTok(NULL),
		mIsVarying(false)
	{
	}

	~Variable()
	{
	}
};

//==================================================================
void DiscoverVariables( TokNode *pNode );

//==================================================================
}

#endif
