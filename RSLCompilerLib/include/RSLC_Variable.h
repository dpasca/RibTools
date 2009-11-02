//==================================================================
/// RSLC_Variable.h
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_VARIABLE_H
#define RSLC_VARIABLE_H

#include <string>
#include "RSLC_VarType.h"
#include "RSLC_Register.h"
#include "RSLC_Token.h"

//==================================================================
namespace RSLC
{

//==================================================================
class Variable
{
public:
	std::string		mInternalName;
	Token			*mpDTypeTok;
	Token			*mpDetailTok;
	Token			*mpSpaceCastTok;
	Token			*mpDefNameTok;
	DVec<Token*>	mpDefValToks;
	VarType			mVarType;
	bool			mIsVarying;
	bool			mIsForcedDetail;
	bool			mIsLValue;
	bool			mIsGlobal;
	bool			mIsSHParam;
	bool			mIsUsed;

	Register		mBuild_Register;

	bool			mHasBaseVal;
	DVec<float>		mBaseValNum;
	std::string		mBaseValStr;

	Variable() :
		//mpOwnerNode(NULL),
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpDefNameTok(NULL),
		mVarType(VT_UNKNOWN),
		mIsVarying(false),
		mIsForcedDetail(false),
		mIsLValue(false),
		mIsGlobal(false),
		mIsSHParam(false),
		mIsUsed(false),
		mHasBaseVal(false)
	{
	}

/*
	Variable( const Variable &from ) :
		mInternalName	(	from.mInternalName		),
		mpDTypeTok		(	from.mpDTypeTok			),
		mpDetailTok		(	from.mpDetailTok		),
		mpSpaceCastTok	(	from.mpSpaceCastTok		),
		mpDefNameTok	(	from.mpDefNameTok		),
		mpDefValToks	(	from.mpDefValToks		),
		mVarType		(	from.mVarType			),
		mIsVarying		(	from.mIsVarying			),
		mIsForcedDetail	(	from.mIsForcedDetail	),
		mIsLValue		(	from.mIsLValue			),
		mIsGlobal		(	from.mIsGlobal			),
		mIsSHParam		(	from.mIsSHParam			),
		mBaseValNum		(	from.mBaseValNum		),
		mIsUsed			(	from.mIsUsed			),
		mHasBaseVal		(	from.mHasBaseVal		)
	{
	}
*/

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

	std::string GetUseName() const;

	void AssignRegister( int regIdx );
	bool IsRegisterAssigned() const;

	VarType GetVarType() const;

	bool IsVarying() const;
	bool IsForcedDetail() const;
	void SetVarying( bool varying );

	bool IsConstant() const
	{
		return mHasBaseVal && !mIsGlobal && !mIsSHParam && !mIsVarying;
	}
};

//==================================================================
}

#endif
