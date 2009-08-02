//==================================================================
/// DUtils_FileManager.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DUtils_FileManager.h"

//==================================================================
namespace DUT
{

//==================================================================
FileManager::FileManager() :
	mSock(INVALID_SOCKET),
	mIsConnectionBroken(false),
	mIsServing(true),
	mpPkMan(NULL)
{
}

//==================================================================
FileManager::FileManager( SOCKET sock, bool serving ) :
	mSock(sock),
	mIsConnectionBroken(false),
	mIsServing(serving),
	mpPkMan(NULL)
{
	mpPkMan = DNEW DNET::PacketManager( sock );
}

//==================================================================
FileManager::~FileManager()
{
	DSAFE_DELETE( mpPkMan );
}

//==================================================================
void FileManager::SendData( const void *pData, size_t dataSize )
{
	mpPkMan->Send( pData, dataSize );
}

//==================================================================
bool FileManager::GetFile( DUT::MemFile &memfile, const char *pFileName )
{
	if ( mpPkMan )
	{
		//mpPkMan->Send();

		DNET::Packet *pPacket = mpPkMan->WaitNextPacket();

		if NOT( pPacket )
		{
			DASSERT( 0 );
			//DASSTHROW( pPacket, ( "Could not retrieve the file %s !", pFileName ) );
			DSAFE_DELETE( pPacket );
			return false;
		}

		memfile.InitExclusiveOwenership( pPacket->mDataBuff );
		DSAFE_DELETE( pPacket );
	}
	else
	{
		memfile.Init( pFileName );
	}

	return true;
}

//==================================================================
bool FileManager::GetData( void *pDestData, size_t destSize )
{
	DASSERT( mpPkMan != NULL );

	DNET::Packet *pPacket = mpPkMan->WaitNextPacket();

	if ( !pPacket || pPacket->mDataBuff.size() != destSize )
	{
		DSAFE_DELETE( pPacket );
		//DASSERT( 0 );
		return false;
	}
	else
	{
		memcpy( pDestData, &pPacket->mDataBuff[0], destSize );
		DSAFE_DELETE( pPacket );
		return true;
	}
}

//==================================================================
}
