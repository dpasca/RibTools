//==================================================================
/// Tokenizer.h
///
/// Created by Davide Pasca - 2008/111/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include <string>

//===============================================================
class Tokenizer
{
    enum StateEnum
    {
        UNKNOWN,
        NORMAL,
        STRING,
        ARRAY,
    };
    
    StateEnum		mState;
    bool			mStateInComment;
    bool			mStateCommentStarted;
    int				mLineNumber;

    int				mDataInt;
    float			mDataFloat;
    DStr			mDataString;
    DStr			mDataAlphanum;
    DVec<float>		mFloatArray;
    DVec<int>		mIntArray;
    DVec<DStr>		mStringArray;
    char			mArrayType;
    bool			mIsArrayOfQuotedStrings;
    bool			mIsInArrayItemQuotedString;
    DStr			mCurToken;

public:
    enum DataType
    {
        DT_NONE,
        DT_ALPHANUMERIC,
        DT_INT,
        DT_FLOAT,
        DT_STRING,
        DT_INT_ARRAY,
        DT_FLOAT_ARRAY,
        DT_STRING_ARRAY,
    };
    
    static const char *GetDataTypeName( DataType dtype );

private:
    DataType			mDataType;

public:
    Tokenizer()
    {
        ResetState();
        mLineNumber = 1;
    }
    
    //===============================================================
    bool AddChar( char ch );

    //===============================================================
    DataType	GetDataType() const
    {
        return mDataType;
    }
    
    int GetCurLineNumber() const
    {
        return mLineNumber;
    }
    
    //===============================================================
    void ResetState()
    {
        mState = UNKNOWN;
        mStateInComment = false;
        mStateCommentStarted = false;
        mCurToken = "";
        mArrayType = 0;
        mIsArrayOfQuotedStrings = false;
        mIsInArrayItemQuotedString = false;
    }

    const char			*GetDataAphaNum()		const { DASSERT( mDataType == DT_ALPHANUMERIC ); return mDataAlphanum.c_str();	}
    int					GetDataInt()			const { DASSERT( mDataType == DT_INT ); return mDataInt;	}
    float				GetDataFloat()			const { DASSERT( mDataType == DT_FLOAT ); return mDataFloat;	}
    const char			*GetDataString()		const { DASSERT( mDataType == DT_STRING ); return mDataString.c_str();	}
    const DVec<int>		&GetDataIntArray()		const { DASSERT( mDataType == DT_INT_ARRAY ); return mIntArray; }
    const DVec<float>	&GetDataFloatArray()	const { DASSERT( mDataType == DT_FLOAT_ARRAY ); return mFloatArray; }
    const DVec<DStr>	&GetDataStringArray()	const { DASSERT( mDataType == DT_STRING_ARRAY ); return mStringArray; }

private:
    void	setDataType();
    void	changeToFloatArray();
    bool	inputHandleArray( char ch );
};

#endif