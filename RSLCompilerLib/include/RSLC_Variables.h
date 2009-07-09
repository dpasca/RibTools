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
enum VarType
{
	VT_FLOAT,
	VT_POINT,
	VT_COLOR,
	VT_STRING,
	VT_VECTOR,
	VT_NORMAL,
	VT_MATRIX,
	VT_N
};

//==================================================================
class Register
{
public:
	VarType			mVarType;
	bool			mIsVarying;
	int				mRegIdx;

	Register() :
		mVarType(VT_FLOAT),
		mIsVarying(false),
		mRegIdx(-1)
	{
	}

	bool IsValid() const { return mRegIdx != -1; }
};

//==================================================================
class Variable
{
public:
	std::string		mInternalName;
	//TokNode			*mpOwnerNode;
	Token			*mpDTypeTok;
	Token			*mpDetailTok;
	Token			*mpSpaceCastTok;
	Token			*mpDefNameTok;
	DVec<Token*>	mpDefValToks;
	bool			mIsVarying;
	bool			mIsLValue;

	Register		mBuild_Register;


	Variable() :
		//mpOwnerNode(NULL),
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpDefNameTok(NULL),
		mIsVarying(true),
		mIsLValue(false)
	{
	}

	Variable( const Variable &from ) :
		mInternalName	(	from.mInternalName		),
		mpDTypeTok		(	from.mpDTypeTok			),
		mpDetailTok		(	from.mpDetailTok		),
		mpSpaceCastTok	(	from.mpSpaceCastTok		),
		mpDefNameTok	(	from.mpDefNameTok		),
		mpDefValToks	(	from.mpDefValToks		),
		mIsVarying		(	from.mIsVarying			),
		mIsLValue		(	from.mIsLValue			)
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

	void BuildSetupRegister( int &io_regIdx );
};

//==================================================================
void AddStandardVariables( TokNode *pNode );
void DiscoverVariablesDeclarations( TokNode *pNode );
void DiscoverVariablesUsage( TokNode *pNode );
void DiscoverVariables( TokNode *pNode );
void WriteVariables( FILE *pFile, TokNode *pNode );

//==================================================================
}

#endif
