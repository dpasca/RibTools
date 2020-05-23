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

#if !defined(_MSC_VER)

//==================================================================
//==================================================================
Mutex::Mutex()
{
    memset( &mMutex, 0, sizeof(mMutex) );
    if ( pthread_mutex_init( &mMutex, NULL ) ) {
        DEX_RUNTIME_ERROR( "Could not create a Mutex" );
    }
}
//==================================================================
Mutex::~Mutex()
{
    pthread_mutex_destroy( &mMutex );
}

//==================================================================
//==================================================================
CondVar::CondVar()
{
    memset( &mCV, 0, sizeof(mCV) );
    if ( pthread_cond_init( &mCV, NULL ) )
    {
        DEX_RUNTIME_ERROR( "Could not create a Condition Variable !" );
    }
}
//==================================================================
CondVar::~CondVar()
{
    pthread_cond_destroy( &mCV );
}

#endif

//==================================================================
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
    DASSERT( mQuitAck == true );
}

//==================================================================
void ThreadedBase::KillThread()
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
