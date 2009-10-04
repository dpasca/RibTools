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
class Function;

//==================================================================
enum VarType
{
	VT_UNKNOWN	,
	VT_FLOAT	,
	VT_POINT	,
	VT_COLOR	,
	VT_VECTOR	,
	VT_NORMAL	,
	VT_MATRIX	,
	VT_STRING	,
	VT_BOOL		,
	VT_N
};

//==================================================================
class Register
{
	VarType			mVarType;
	bool			mIsVarying;
	bool			mIsForcedDetail;
	int				mRegIdx;

public:
	Register() :
		mVarType(VT_UNKNOWN),
		mIsVarying(false),
		mIsForcedDetail(false),
		mRegIdx(-1)
	{
	}

	void SetType( VarType vtype, bool isVarying, bool isForcedDetail )
	{
		DASSERT( mVarType == VT_UNKNOWN && mIsVarying == false );
		mVarType		= vtype;
		mIsVarying		= isVarying;
		mIsForcedDetail	= isForcedDetail;
	}

	void SetRegIdx( int regIdx )
	{
		DASSERT( mRegIdx == -1 );
		mRegIdx		= regIdx;
	}

	void SetVarying( bool varying )
	{
		DASSERT( mIsForcedDetail == false );
		mIsVarying = varying;
	}

	VarType	GetVarType() const		{ return mVarType; }
	bool	IsVarying() const		{ return mIsVarying; }
	bool	IsForcedDetail() const	{ return mIsForcedDetail; }
	int		GetRegIdx() const		{ return mRegIdx; }

	bool IsValid() const			{ return mRegIdx != -1; }
	bool IsAssigned() const			{ return mRegIdx != -1; }
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
	VarType			mVarType;
	bool			mIsVarying;
	bool			mIsForcedDetail;
	bool			mIsLValue;
	bool			mIsGlobal;
	bool			mIsSHParam;

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
		mHasBaseVal(false)
	{
	}

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
		mHasBaseVal		(	from.mHasBaseVal		)
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
Variable *AddVariable(
			TokNode *pNode,
			TokNode *pDTypeNode,
			TokNode *pDetailNode,
			TokNode *pSpaceCastTok,
			TokNode *pNameNode );

void AddSelfVariable(
			TokNode *pNode,
			VarType	varType,
			bool	isVarying,
			bool	isDetailForced );

void AddConstVariable( TokNode *pNode, TokNode *pRoot );

//==================================================================
VarType VarTypeFromToken( const Token *pTok );
const char *VarTypeToString( VarType type );
char VarTypeToLetter( VarType type );

//==================================================================
void AddStandardVariables( TokNode *pNode );
void CollectUsedStdVars( TokNode *pNode, DVec<size_t> &io_usedStdVarsList );
void DiscoverVariablesDeclarations( TokNode *pNode );
void DiscoverVariablesUsage( TokNode *pNode );
void DiscoverVariables( TokNode *pNode );
void WriteVariables( FILE *pFile, TokNode *pNode, const DVec<size_t> &usedStdVars );

//==================================================================
}

#endif
