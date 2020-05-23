//==================================================================
/// RRL_Net_FileManager.cpp
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RRL_Net.h"
#include "RRL_Net_FileManager.h"

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

    DUT::LongCriticalSection::Block	lock( mLongCS );

    mpPakMan->Send( buff, writer.GetCurSize() );

    printf( "NETLOG: SEND MSGID_FILEREQ %s\n", pFileName );

    while ( true )
    {
        if NOT( mpPakMan->IsConnected() )
            DEX_RUNTIME_ERROR( "Could not read %s, connection broken !", pFileName );

        U32 ids[] = { MSGID_FILEREQANS_DATA, MSGID_FILEREQANS_FAIL };
        DNET::Packet *pPacket = mpPakMan->WaitNextPacketMatchID32( true, ids, _countof(ids), 10 );

        if ( pPacket )
        {
            MsgID	msgID = GetMsgID( pPacket );

            switch ( msgID )
            {
            case MSGID_FILEREQANS_DATA:
                printf( "NETLOG: RECV MSGID_FILEREQANS_DATA\n" );
                {
                    size_t dataSize = pPacket->mDataBuff.size() - sizeof(U32);
                    out_vec.resize( dataSize );
                    memcpy( &out_vec[0], &pPacket->mDataBuff[sizeof(U32)], dataSize );
                }
                break;

            case MSGID_FILEREQANS_FAIL:
                printf( "NETLOG: RECV MSGID_FILEREQANS_FAIL\n" );
                DEX_RUNTIME_ERROR( "ERROR: Could not retrieve the file %s !\n", pFileName );
                break;

            default:
                printf( "NETLOG: RECV *UNKNOWN*\n" );
                break;
            }

            mpPakMan->DeletePacket( pPacket );
            break;
        }
    }
}

//===============================================================
bool FileManagerNet::FileExists( const char *pFileName )
{
    char buff[2048];

    U32 nameLen = (U32)strlen(pFileName);
    DASSERT( nameLen < _countof(buff) );

    DUT::MemWriter	writer( buff, sizeof(buff) );
    writer.WriteValue( MSGID_FILEEXISTREQ );
    writer.WriteValue( nameLen );
    writer.WriteArray( pFileName, nameLen );

    DUT::LongCriticalSection::Block	lock( mLongCS );

    mpPakMan->Send( buff, writer.GetCurSize() );

    printf( "NETLOG: SEND MSGID_FILEEXISTREQ %s\n", pFileName );

    while ( true )
    {
        if NOT( mpPakMan->IsConnected() )
            DEX_RUNTIME_ERROR( "Could not read %s, connection broken !", pFileName );

        U32 ids[] = { MSGID_FILEEXISTANSYES, MSGID_FILEEXISTANSNO };
        DNET::Packet *pPacket = mpPakMan->WaitNextPacketMatchID32( true, ids, _countof(ids), 10 );

        if ( pPacket )
        {
            MsgID msgID = GetMsgID( pPacket );
            switch ( msgID )
            {
            case MSGID_FILEEXISTANSYES:
                printf( "NETLOG: RECV MSGID_FILEEXISTANSYES\n" );
                return true;

            case MSGID_FILEEXISTANSNO:
                printf( "NETLOG: RECV MSGID_FILEEXISTANSNO\n" );
                return false;

            default:
                printf( "NETLOG: RECV *UNKNOWN*\n" );
                break;
            }

            mpPakMan->DeletePacket( pPacket );
        }
    }
}

//==================================================================
}
//==================================================================
}
