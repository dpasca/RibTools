/*
 *  RI_Parser.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/xx.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef RI_PARSER_H
#define RI_PARSER_H

#include "DTypes.h"
#include "RI_Param.h"

//==================================================================
namespace RI
{

//==================================================================
/// 
//==================================================================
class Parser
{
	class Tokenizer	*mpTokenizer;

public:

	DStr		mNextCommand;
	DStr		mCurCommand;
	DVec<Param>	mCurParams;

public:
	Parser();
	~Parser();

	void AddChar( char ch );

	bool HasNewCommand() const
	{
		return	(mCurCommand.length() != 0 && mNextCommand.length() != 0) ||
				(mNextCommand.length() != 0 && mReachedEOF);
	}
	
	void FlushNewCommand( DStr *out_pCmdName, DVec<Param> *out_pParams )
	{
		if ( mReachedEOF )
			*out_pCmdName	= mNextCommand;
		else
			*out_pCmdName	= mCurCommand;
			
		*out_pParams	= mCurParams;
		
		//mCurCommand		= mNextCommand;
		mCurCommand	= "";
		
		mCurParams.clear();
	}
	
private:
	bool	mReachedEOF;
};

//==================================================================
}

#endif