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
	bool			mIsVariabilityFinalized;
	bool			mIsForcedDetail;

	bool			mIsArray;

	bool			mIsLValue;
	bool			mIsGlobal;
	bool			mIsSHParam;
	bool			mIsUsed;

	Register		mBuild_Register;

	class BaseVal
	{
		friend class Variable;
		
		bool			mUse;
		DVec<float>		mNumVec;
		std::string		mStr;
		bool			mBool;
	
	public:
		BaseVal() :
			mUse(false),
			mBool(false)
		{
		}

		void Set( float num )
		{
			DASSERT( mUse == false );
			mUse = true;
			mNumVec.push_back( num );
		}

		void Set( const char *pStr )
		{	
			DASSERT( mUse == false && mNumVec.size() == 0 );
			mUse = true;
			mStr = pStr;
		}

		void Set( bool val )
		{
			DASSERT( mUse == false && mNumVec.size() == 0 );
			mUse = true;
			mBool = val;
		}

	} mBaseVal;


	Variable() :
		//mpOwnerNode(NULL),
		mpDTypeTok(NULL),
		mpDetailTok(NULL),
		mpSpaceCastTok(NULL),
		mpDefNameTok(NULL),
		mVarType(VT_UNKNOWN),
		mIsVarying(false),
		mIsVariabilityFinalized(false),
		mIsForcedDetail(false),
		mIsArray(false),
		mIsLValue(false),
		mIsGlobal(false),
		mIsSHParam(false),
		mIsUsed(false)
	{
	}

	~Variable()
	{
	}


	float GetBaseValFloat() const
	{
		DASSERT( mVarType == VT_FLOAT && mBaseVal.mUse );
		return mBaseVal.mNumVec[0];
	}
	const DVec<float> &GetBaseValFloatVec() const
	{
		DASSERT(
			(  mVarType == VT_FLOAT	
			|| mVarType == VT_POINT	
			|| mVarType == VT_COLOR	
			|| mVarType == VT_VECTOR	
			|| mVarType == VT_NORMAL	
			|| mVarType == VT_MATRIX )
			&& mBaseVal.mUse );

		return mBaseVal.mNumVec;
	}
	const char *GetBaseValString() const
	{
		DASSERT( mVarType == VT_STRING && mBaseVal.mUse );
		return mBaseVal.mStr.c_str();
	}
	bool GetBaseValBool() const
	{
		DASSERT( mVarType == VT_BOOL && mBaseVal.mUse );
		return mBaseVal.mBool;
	}

	bool HasBaseVal( VarType varType=VT_UNKNOWN ) const
	{
		return mBaseVal.mUse && (varType == VT_UNKNOWN || mVarType == varType);
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
	void SetForcedDetail( bool onoff );
	void SetVarying( bool varying );

	bool IsConstant() const
	{
		return mBaseVal.mUse && !mIsGlobal && !mIsSHParam && !mIsVarying;
	}

	bool IsArray() const { return mIsArray; }
};

//==================================================================
}

#endif
