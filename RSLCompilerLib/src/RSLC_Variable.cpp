//==================================================================
/// RSLC_Variable.cpp
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Variable.h"
#include "RSLC_Tree.h"

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
	//DASSERT( mBuild_Register.IsForcedDetail() == mIsForcedDetail );

	return mIsForcedDetail;
}

//==================================================================
void Variable::SetForcedDetail( bool onoff )
{
	DASSERT( mIsForcedDetail == false );

	mIsForcedDetail = onoff;
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
DStr Variable::GetUseName() const
{
	if ( mIsGlobal || mIsSHParam )
		return mpDefNameNode->GetTokStr();
	else
	if ( mBuild_Register.IsValid() )
	{
		return mBuild_Register.GetName();
	}
	else
		return mInternalName;
}

//==================================================================
const char * Variable::GetDefName() const
{
	if ( mpDefNameNode )
		return mpDefNameNode->GetTokStr();
	else
		return NULL;
}

//==================================================================
}
