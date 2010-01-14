//==================================================================
/// DMemFile.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DUtils_Files.h"
#include "DUtils_MemFile.h"

//==================================================================
namespace DUT
{

//==================================================================
/// MemFile
//==================================================================
MemFile::MemFile() :
	mpData(NULL),
	mDataSize(0),
	mReadPos(0),
	mIsReadOnly(true)
{
}

//==================================================================
MemFile::MemFile( const void *pDataSrc, size_t dataSize ) :
	mpData((const U8 *)pDataSrc),
	mDataSize(dataSize),
	mReadPos(0),
	mIsReadOnly(true)
{
}

//==================================================================
MemFile::MemFile( const char *pFileName ) :
	mpData(NULL),
	mDataSize(0),
	mReadPos(0),
	mIsReadOnly(true)
{
	Init( pFileName );
}

//==================================================================
MemFile::~MemFile()
{
}

//==================================================================
void MemFile::Init( const void *pDataSrc, size_t dataSize )
{
	DASSERT( mpData == NULL && mDataSize == 0 && mReadPos == 0 );

	mpData		= (const U8 *)pDataSrc;
	mDataSize	= dataSize;
	mReadPos	= 0;
	mIsReadOnly	= true;
}

//==================================================================
void MemFile::Init( const char *pFileName )
{
	DASSERT( mpData == NULL && mDataSize == 0 && mReadPos == 0 );

	if NOT( GrabFile( pFileName, mOwnData ) )
	{
		DASSTHROW( 0, ("Could not open the file %s in input.", pFileName) );
	}

	mpData = (const U8 *)&mOwnData[0];
	mDataSize = mOwnData.size();
}

//==================================================================
void MemFile::InitExclusiveOwenership( DVec<U8> &fromData )
{
	mDataSize = fromData.size();
	mOwnData.get_ownership( fromData );

	mpData		= &mOwnData[0];
	mReadPos	= 0;
	mIsReadOnly	= true;
}

//==================================================================
bool MemFile::ReadTextLine( char *pDestStr, size_t destStrMaxSize )
{
	if ( mReadPos >= mDataSize )
		return false;

	size_t	destIdx = 0;
	while ( mReadPos < mDataSize )
	{
		char ch = mpData[ mReadPos++ ];

		if ( ch == '\n' )
		{
			break;
		}
		else
		{
			if NOT( destIdx < destStrMaxSize )
				throw std::out_of_range( "Writing out of bounds !" );

			pDestStr[ destIdx++ ] = ch;
		}
	}

	if NOT( destIdx < destStrMaxSize )
		throw std::out_of_range( "Writing out of bounds !" );

	pDestStr[ destIdx ] = 0;

	DASSERT( mReadPos <= mDataSize );

	return true;
}

//==================================================================
void MemFile::ReadData( void *pDest, size_t readSize )
{
	memcpy( pDest, ReadDataPtr( readSize ), readSize );
}

//==================================================================
const void *MemFile::ReadDataPtr( size_t readSize )
{
	size_t	readPos = mReadPos;

	SeekFromCur( readSize );

	return mpData + readPos;
}

//==================================================================
void MemFile::SeekFromCur( size_t skipSize )
{
	size_t	endPos = mReadPos + skipSize;

	if NOT( endPos <= mDataSize )
		throw std::out_of_range( "Writing out of bounds !" );

	mpData += skipSize;
}

//==================================================================
}
