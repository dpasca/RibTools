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
		#if defined(WIN32)
			EnterCriticalSection( &mpCSection->mCSection );
		
		#elif defined(__linux__)

		#endif
		}

		~Block()
		{
		#if defined(WIN32)
			LeaveCriticalSection( &mpCSection->mCSection );

		#elif defined(__linux__)

		#endif
		}
	};

public:
	CriticalSection()
	{
		InitializeCriticalSection( &mCSection );
	}

	~CriticalSection()
	{
		DeleteCriticalSection( &mCSection );
	}
};


//==================================================================
}

#endif
