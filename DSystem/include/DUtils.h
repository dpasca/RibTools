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
#include "DUtils_Base.h"
#include "DUtils_Files.h"

//==================================================================
namespace DUT
{

//==================================================================

char *SSPrintF( const char *pFmt, ... );
std::string SSPrintFS( const char *pFmt, ... );

//==================================================================
/// MemFile
//==================================================================
class MemFile
{
	DVec<U8>	mOwnData;
	const U8	*mpData;
	size_t		mDataSize;
	size_t		mReadPos;
	bool		mIsReadOnly;

public:
	MemFile( const void *pDataSrc, size_t dataSize );
	MemFile( const char *pFileName );
	~MemFile();

	const U8 *GetData() const		{	return mpData;	}
	size_t GetDataSize() const	{	return mDataSize;	}

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