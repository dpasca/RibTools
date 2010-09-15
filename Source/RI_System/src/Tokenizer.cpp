//==================================================================
/// Tokenizer.cpp
///
/// Created by Davide Pasca - 2008/11/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

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
	case DT_STRING:			return "STRING";
	case DT_INT_ARRAY:		return "INT_ARRAY";
	case DT_FLOAT_ARRAY:	return "FLOAT_ARRAY";
	case DT_STRING_ARRAY:	return "STRING_ARRAY";
	}
	
	DASSERT( 0 );
	return "UNKNOWN";
}

/*
//===============================================================
static bool isFloatStr( const char *pStr )
{
	const char *pStrRun = pStr;

	char ch;
	while ( ch = *pStrRun++ )
	{
		if NOT( (ch >= '0' && ch <= '9') ||
				ch == '.' ||
				ch == 'e' ||
				ch == 'f' ||
				ch == 'E' ||
				ch == 'F' ||
				ch == '-' ||
				ch == '+'  )
		{
			return false;
		}
	}

	return true;
}
*/

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
		return 1 == sscanf( pStr, "%lf", &tmp );
	}
	else
		return false;
}

//===============================================================
static bool isStringStr( const char *pStr )
{
	size_t	len = strlen( pStr );

	if ( len >= 2 && pStr[0] == '"' && pStr[len-1] == '"' )
		return true;
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
	if ( mState != STRING && !mIsInArrayItemQuotedString )
	{
		if ( ch == '#' )
		{
			if ( mStateInComment )
			{
				// handle the ## case
				if ( mStateCommentStarted )
				{
					// not a comment anymore
					// and return
					mStateInComment = false;
					mStateCommentStarted = false;
					return false;
				}
			}
			else
			{
				// it's a comment, and it's the first char of it
				mStateInComment = true;

				// This is to handle ## at the beginning of the line
				// ... for the time being, simply ignore it
				//mStateCommentStarted = true;
				mStateCommentStarted = false;
				return false;
			}
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

		// not start of a comment anymore 8)
		mStateCommentStarted = false;
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
			mStringArray.clear();
			mState = ARRAY;
			mArrayType = 0;
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

	if ( ch == '"' )
	{
		if ( mIsInArrayItemQuotedString )
		{
			mIsInArrayItemQuotedString = false;
			if ( mArrayType && mArrayType != 's' )
			{
				DASSTHROW( false, ("Cannot mix strings with numerical values in arrays") );
			}
			//mCurToken += ch;
			return false;
		}
		else
		{
			mIsInArrayItemQuotedString = true;
			mArrayType = 's';
			//mCurToken += ch;
			return false;
		}
	}
	else
	{
		if ( mIsInArrayItemQuotedString )
		{
			mCurToken += ch;
			return false;
		}
	}
	
	if ( isWhite( ch ) || isEndOfArray )
	{
		if ( ch == '\n' )
			mLineNumber += 1;

		if ( mCurToken.length() != 0 )
		{
			if ( isStringStr( mCurToken.c_str() ) )
			{
				// initialize this array to string
				if ( mArrayType == 0 )
					mArrayType = 's';

				if ( mArrayType != 's'  )
				{
					DASSTHROW( false, ("Cannot mix strings with numerical values in arrays") );
				}
			}
			else
			if ( isFloatStr( mCurToken.c_str() ) )
			{
				// initialize this array to float
				if ( mArrayType == 0 )
					mArrayType = 'f';

				if ( mArrayType != 'f' )
				{
					changeToFloatArray();
				}
			}
			else
			{
				// initialize this array to int
				if ( mArrayType == 0 )
					mArrayType = 'i';
			}

			// TODO: check types
			if ( mArrayType == 'f' )
			{
				float val = (float)atof( mCurToken.c_str() );
				mFloatArray.push_back( val );
			}
			else
			if ( mArrayType == 'i' )
			{
				int val = atoi( mCurToken.c_str() );
				mIntArray.push_back( val );
			}
			else
			if ( mArrayType == 's' )
			{
				mStringArray.push_back( mCurToken );
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
		if ( mArrayType == 'f' )
			mDataType = DT_FLOAT_ARRAY;
		else
		if ( mArrayType == 'i' )
			mDataType = DT_INT_ARRAY;
		else
		if ( mArrayType == 's' )
			mDataType = DT_STRING_ARRAY;
	}
	else
	if ( mState == STRING )
		mDataType = DT_STRING;
		
	mCurToken = "";
}

//===============================================================
void Tokenizer::changeToFloatArray()
{
	mArrayType = 'f';
	DASSERT( mFloatArray.size() == 0 );
	
	mFloatArray.resize( mIntArray.size() );

	for (size_t i=0; i < mIntArray.size(); ++i)
		mFloatArray[i] = (float)mIntArray[i];
}	
