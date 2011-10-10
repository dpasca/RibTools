//==================================================================
/// DUtils_Base.h
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DUTILS_BASE_H
#define DUTILS_BASE_H

#include <stdarg.h>

//==================================================================
namespace DUT
{

//==================================================================
#if defined(_DEBUG) || defined(DEBUG)
	#define DASSERT(_X_)			\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssert( ok, __FILE__, __LINE__, #_X_ );    \
			} }
#else
	#define DASSERT(_X_)			((void)0)
#endif

/*
#define DASSTHROW(_X_,_MSG_)		\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssThrow( ok, __FILE__, __LINE__, DUT::SSPrintF _MSG_ );\
			} }
*/

#if defined(D_NO_ASSTHROW)
# define DASSTHROW(_X_,_MSG_, ...)
#else
# define DASSTHROW(_X_,_MSG_, ... )                                     \
    {                                                                   \
        bool ok=(_X_);                                                  \
        if ( !ok ) {                                                    \
            DUT::DAssThrow( ok, __FILE__, __LINE__, _MSG_, ##__VA_ARGS__ ); \
        }                                                               \
    }
#endif

#if defined(VERBOSE)
# define DVERBOSE( _MSG_ , ... )  DUT::DVerbose( _MSG_ , ##__VA_ARGS__ )
#else
# define DVERBOSE( _MSG_ , ... )
#endif

#if defined(ANDROID)
# define DTRY       if (true)
# define DCATCH_ALL else
#else
# define DTRY		try
# define DCATCH_ALL	catch(...)
#endif

//==================================================================
void DAssert( bool ok, const char *pFile, int line, const char *msg = 0 );
void DSAssThrow( bool ok, const char *pFile, int line, const char *pNewCharMsg );
void DVAssThrow( bool ok, const char *pFile, int line, const char *pFmt, va_list vl );
void DAssThrow( bool ok, const char *pFile, int line, const char *pFmt, ... );
void DVerbose( const char *fmt, ... );
void DVVerbose(const char *fmt, va_list vl);
//==================================================================
}

#endif
