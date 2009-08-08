//==================================================================
/// RibRenderLib_Net_FileServer.cpp
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib_Net.h"
#include "RibRenderLib_Net_FileServer.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
FileServer::FileServer( DNET::PacketManager *pPakMan ) :
	mpPakMan(pPakMan)
{

}

//==================================================================
static bool makeString( char *pDest, size_t destMaxSize, DUT::MemReader &reader )
{
	U32 strLen = reader.ReadValue<U32>();

	if ( strLen >= destMaxSize )
	{
		return false;
	}

	memcpy( pDest, reader.GetDataPtr( strLen ), strLen );
	pDest[ strLen ] = 0;

	return true;
}

//==================================================================
void FileServer::threadMain()
{
	while ( true )
	{
		if ( mQuitRequest )
		{
			// acknowledge quit request and returns
			mQuitAck = true;
			return;
		}

		DNET::Packet *pPacket = mpPakMan->WaitNextPacket( false, 20 );
		if NOT( pPacket )
			continue;

		char buff[1024];

		DUT::MemReader	reader( pPacket->mDataBuff );

		U32	msgID = reader.ReadValue<U32>();

		if ( msgID == MSGID_FILEEXISTREQ )
		{
			if NOT( makeString( buff, sizeof(buff), reader ) )
			{
				printf( "FileServer: requested bad filename !\n" );
				mpPakMan->RemoveAndDeletePacket( pPacket );
				continue;
			}

			printf( "FileServer: exit req for %s\n", buff );	
			mpPakMan->RemoveAndDeletePacket( pPacket );
		}
		else
		if ( msgID == MSGID_FILEEXISTREQ )
		{
			if NOT( makeString( buff, sizeof(buff), reader ) )
			{
				printf( "FileServer: requested bad filename !\n" );
				mpPakMan->RemoveAndDeletePacket( pPacket );
				continue;
			}

			printf( "FileServer: exit req for %s\n", buff );	
			mpPakMan->RemoveAndDeletePacket( pPacket );
		}
	}
}

//==================================================================
}
//==================================================================
}
