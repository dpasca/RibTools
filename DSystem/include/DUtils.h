//==================================================================
/// DUtils.h
///
/// Created by Davide Pasca - 2008/12/31
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DUTILS_H
#define DUTILS_H

#include "DTypes.h"
#include "DUtils_Base.h"
//#include "DUtils_Files.h"
//#include "DUtils_MemFile.h"
//#include "DUtils_FileManager.h"

//==================================================================
namespace DUT
{

//==================================================================
void StrStripBeginEndWhite( char *pStr );
const char *StrStrI( const char *pStr, const char *pSearch );

I64 GetTimeTicks();
double TimeTicksToMS( I64 ticks );

//==================================================================
/// QuickProf
//==================================================================
class QuickProf
{
	const char *mpMsg;
	I64			mStart;

public:
	QuickProf( const char *pMsg ) :
		mpMsg(pMsg)
	{
		mStart = DUT::GetTimeTicks();
	}

	~QuickProf()
	{
		I64	elapsed = DUT::GetTimeTicks() - mStart;
		printf( "%s: %4.2lf ms\n", mpMsg, DUT::TimeTicksToMS( elapsed ) );
	}
};

//==================================================================
}

#endif
