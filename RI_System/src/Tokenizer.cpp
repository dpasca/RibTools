/*
 *  Tokenizer.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/19/11.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "Tokenizer.h"

//===============================================================
const char *Tokenizer::GetDataTypeName( DataType dtype )
{
	switch ( dtype )
	{
	case DT_NONE:			return "NONE";
	case DT_ALPHANUMERIC:	return "ALPHANUMERIC";
	case DT_INT:			return "INT";
	case DT_FLOAT:			return "FLOAT";
	case DT_INT_ARRAY:		return "INT_ARRAY";
	case DT_FLOAT_ARRAY:	return "FLOAT_ARRAY";
	case DT_STRING:			return "STRING";
	}
	
	DASSERT( 0 );
	return "UNKNOWN";
}

//===============================================================
static bool isFloatStr( const char *pStr )
{
	if (
		strchr( pStr, '.' ) ||
		strchr( pStr, 'f' ) ||
		strchr( pStr, 'F' ) ||
		strchr( pStr, 'd' ) ||
		strchr( pStr, 'D' ) ||
		strchr( pStr, 'e' ) ||
		strchr( pStr, 'E' ) )
	{
		double tmp;
		return 1 == sscanf( pStr, "%ld", &tmp );
	}
	else
		return false;
}

//===============================================================
static bool isIntStr( const char *pStr )
{
	char ch;
	while ( ch = *pStr++ )
	{
		if NOT( (ch >= '0' && ch <= '9') ||
				ch == '-' ||
				ch == '+'  )
			return false;
	}
	
	return true;
}

//===============================================================
static bool isAlphanumStr( const char *pStr )
{
	if NOT( pStr[0] )
		return false;

	if ( (pStr[0] >= 'a' && pStr[0] <= 'z') ||
		 (pStr[0] >= 'A' && pStr[0] <= 'Z') ||
		 (pStr[0] >= '_' ) )
	{
		return true;
	}	
	
	return false;
}

//===============================================================
static bool isWhite( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}

//===============================================================
static bool isNewLine( char ch )
{
	return ch == '\n' || ch == '\r' || ch == '\f';
}

//===============================================================
bool Tokenizer::AddChar( char ch )
{
	if ( mState != STRING )
	{
		if ( ch == '#' )
		{
			//setDataType();
			mStateInComment = true;
			//mState = COMMENT;
		}
	}
	
	if ( mStateInComment )
	{
		if ( isNewLine( ch ) )
		{
			if ( ch == '\n' )
				mLineNumber += 1;

			mStateInComment = false;
			//ResetState();
		}
	}
	else
	if ( mState == UNKNOWN )
	{
		if ( ch == '"' )
		{
			mState = STRING;
			setDataType();
		}
		else
		if ( ch == '[' )
		{
			mFloatArray.clear();
			mIntArray.clear();
			mState = ARRAY;
		}
		else
		{
			if NOT( isWhite( ch ) )
			{
				mState = NORMAL;
				mCurToken += ch;
			}
			else
				if ( ch == '\n' )
					mLineNumber += 1;			
		}
	}
	else
	if ( mState == NORMAL )
	{
		if ( isWhite( ch ) )
		{
			if ( ch == '\n' )
				mLineNumber += 1;

			setDataType();
			return true;
		}
		else
		{
			mCurToken += ch;
		}
	}
	else
	if ( mState == STRING )
	{
		if ( ch == '"' )
		{
			mDataString = mCurToken;
			setDataType();
			return true;
		}
		else
		{
			mCurToken += ch;
		}
	}
	else
	if ( mState == ARRAY )
	{
		return inputHandleArray( ch );
	}
	
	return false;
}

//===============================================================
bool Tokenizer::inputHandleArray( char ch )
{
	bool	isEndOfArray = (ch == ']');
	
	if ( isWhite( ch ) || isEndOfArray )
	{
		if ( ch == '\n' )
			mLineNumber += 1;

		if ( mCurToken.length() != 0 )
		{
			if ( !mIsArrayFloat && isFloatStr( mCurToken.c_str() ) )
			{
				changeToFloatArray();
			}

			if ( mIsArrayFloat )
			{
				float val = (float)atof( mCurToken.c_str() );
				mFloatArray.push_back( val );
			}
			else
			{
				int val = atoi( mCurToken.c_str() );
				mIntArray.push_back( val );
			}
			
			mCurToken = "";
		}

		if ( isEndOfArray )
		{
			setDataType();
			return true;
		}
	}
	else
	{
		mCurToken += ch;
	}
	
	return false;
}

//===============================================================
void Tokenizer::setDataType()
{
	if ( mState == UNKNOWN )
		mDataType = DT_NONE;
	else
	if ( mState == NORMAL )
	{
		if ( isIntStr( mCurToken.c_str() ) )
		{
			mDataInt = atoi( mCurToken.c_str() );
			mDataType = DT_INT;
		}
		else
		if ( isFloatStr( mCurToken.c_str() ) )
		{
			mDataFloat = (float)atof( mCurToken.c_str() );
			mDataType = DT_FLOAT;
		}
		else
		if ( isAlphanumStr( mCurToken.c_str() ) )
		{
			mDataAlphanum = mCurToken;
			mDataType = DT_ALPHANUMERIC;
		}
		else
			mDataType = DT_NONE;		
	}
	else
	if ( mState == ARRAY )
	{
		if ( mIsArrayFloat )
			mDataType = DT_FLOAT_ARRAY;
		else
			mDataType = DT_INT_ARRAY;
	}
	else
	if ( mState == STRING )
		mDataType = DT_STRING;
		
	mCurToken = "";
}

//===============================================================
void Tokenizer::changeToFloatArray()
{
	mIsArrayFloat = true;
	DASSERT( mFloatArray.size() == 0 );
	
	mFloatArray.resize( mIntArray.size() );

	for (size_t i=0; i < mIntArray.size(); ++i)
		mFloatArray[i] = (float)mIntArray[i];
}	
