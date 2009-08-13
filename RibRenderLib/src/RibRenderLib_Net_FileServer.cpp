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

		U32 ids[] = { MSGID_FILEEXISTREQ, MSGID_FILEREQ };
		DNET::Packet *pPacket = mpPakMan->WaitNextPacketMatch( true, ids, _countof(ids), 20 );

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
				mpPakMan->DeletePacket( pPacket );
				continue;
			}

			if ( DUT::FileExists( buff ) )
			{
				printf( "NETLOG: SEND MSGID_FILEEXISTANSYES (%s)\n", buff );
				mpPakMan->SendValue( (U32)MSGID_FILEEXISTANSYES );
			}
			else
			{
				printf( "NETLOG: SEND MSGID_FILEEXISTANSNO (%s)\n", buff );
				mpPakMan->SendValue( (U32)MSGID_FILEEXISTANSNO );
			}

			mpPakMan->DeletePacket( pPacket );
		}
		else
		if ( msgID == MSGID_FILEREQ )
		{
			if NOT( makeString( buff, sizeof(buff), reader ) )
			{
				printf( "FileServer: requested bad filename !\n" );
				mpPakMan->DeletePacket( pPacket );
				continue;
			}

			size_t	fileSize;
			FILE	*pFile;
			if NOT( pFile = DUT::BeginGrabFile( buff, fileSize ) )
			{
				printf( "NETLOG: SEND MSGID_FILEREQANS_FAIL (%s)\n", buff );
				mpPakMan->SendValue( (U32)MSGID_FILEREQANS_FAIL );
			}
			else
			{
				// $$$ does not handle failure in read...

				U8	*pPackData = mpPakMan->SendBegin( fileSize + sizeof(U32) );
				DUT::MemWriter	writer( pPackData, fileSize + sizeof(U32) );
				writer.WriteValue( (U32)MSGID_FILEREQANS_DATA );

				DUT::EndGrabFile( pFile, writer.GetDataPtr( fileSize ), fileSize );

				printf( "NETLOG: SEND MSGID_FILEREQANS_DATA (%s)\n", buff );
				mpPakMan->SendEnd();
			}

			mpPakMan->DeletePacket( pPacket );
		}
	}
}

//==================================================================
}
//==================================================================
}
