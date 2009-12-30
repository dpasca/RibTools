//==================================================================
/// RibRenderLib_Translator.h
///
/// Created by Davide Pasca - 2009/12/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_TRANSLATOR_H
#define RIBRENDERLIB_TRANSLATOR_H

#include "DSystem/include/DContainers.h"
#include "RI_System/include/RI_Base.h"
#include "RI_System/include/RI_State.h"

//==================================================================
namespace RRL
{

//==================================================================
/// 
//==================================================================
class Translator
{
public:
	struct Params
	{
		RI::State::Params	mState;
		int					mForcedLongDim		;
		int					mForcedWd			;
		int					mForcedHe			;

		Params() :
			mForcedLongDim		(-1),
			mForcedWd			(-1),
			mForcedHe			(-1)
		{
		}
	};

private:
	RI::State	mState;
	Params		mParams;

public:
	Translator( const Params &params );

	void AddCommand(const DStr	&cmdName,
					RI::ParamList	&cmdParams );

	void ErrHandler( RI::Error errCode )
	{
		printf( "Error %s !!\n", ErrorToString( errCode ) );
	}

	RI::State &GetState()	{	return mState;	}

private:
	void unknownCommand( const char *pCmdName );
	void exN( size_t n, const RI::ParamList &cmdParams );
	void geN( size_t n, const RI::ParamList &cmdParams );

	void addFormatCmd( RI::ParamList &p );

	bool addCommand_prims( const DStr &nm, RI::ParamList &p );
	bool addCommand_options( const DStr &nm, RI::ParamList &p );
	bool addCommand_transforms( const DStr &nm, RI::ParamList &p );

	static RtToken matchToken( const char *pStr, RtToken pAllowedTokens[] );
};

//==================================================================
}

#endif
