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

#ifdef NDEBUG
	#define DASSERT(_X_)			((void)0)
#else
	#define DASSERT(_X_)			\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssert( ok, __FILE__, __LINE__ );\
			} }
#endif

#define DASSTHROW(_X_,_MSG_)		\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssThrow( ok, __FILE__, __LINE__, DUT::SSPrintF _MSG_ );\
			} }

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, void * &out_pData, size_t &out_dataSize );

DStr GetDirNameFromFPathName( const char *pInFPathname );

char *SSPrintF( const char *pFmt, ... );
std::string SSPrintFS( const char *pFmt, ... );

void DAssert( bool ok, const char *pFile, int line );
void DAssThrow( bool ok, const char *pFile, int line, char *pNewCharMsg );

//==================================================================
const char *GetFileNameOnly( const char *pPathFileName );

//==================================================================
/// MemFile
//==================================================================
class MemFile
{
	const U8	*mpData;
	size_t		mDataSize;
	size_t		mReadPos;
	bool		mOwnData;
	bool		mIsReadOnly;

public:
	MemFile( const void *pDataSrc, size_t dataSize );
	MemFile( const char *pFileName );
	~MemFile();

	bool ReadTextLine( char *pDestStr, size_t destStrMaxSize );
};

//==================================================================
void StrStripBeginEndWhite( char *pStr );

I64 GetTimeTicks();
double TimeTicksToMS( I64 ticks );

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
}

#endif