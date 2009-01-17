/*
 *  ribparser.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/xx.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

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
Parser::Parser() : mReachedEOF(false), mpTokenizer(NULL)
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
		mReachedEOF = true;
		return;
	}

	bool foundToken = mpTokenizer->AddChar( ch );

	if NOT( foundToken )
		return;

	//printf( "> %s\t", Tokenizer::GetDataTypeName( mpTokenizer->GetDataType() ) );

	Param	param;
	bool	addParam = true;
	
	switch ( mpTokenizer->GetDataType() )
	{
	case Tokenizer::DT_ALPHANUMERIC:
			mCurCommand = mNextCommand;
			mNextCommand = mpTokenizer->GetDataAphaNum();
			addParam = false;
			break;
			
	case Tokenizer::DT_INT:
			param.type				= Param::INT;
			param.u.intVal			= mpTokenizer->GetDataInt();
			break;
	
	case Tokenizer::DT_FLOAT:
			param.type				= Param::FLT;
			param.u.floatVal		= mpTokenizer->GetDataFloat();
			break;
	
	case Tokenizer::DT_INT_ARRAY:
			param.type				= Param::INT_ARR;
			param.u.intArrayVal		= mpTokenizer->GetDataIntAttary();
			break;
	
	case Tokenizer::DT_FLOAT_ARRAY:
			param.type				= Param::FLT_ARR;
			param.u.floatArrayVal	= mpTokenizer->GetDataFloatAttary();
			break;

	case Tokenizer::DT_STRING:
			param.type				= Param::STR;
			param.u.stringVal		= mpTokenizer->GetDataString();
			break;
	
	default:
			param.type = Param::UNKNOWN;
			//puts( "Unknown !!!" );
			break;
	}

	if ( addParam )
		mCurParams.push_back( param );
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
}
