//==================================================================
/// RibRenderLib_Net_FileManager.cpp
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib_Net.h"
#include "RibRenderLib_Net_FileManager.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// FileManagerNet
//==================================================================
FileManagerNet::FileManagerNet( DNET::PacketManager &packManager ) :
	mpPakMan(&packManager)
{
}

//===============================================================
void FileManagerNet::GrabFile( const char *pFileName, DVec<U8> &out_vec )
{
	char buff[2048];

	U32 nameLen = (U32)strlen(pFileName);
	DASSERT( nameLen < _countof(buff) );

	DUT::MemWriter	writer( buff, sizeof(buff) );
	writer.WriteValue( MSGID_FILEREQ );
	writer.WriteValue( nameLen );
	writer.WriteArray( pFileName, nameLen );
	mpPakMan->Send( buff, writer.GetCurSize() );

	while ( true )
	{
		DNET::Packet *pPacket = mpPakMan->WaitNextPacket( false, 10 );

		if ( pPacket )
		{
			switch ( GetMsgID( pPacket ) )
			{
			case MSGID_FILEREQANS_DATA:
				printf( "Got data for file %s\n", pFileName );
				{
					size_t dataSize = pPacket->mDataBuff.size() - sizeof(U32);
					out_vec.resize( dataSize );
					memcpy( &out_vec[0], &pPacket->mDataBuff[0], dataSize );
				}
				mpPakMan->RemoveAndDeletePacket( pPacket );
				break;

			case MSGID_FILEREQANS_FAIL:
				DASSTHROW( 0, ( "ERROR: Could not retrieve the file %s !\n", pFileName ) );
				mpPakMan->RemoveAndDeletePacket( pPacket );
				break;
			}
		}
	}
}

//===============================================================
bool FileManagerNet::FileExists( const char *pFileName ) const
{
	char buff[2048];

	U32 nameLen = (U32)strlen(pFileName);
	DASSERT( nameLen < _countof(buff) );

	DUT::MemWriter	writer( buff, sizeof(buff) );
	writer.WriteValue( MSGID_FILEEXISTREQ );
	writer.WriteValue( nameLen );
	writer.WriteArray( pFileName, nameLen );
	mpPakMan->Send( buff, writer.GetCurSize() );

	while ( true )
	{
		DNET::Packet *pPacket = mpPakMan->WaitNextPacket( false, 10 );

		if ( pPacket )
		{
			switch ( GetMsgID( pPacket ) )
			{
			case MSGID_FILEEXISTANSYES:
				return true;

			case MSGID_FILEEXISTANSNO:
				return false;
			}
		}
	}
}

//==================================================================
}
//==================================================================
}
