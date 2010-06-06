//==================================================================
/// DUtils_Base.h
///
/// Created by Davide Pasca - 2009/7/23
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DUTILS_BASE_H
#define DUTILS_BASE_H

#include <string>

//==================================================================
namespace DUT
{

//==================================================================
#ifdef _DEBUG
	#define DASSERT(_X_)			\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssert( ok, __FILE__, __LINE__ );\
			} }
#else
	#define DASSERT(_X_)			((void)0)
#endif

#define DASSTHROW(_X_,_MSG_)		\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssThrow( ok, __FILE__, __LINE__, DUT::SSPrintF _MSG_ );\
			} }


//==================================================================
void DAssert( bool ok, const char *pFile, int line );
void DAssThrow( bool ok, const char *pFile, int line, char *pNewCharMsg );
char *SSPrintF( const char *pFmt, ... );
std::string SSPrintFS( const char *pFmt, ... );

//==================================================================
}

#endif
