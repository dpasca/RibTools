//==================================================================
/// RSLC_Variable.cpp
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Variable.h"

//==================================================================
namespace RSLC
{

//==================================================================
void Variable::AssignRegister( int regIdx )
{
	DASSERT( !mBuild_Register.IsValid() );

	mBuild_Register.SetType( mVarType, mIsVarying, mIsForcedDetail );
	mBuild_Register.SetRegIdx( regIdx );
}

//==================================================================
bool Variable::IsRegisterAssigned() const
{
	return mBuild_Register.IsAssigned();
}

//==================================================================
RSLC::VarType Variable::GetVarType() const
{
	DASSERT( !mBuild_Register.IsValid() || mBuild_Register.GetVarType() == mVarType );

	return mVarType;
}

//==================================================================
bool Variable::IsVarying() const
{
	DASSERT( !mBuild_Register.IsValid() || mBuild_Register.IsVarying() == mIsVarying );

	return mIsVarying;
}

//==================================================================
bool Variable::IsForcedDetail() const
{
	DASSERT( mBuild_Register.IsForcedDetail() == mIsForcedDetail );

	return mIsForcedDetail;
}

//==================================================================
void Variable::SetVarying( bool varying )
{
	DASSERT( mIsForcedDetail == false );

	mIsVarying = varying;

	if ( mBuild_Register.IsValid() )
	{
		mBuild_Register.SetVarying( varying );
	}
}

//==================================================================
std::string Variable::GetUseName() const
{
	if ( mIsGlobal || mIsSHParam )
		return mpDefNameTok->str;
	else
	if ( mBuild_Register.IsValid() )
	{
		return mBuild_Register.GetName();
	}
	else
		return mInternalName;
}


//==================================================================
}
