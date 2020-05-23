//==================================================================
/// RSLC_Register.h
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_REGISTER_H
#define RSLC_REGISTER_H

#include "RSLC_VarType.h"
#include "DSystem/include/DStr.h"
#include "DSystem/include/DUtils_Base.h"

//==================================================================
namespace RSLC
{

//==================================================================
/// Register
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

    //==================================================================
    DStr GetName() const;
};

//==================================================================
}

#endif
