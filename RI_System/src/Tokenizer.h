/*
 *  Tokenizer.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/19/11.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "DTypes.h"
#include "DContainers.h"
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
	int				mLineNumber;

	int				mDataInt;
	float			mDataFloat;
	std::string		mDataString;
	std::string		mDataAlphanum;
	DVec<float>		mFloatArray;
	DVec<int>		mIntArray;
	bool			mIsArrayFloat;
	std::string		mCurToken;

public:
	enum DataType
	{
		DT_NONE,
		DT_ALPHANUMERIC,
		DT_INT,
		DT_FLOAT,
		DT_INT_ARRAY,
		DT_FLOAT_ARRAY,
		DT_STRING
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
		mCurToken = "";
		mIsArrayFloat = false;
	}

	const char			*GetDataAphaNum()		const { DASSERT( mDataType == DT_ALPHANUMERIC ); return mDataAlphanum.c_str();	}
	int					GetDataInt()			const { DASSERT( mDataType == DT_INT ); return mDataInt;	}
	float				GetDataFloat()			const { DASSERT( mDataType == DT_FLOAT ); return mDataFloat;	}
	const DVec<int>		&GetDataIntArray()		const { DASSERT( mDataType == DT_INT_ARRAY ); return mIntArray; }
	const DVec<float>	&GetDataFloatArray()	const { DASSERT( mDataType == DT_FLOAT_ARRAY ); return mFloatArray; }
	const char			*GetDataString()		const { DASSERT( mDataType == DT_STRING ); return mDataString.c_str();	}

private:
	void	setDataType();
	void	changeToFloatArray();
	bool	inputHandleArray( char ch );
};

#endif