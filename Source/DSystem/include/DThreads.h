//==================================================================
/// DThreads.h
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DTHREADS_H
#define DTHREADS_H

//==================================================================
namespace DTH
{

//==================================================================
class ThreadedBase
{
protected:
#if defined(WIN32)
	HANDLE				mThreadHandle;
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
