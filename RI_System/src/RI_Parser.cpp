/*
 *  ribparser.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/xx.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include <stdio.h>
#include "Tokenizer.h"
#include "RI_Parser.h"

//==================================================================
static void printArray( const std::vector<int> &vec )
{
	for (size_t i=0; i < vec.size(); ++i)
		printf( "%i ", vec[i] );

	printf( "\n" );
}

//==================================================================
static void printArray( const std::vector<float> &vec )
{
	for (size_t i=0; i < vec.size(); ++i)
		printf( "%f ", vec[i] );

	printf( "\n" );
}

//==================================================================
namespace RI
{

//==================================================================
Parser::Parser() :
	mReachedEOF(false),
	mpTokenizer(NULL),
	mNextCommandLine(0),
	mCurCommandLine(0)
{
	mpTokenizer = new Tokenizer();
}

//==================================================================
Parser::~Parser()
{
	if ( mpTokenizer )
		delete mpTokenizer;
}

//==================================================================
void Parser::AddChar( char ch )
{
	if ( ch == 0 )
	{
		mCurCommand			= mNextCommand;
		mCurCommandLine		= mNextCommandLine;

		if ( mpTokenizer->AddChar( '\n' ) )
		{
			mNextCommand		= mpTokenizer->GetDataAphaNum();
			mNextCommandLine	= mpTokenizer->GetCurLineNumber();
		}
		else
		{
			mNextCommand		= "";
			mNextCommandLine	= -1;
		}

		mReachedEOF = true;
		return;
	}

	bool foundToken = mpTokenizer->AddChar( ch );

	if NOT( foundToken )
		return;

	//printf( "> %s\t", Tokenizer::GetDataTypeName( mpTokenizer->GetDataType() ) );

	Param	param;

	switch ( mpTokenizer->GetDataType() )
	{
	case Tokenizer::DT_ALPHANUMERIC:
			mCurCommand			= mNextCommand;
			mCurCommandLine		= mNextCommandLine;
			mNextCommand		= mpTokenizer->GetDataAphaNum();
			mNextCommandLine	= mpTokenizer->GetCurLineNumber();
			break;
			
	case Tokenizer::DT_INT:
			param.type				= Param::INT;
			param.u.intVal			= mpTokenizer->GetDataInt();
			mCurParams.push_back( param );
			break;
	
	case Tokenizer::DT_FLOAT:
			param.type				= Param::FLT;
			param.u.floatVal		= mpTokenizer->GetDataFloat();
			mCurParams.push_back( param );
			break;
	
	case Tokenizer::DT_INT_ARRAY:
			param.type				= Param::INT_ARR;
			param.u.intArrayVal		= mpTokenizer->GetDataIntAttary();
			mCurParams.push_back( param );
			break;
	
	case Tokenizer::DT_FLOAT_ARRAY:
			param.type				= Param::FLT_ARR;
			param.u.floatArrayVal	= mpTokenizer->GetDataFloatAttary();
			mCurParams.push_back( param );
			break;

	case Tokenizer::DT_STRING:
			param.type				= Param::STR;
			param.u.stringVal		= mpTokenizer->GetDataString();
			mCurParams.push_back( param );
			break;
	
	default:
			param.type = Param::UNKNOWN;
			//puts( "Unknown !!!" );
			mCurParams.push_back( param );
			break;
	}

/*
	switch ( mpTokenizer->GetDataType() )
	{
	case Tokenizer::DT_ALPHANUMERIC:printf( "%s \n", mpTokenizer->GetDataAphaNum() );	break;
	case Tokenizer::DT_INT:			printf( "%i \n", mpTokenizer->GetDataInt()	);	break;
	case Tokenizer::DT_FLOAT:		printf( "%f \n", mpTokenizer->GetDataFloat()	);	break;
	case Tokenizer::DT_INT_ARRAY:	printArray( mpTokenizer->GetDataIntAttary()	);	break;	
	case Tokenizer::DT_FLOAT_ARRAY:	printArray( mpTokenizer->GetDataFloatAttary()	);	break;
	case Tokenizer::DT_STRING:		printf( "%s \n", mpTokenizer->GetDataString()	);	break;
	
	default:
		puts( "Unknown !!!" );
		break;
	}
*/		
	mpTokenizer->ResetState();
}

//==================================================================
void Parser::FlushNewCommand( DStr		*out_pCmdName,
							  ParamList	*out_pParams,
							  int		*out_pCmdLine )
{
	*out_pCmdName	= mCurCommand;
	*out_pCmdLine	= mCurCommandLine;
	*out_pParams	= mCurParams;

	mCurParams.clear();

	if ( mReachedEOF )
	{
		mCurCommand			= mNextCommand;
		mCurCommandLine		= mNextCommandLine;

		mNextCommand		= "";
		mNextCommandLine	= -1;
	}
	else
	{
		mCurCommand			= "";	
	}
}

//==================================================================
int Parser::GetCurLineNumber() const
{
	return mpTokenizer->GetCurLineNumber();
}

//==================================================================
}
