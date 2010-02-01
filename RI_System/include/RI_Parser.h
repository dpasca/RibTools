//==================================================================
/// RI_Parser.h
///
/// Created by Davide Pasca - 2008/12/xx
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PARSER_H
#define RI_PARSER_H

#include "DSystem/include/DTypes.h"
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
	ParamList	mCurParams;

public:
	Parser();
	~Parser();

	void AddChar( char ch );

	bool HasNewCommand() const
	{
		return	( mCurCommand.length() != 0 );
	}
	
	void FlushNewCommand( DStr		*out_pCmdName,
						  ParamList	*out_pParams,
						  int		*out_pCmdLine );
	
	int GetCurLineNumber() const;
	
private:
	bool	mReachedEOF;
};

//==================================================================
}

#endif