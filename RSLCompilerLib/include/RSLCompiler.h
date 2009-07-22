//==================================================================
/// RSLCompiler.h
///
/// Created by Davide Pasca - 2009/5/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLCOMPILER_H
#define RSLCOMPILER_H

#include "RSLC_Exceptions.h"
#include "RSLC_Token.h"
#include "RSLC_Tree.h"

//==================================================================
/// RSLCompiler
//==================================================================
class RSLCompiler
{
	DVec<RSLC::Token>	mTokens;
	RSLC::TokNode		*mpRoot;

	static const char	*mpsVersionString;

	DVec<size_t>		mUsedStdVars;

public:
	class Params
	{
	public:
		bool	mDbgOutputTree;

		Params() :
			mDbgOutputTree(false)
		{
		}
	};

	RSLCompiler(
		const char *pSource,
		size_t sourceSize,
		const char *pBaseInclude,
		const Params &params );

	~RSLCompiler();

	void SaveASM( const char *pFName, const char *pRefSourceName );
};

#endif
