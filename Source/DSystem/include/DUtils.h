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
#include "DStr.h"
#include "DUtils_Str.h"
//#include "DUtils_Files.h"
//#include "DUtils_MemFile.h"
//#include "DUtils_FileManager.h"

//==================================================================
namespace DUT
{

DStr VSSPrintFS( const char *pFmt, va_list vl );
DStr SSPrintFS( const char *pFmt, ... );

I64 GetTimeTicks();
double TimeTicksToMS( I64 ticks );

void SleepMS( U32 sleepMS );

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
/// TimeOut
//==================================================================
class TimeOut
{
	I64	mStartTicks;
	U32	mTimeOutMS;

public:
	TimeOut( U32 timeoutMS ) :
		mStartTicks(GetTimeTicks()),
		mTimeOutMS(timeoutMS)
	{
	}

	bool IsExpired() const
	{
		I64 delta = GetTimeTicks() - mStartTicks;

		return TimeTicksToMS( delta ) > mTimeOutMS;
	}

	void Reset()
	{
		mStartTicks = GetTimeTicks();
	}
};

//==================================================================
}

#endif
