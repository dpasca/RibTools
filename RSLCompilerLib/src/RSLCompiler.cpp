//==================================================================
/// RSLCompiler.cpp
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DUtils.h"
#include "DSystem/include/DContainers.h"
#include "RSLC_Token.h"
#include "RSLCompiler.h"

//==================================================================
using namespace	RSLC;

//==================================================================
RSLCompiler::RSLCompiler( const char *pSource, size_t sourceSize )
{
	DVec<Token>	tokens;

	Tokenizer( tokens, pSource, sourceSize );

	for (size_t i=0; i < tokens.size(); ++i)
	{
		printf( "%3i) %i - %s\n", i, tokens[i].isPrecededByWS, tokens[i].str.c_str() );
	}
}

//==================================================================
RSLCompiler::~RSLCompiler()
{

}
