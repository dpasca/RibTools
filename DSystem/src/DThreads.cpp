//==================================================================
/// DThreads.cpp
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(WIN32)
#include <windows.h>
#endif

#include "DUtils.h"
#include "DThreads.h"

//==================================================================
namespace DTH
{

//==================================================================
ThreadedBase::ThreadedBase() :
	mQuitRequest(false),
	mQuitAck(false)
{
#if defined(WIN32)
	mThreadHandle = CreateThread( NULL, 16384, (LPTHREAD_START_ROUTINE)threadMain_s, (void *)this, 0, NULL );
#endif
}

//==================================================================
ThreadedBase::~ThreadedBase()
{
#if defined(WIN32)
	// wait for quit to be acknowledged
	mQuitRequest = true;
	ResumeThread( mThreadHandle );
	while ( mQuitAck == false )
	{
		ResumeThread( mThreadHandle );
		//SwitchToThread();
		DUT::SleepMS( 10 );
	}

	CloseHandle( mThreadHandle );
#endif
}

//==================================================================
}
