/*
 *  RI_Machine.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_MACHINE_H
#define RI_MACHINE_H

#include "DContainers.h"
#include "RI_Base.h"
#include "RI_State.h"

//==================================================================
namespace RI
{

//==================================================================
/// 
//==================================================================
class Machine
{
	State	mState;
	int		mForcedWd;
	int		mForcedHe;

public:
	Machine(FrameworkBase *pFramework,
			const char *pDefaultShadersDir,
			int forcedWd=-1,
			int forcedHe=-1 );
	
	void AddCommand(const DStr	&cmdName,
					ParamList	&cmdParams );
	
	void ErrHandler( Error errCode )
	{
		printf( "Error %s !!\n", ErrorToString( errCode ) );
	}

private:
	void unknownCommand( const char *pCmdName );
	void exN( size_t n, const ParamList &cmdParams );
	void geN( size_t n, const ParamList &cmdParams );

};

//==================================================================
}

#endif