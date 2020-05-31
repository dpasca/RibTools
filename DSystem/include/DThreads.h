//==================================================================
/// DThreads.h
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DTHREADS_H
#define DTHREADS_H

#include "incpthreads.h"

#include <future>
#include <thread>
#include <functional>

//==================================================================
namespace DTH
{

//==================================================================
class ParallelTasks
{
    DVec<std::future<void>> mFutures;

    const size_t  mMaxN;

public:
    ParallelTasks( size_t maxN=std::thread::hardware_concurrency() )
        : mMaxN(maxN)
    {
        mFutures.reserve( mMaxN );
    }

    void AddTask( const std::function<void ()> &fn )
    {
        mFutures.push_back( std::async( std::launch::async, fn ) );

        while ( mFutures.size() >= mMaxN )
        {
            mFutures[0].get();
            mFutures.erase( mFutures.begin() );
        }
    }
};

#if !defined(_MSC_VER)

//==================================================================
class Mutex
{
public:
    pthread_mutex_t mMutex;

public:
    Mutex();
    ~Mutex();

    class Lock
    {
        Mutex  &mMutex;
        bool   mIsLocked;

    public:
        Lock( Mutex &mutex ) : mMutex(mutex), mIsLocked(true)
        {
            pthread_mutex_lock( &mMutex.mMutex );
        }
        ~Lock()
        {
            Release();
        }

        void Release()
        {
            if NOT( mIsLocked ) {
                mIsLocked = false;
                pthread_mutex_unlock( &mMutex.mMutex );
            }
        }
    };
};

//==================================================================
class CondVar
{
public:
    pthread_cond_t mCV;

    CondVar();
    ~CondVar();

    void Signal() {
        pthread_cond_signal( &mCV );
    }
    void Broadcast() {
        pthread_cond_broadcast( &mCV );
    }
    void Wait( Mutex &mutex ) {
        pthread_cond_wait( &mCV, &mutex.mMutex );
    }
};

#endif

//==================================================================
class ThreadedBase
{
protected:
#if defined(WIN32)
    void				*mThreadHandle;
#endif
    bool				mQuitRequest;
    bool				mQuitAck;

public:
    ThreadedBase();
    virtual ~ThreadedBase();

private:
    static void threadMain_s( void *pThis )
    {
        ((ThreadedBase *)pThis)->threadMain();
    }

protected:
    void KillThread();
    virtual void threadMain() = 0;
};

//==================================================================
}

#endif
