//==================================================================
/// DCriticalSection.h
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DCRITICALSECTION_H
#define DCRITICALSECTION_H

#if defined(WIN32)
#include <windows.h>
#endif
#include "DUtils.h"

//==================================================================
namespace DUT
{

//===============================================================
class CriticalSection
{
	friend class Block;

#if defined(WIN32)
	CRITICAL_SECTION	mCSection;
#endif

public:
	//===============================================================
	class Block
	{
		CriticalSection *mpCSection;

	public:
		Block( CriticalSection &csection ) :
			mpCSection(&csection)
		{
			mpCSection->Enter();
		}

		~Block()
		{
			mpCSection->Leave();
		}
	};

public:
	void Enter()
	{
	#if defined(WIN32)
		EnterCriticalSection( &mCSection );

	#elif defined(__linux__)

	#endif
	}

	void Leave()
	{
	#if defined(WIN32)
		LeaveCriticalSection( &mCSection );

	#elif defined(__linux__)

	#endif
	}

	CriticalSection()
	{
	#if defined(WIN32)
		InitializeCriticalSection( &mCSection );
		
	#elif defined(__linux__)

	#endif
	}

	~CriticalSection()
	{
	#if defined(WIN32)
		DeleteCriticalSection( &mCSection );
		
	#elif defined(__linux__)

	#endif
	}
};

//===============================================================
class LongCriticalSection
{
	friend class Block;

	CriticalSection	mCS;
	volatile bool	mIsBlocked;

public:
	//===============================================================
	class Block
	{
		LongCriticalSection *mpLCS;

	public:
		Block( LongCriticalSection &lcs ) :
			mpLCS(&lcs)
		{
			mpLCS->mCS.Enter();
			while ( mpLCS->mIsBlocked )
			{
				mpLCS->mCS.Leave();
				DUT::SleepMS( 1 );
				mpLCS->mCS.Enter();
			}
			mpLCS->mIsBlocked = true;
			mpLCS->mCS.Leave();
		}

		~Block()
		{
			mpLCS->mCS.Enter();
			DASSERT( mpLCS->mIsBlocked == true );
			mpLCS->mIsBlocked = false;
			mpLCS->mCS.Leave();
		}
	};

	LongCriticalSection() :
		mIsBlocked(false)
	{
	}
};

//==================================================================
}

#endif
