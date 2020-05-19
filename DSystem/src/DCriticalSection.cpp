//==================================================================
/// DCriticalSection.cpp
///
/// Created by Davide Pasca - 2012/4/29
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(WIN32)
#include <windows.h>
#endif

#include "DCriticalSection.h"

//==================================================================
namespace DUT
{

//==================================================================
CriticalSection::CriticalSection()
{
#if defined(WIN32)
	mCSection = DNEW CRITICAL_SECTION;
	InitializeCriticalSection( (CRITICAL_SECTION *)mCSection );
	
#elif defined(__linux__)

#endif
}

//==================================================================
CriticalSection::~CriticalSection()
{
#if defined(WIN32)
	CRITICAL_SECTION *pCSection = (CRITICAL_SECTION *)mCSection;

	DeleteCriticalSection( pCSection );
	DSAFE_DELETE( pCSection );
	
#elif defined(__linux__)

#endif
}

//==================================================================
void CriticalSection::Enter()
{
#if defined(WIN32)
	EnterCriticalSection( (CRITICAL_SECTION *)mCSection );

#elif defined(__linux__)

#endif
}

//==================================================================
void CriticalSection::Leave()
{
#if defined(WIN32)
	LeaveCriticalSection( (CRITICAL_SECTION *)mCSection );

#elif defined(__linux__)

#endif
}

//==================================================================
}
