/*
 *  RI_Parser.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/xx.
 *  Copyright 2008 Davide Pasca. All rights reserved.
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
	int			mNextCommandLine;
	DStr		mCurCommand;
	int			mCurCommandLine;
	DVec<Param>	mCurParams;

public:
	Parser();
	~Parser();

	void AddChar( char ch );

	bool HasNewCommand() const
	{
		return	( mCurCommand.length() != 0 );
	}
	
	void FlushNewCommand( DStr			*out_pCmdName,
						  DVec<Param>	*out_pParams,
						  int			*out_pCmdLine );
	
	int GetCurLineNumber() const;
	
private:
	bool	mReachedEOF;
};

//==================================================================
}

#endif