//==================================================================
/// DUtils_MemFile.cpp
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
MemFile::MemFile( MemWriterDynamic &mw )
{
	InitExclusiveOwenership( mw );
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
		DEX_RUNTIME_ERROR( "Could not open the file '%s' for reading.", pFileName );
	}

	mpData = (const U8 *)&mOwnData[0];
	mDataSize = mOwnData.size();
}

//==================================================================
bool MemFile::InitNoThrow( const char *pFileName )
{
	DASSERT( mpData == NULL && mDataSize == 0 && mReadPos == 0 );

	if NOT( GrabFile( pFileName, mOwnData ) )
	{
		return false;
	}

	mDataSize = mOwnData.size();
	if ( mDataSize )
		mpData = (const U8 *)&mOwnData[0];

	return true;
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
void MemFile::InitExclusiveOwenership( MemWriterDynamic &mw )
{
	InitExclusiveOwenership( mw.mDest );
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
			if ( destIdx >= destStrMaxSize )
				DEX_OUT_OF_RANGE( "Writing out of bounds !" );

			pDestStr[ destIdx++ ] = ch;
		}
	}

	if ( destIdx >= destStrMaxSize )
		DEX_OUT_OF_RANGE( "Writing out of bounds !" );

	pDestStr[ destIdx ] = 0;

	DASSERT( mReadPos <= mDataSize );

	return true;
}

//==================================================================
void MemWriterDynamic::WriteString( const DStr &str )
{
	WriteArray( str.c_str(), str.size() );
}

//==================================================================
void MemWriterDynamic::WriteString( const char *pStr )
{
	WriteArray( pStr, strlen(pStr) );
}

//==================================================================
void MemWriterDynamic::PrintF( const char *pFmt, ... )
{
	va_list	vl;
	va_start( vl, pFmt );

	char	buff[1024];
	vsnprintf( buff, _countof(buff)-1, pFmt, vl );

	va_end( vl );

	WriteString( buff );
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
void MemFile::SeekSet( size_t pos )
{
	if ( pos < 0 || pos > mDataSize )
		throw std::out_of_range( "Writing out of bounds !" );

	mReadPos = pos;
}

//==================================================================
void MemFile::SeekFromCur( ptrdiff_t offset )
{
	SeekSet( (ptrdiff_t)mReadPos + offset );
}

//==================================================================
void MemFile::SeekFromEnd( ptrdiff_t offset )
{
	SeekSet( (ptrdiff_t)mDataSize + offset );
}

//==================================================================
}
