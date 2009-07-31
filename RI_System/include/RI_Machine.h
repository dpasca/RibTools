//==================================================================
/// RI_Machine.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_MACHINE_H
#define RI_MACHINE_H

#include "DSystem/include/DContainers.h"
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
	Machine(FrameworkREYES *pFramework,
			const char *pBaseDir,
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
