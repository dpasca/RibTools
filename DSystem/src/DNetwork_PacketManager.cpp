//==================================================================
/// DNetwork_PacketManager.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(WIN32)
#include <WinSock2.h>
#include <windows.h>
#endif

#include "DNetwork.h"
#include "DNetwork_PacketManager.h"
#include "DUtils.h"

//==================================================================
namespace DNET
{

//==================================================================
/// PacketManager
//==================================================================
PacketManager::PacketManager( SOCKET socket )
    : mSocket(socket)
{
}

//==================================================================
PacketManager::~PacketManager()
{
    DTH::ThreadedBase::KillThread();
}

//==================================================================
bool PacketManager::IsConnected() const
{
    return mSocket != INVALID_SOCKET && !mFatalError;
}

//==================================================================
void PacketManager::Send( const void *pData, size_t dataSize )
{
    {
        std::lock_guard<std::mutex> lock( mSendList.mInQueueCS );

        Packet	*pPacket = DNEW Packet();

        mSendList.mInQueue.push_back( pPacket );

        pPacket->mDataBuff.resize( dataSize + sizeof(U32) );

        ((U32 *)&pPacket->mDataBuff[0])[0] = (U32)dataSize;

        memcpy(
            &pPacket->mDataBuff[sizeof(U32)],
            pData,
            dataSize );
    }

#if defined(WIN32)
    ResumeThread( mThreadHandle );
#endif
}

//==================================================================
Packet *PacketManager::SendBegin( size_t dataSize )
{
    //std::lock_guard<std::mutex> lock( mSendList.mInQueueCS );

    Packet	*pPacket = DNEW Packet();

    pPacket->mDataBuff.resize( dataSize + sizeof(U32) );

    U32	*ptr = (U32 *)&pPacket->mDataBuff[0];

    ptr[0] = (U32)dataSize;

    return pPacket;
}

//==================================================================
void PacketManager::SendEnd( Packet *pPacket )
{
    {
        std::lock_guard<std::mutex> lock( mSendList.mInQueueCS );

        mSendList.mInQueue.push_back( pPacket );
    }

#if defined(WIN32)
    ResumeThread( mThreadHandle );
#endif
}

//==================================================================
Packet *PacketManager::GetNextPacket( bool doRemove )
{
    if NOT( mRecvOutQueue.size() )
        return NULL;

    std::lock_guard<std::mutex> lock( mRecvOutQueueCS );

    if NOT( mRecvOutQueue.size() )
        return NULL;

    Packet	*pOutEntry = mRecvOutQueue[0];

    if ( doRemove )
        mRecvOutQueue.erase( mRecvOutQueue.begin() );

    return pOutEntry;
}

//==================================================================
Packet *PacketManager::GetNextPacketMatchID32(
                            bool doRemove,
                            U32 matchArray[],
                            size_t matchArrayN )
{
    if NOT( mRecvOutQueue.size() )
        return NULL;

    std::lock_guard<std::mutex> lock( mRecvOutQueueCS );

    if NOT( mRecvOutQueue.size() )
        return NULL;

    for (size_t i=0; i < mRecvOutQueue.size(); ++i)
    {
        Packet	*pOutEntry = mRecvOutQueue[i];
        DASSERT( pOutEntry->mDataBuff.size() >= sizeof(U32) );

        U32 packID = *((const U32 *)&pOutEntry->mDataBuff[0]);
        for (size_t j=0; j < matchArrayN; ++j)
        {
            if ( packID == matchArray[j] )
            {
                if ( doRemove )
                    mRecvOutQueue.erase( mRecvOutQueue.begin()+i );

                return pOutEntry;		
            }
        }
    }

    return NULL;
}

//==================================================================
Packet *PacketManager::GetNextPacketMatchID8(
                            bool doRemove,
                            U8 matchArray[],
                            size_t matchArrayN )
{
    if NOT( mRecvOutQueue.size() )
        return NULL;

    std::lock_guard<std::mutex> lock( mRecvOutQueueCS );

    if NOT( mRecvOutQueue.size() )
        return NULL;

    for (size_t i=0; i < mRecvOutQueue.size(); ++i)
    {
        Packet	*pOutEntry = mRecvOutQueue[i];
        DASSERT( pOutEntry->mDataBuff.size() >= sizeof(U8) );

        U8 packID = *((const U8 *)&pOutEntry->mDataBuff[0]);
        for (size_t j=0; j < matchArrayN; ++j)
        {
            if ( packID == matchArray[j] )
            {
                if ( doRemove )
                    mRecvOutQueue.erase( mRecvOutQueue.begin()+i );

                return pOutEntry;		
            }
        }
    }

    return NULL;
}

//==================================================================
void PacketManager::RemovePacket( Packet *pPacket )
{
    DASSERT( mRecvOutQueue.size() != 0 );

    std::lock_guard<std::mutex> lock( mRecvOutQueueCS );

    DASSERT( mRecvOutQueue.size() != 0 );

    for (size_t i=0; i < mRecvOutQueue.size(); ++i)
    {
        if ( pPacket == mRecvOutQueue[i] )
        {
            mRecvOutQueue.erase( mRecvOutQueue.begin() + i );
            return;
        }
    }

    DASSERT( 0 );
}

//==================================================================
void PacketManager::RemoveAndDeletePacket( Packet *pPacket )
{
    RemovePacket( pPacket );
    DSAFE_DELETE( pPacket );
}

//==================================================================
Packet *PacketManager::WaitNextPacket( bool doRemove, U32 timeoutMS )
{
    if ( mFatalError )
        return NULL;

    Packet			*pPacket;

    DUT::TimeOut	timeOut( timeoutMS );

    while ( !(pPacket = GetNextPacket( doRemove )) &&
            (timeoutMS == 0 || !timeOut.IsExpired()) )
    {
        if ( mFatalError )
            return NULL;

        DUT::SleepMS( 1 );
    }

    return pPacket;
}

//==================================================================
Packet *PacketManager::WaitNextPacketMatchID32(
                       bool doRemove,
                       U32 matchArray[],
                       size_t matchArrayN,
                       U32 timeoutMS )
{
    if ( mFatalError )
        return NULL;

    Packet			*pPacket;

    DUT::TimeOut	timeOut( timeoutMS );

    while ( !(pPacket = GetNextPacketMatchID32( doRemove, matchArray, matchArrayN )) &&
            (timeoutMS == 0 || !timeOut.IsExpired()) )
    {
        if ( mFatalError )
            return NULL;

        DUT::SleepMS( 1 );
    }

    return pPacket;
}

//==================================================================
void PacketManager::threadOnSockError( Packet &entry )
{
    // would block is ok.. otherwise..
}

//==================================================================
static int niceSend( SOCKET sock, const DVec<U8> &data, size_t &curSize )
{
    // send what's possible
    int doneSize = send(
                    sock,
                    (const char *)&data[ curSize ],
                    (int)(data.size() - curSize),
                    0 );

    // got an error ?
    if ( doneSize == SOCKET_ERROR )
    {
        int err = LastSockErr();
        if ( err == EWOULDBLOCK )
            return 1;
        else
        {
            printf( "niceSend(): Fatal socket error: %s\n",
                        GetSockErrStr( err ) );
            return -1;
        }
    }

    // update the sent size
    curSize += (size_t)doneSize;

    if ( curSize == data.size() )
        return 2;
    else
        return 0;
}

//==================================================================
static int niceRecv( SOCKET sock, DVec<U8> &data, size_t &curSize )
{
    // send what's possible
    int doneSize = recv(
                    sock,
                    (char *)&data[ curSize ],
                    (int)(data.size() - curSize),
                    0 );

    // got an error ?
    if ( doneSize == SOCKET_ERROR )
    {
        int err = LastSockErr();
        if ( err == EWOULDBLOCK )
            return 1;
        else
        {
            printf( "niceRecv(): Fatal socket error: %s\n",
                        GetSockErrStr( err ) );
            return -1;
        }
    }

    // update the sent size
    curSize += (size_t)doneSize;

    if ( curSize == data.size() )
        return 2;
    else
        return 0;
}

//==================================================================
void PacketManager::threadMain()
{
    size_t	curRecvPackDoneHeadSize = 0;
    size_t	curRecvPackDataSize = 0;
    size_t	curRecvPackDoneDataSize = 0;

    //size_t	curSendPackDoneHeadSize = 0;
    size_t	curSendPackDataSize = 0;

    DVec<U8>	recvBuff;

    while ( true )
    {
/*
        // $$$ This isn't really accurate...
        if ( mSendList.mQueue.size() == 0 && mSendList.mInQueue.size() == 0 )
            SuspendThread( mThreadHandle );
        else
            SwitchToThread();
*/

        if ( mQuitRequest )
        {
            // acknowledge quit request and returns
            mQuitAck = true;
            return;
        }

        // zombie state
        if ( mFatalError )
        {
            DUT::SleepMS( 10 );
            continue;
        }

        mSendList.FlushInQueue();

        fd_set readSocks;
        fd_set writeSocks;
        FD_ZERO( &readSocks );
        FD_ZERO( &writeSocks );

        FD_SET( mSocket, &readSocks );

        if ( mSendList.mQueue.size() )
            FD_SET( mSocket, &writeSocks );

        struct timeval	tv;
        tv.tv_sec	= 0;
        tv.tv_usec	= 1000;
        int selRet = select( mSocket+1, &readSocks, &writeSocks, NULL, &tv );

        if ( selRet == -1 )
        {
            mFatalError = true;
            continue;
        }

        // -------- handle sending ---------

        // as long as we have entries to send..
        while ( mSendList.mQueue.size() )
        {
            Packet	*pPacket = mSendList.mQueue[0];

            int retVal = niceSend( mSocket, pPacket->mDataBuff, curSendPackDataSize );

            switch ( retVal )
            {
            case 1:		// would block
                goto outOfSend;
            
            case -1:	// error !!
                mFatalError = true;
                goto fatalError;

            case 2:		// done with this 
                // delete it and remove it from the top of the list if it's done..
                DSAFE_DELETE( pPacket );
                mSendList.mQueue.erase( mSendList.mQueue.begin() );
                curSendPackDataSize = 0;
                break;
            }
        }

    outOfSend:;

        // -------- handle receiving ---------

        // ----- read the head
        if ( curRecvPackDoneHeadSize < sizeof(U32) )
        {
            // receive what's possible
            int doneSize = recv(
                            mSocket,
                            (char *)&curRecvPackDataSize,
                            (int)(sizeof(U32) - curRecvPackDoneHeadSize),
                            0 );

            // got an error ?
            if ( doneSize == SOCKET_ERROR )
            {
                int err = LastSockErr();
                if ( err != EWOULDBLOCK )
                {
                    printf( "PacketManager: Fatal socket error on receive: %s\n",
                                GetSockErrStr( err ) );
                    mFatalError = true;
                    goto fatalError;
                }
            }
            else
            {
                curRecvPackDoneHeadSize += (size_t)doneSize;

                // if the head is done, then set the size of the buffer for receiving
                if ( curRecvPackDoneHeadSize == sizeof(U32) )
                    recvBuff.resize( curRecvPackDataSize );
            }
        }

        // ----- read the body if the head has been set
        if ( curRecvPackDoneHeadSize == sizeof(U32) )
        {
            DASSERT( curRecvPackDoneDataSize < recvBuff.size() );

            // receive what's possible
            int retVal = niceRecv( mSocket, recvBuff, curRecvPackDoneDataSize );

            switch ( retVal )
            {
            case 1:		// would block
                break;
            
            case -1:	// error !!
                mFatalError = true;
                goto fatalError;

            case 2:		// done with this 
                {
                    std::lock_guard<std::mutex> lock( mRecvOutQueueCS );

                    mRecvOutQueue.push_back( DNEW Packet( recvBuff ) );
                }

                recvBuff.clear();

                // reset the head of this packet.. ready for next
                curRecvPackDoneHeadSize = 0;
                curRecvPackDataSize = 0;
                curRecvPackDoneDataSize = 0;
                break;
            }
        }

    fatalError:;
    }
}

//==================================================================
}
