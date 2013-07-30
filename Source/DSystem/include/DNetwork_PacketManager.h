//==================================================================
/// DNetwork_PacketManager.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DNETWORK_PACKETMANAGER_H
#define DNETWORK_PACKETMANAGER_H

#include <memory.h>
#include "DNetwork_Base.h"
#include "DContainers.h"
#include "DCriticalSection.h"
#include "DThreads.h"

//==================================================================
namespace DNET
{

//==================================================================
/// Packet
//==================================================================
class Packet
{
public:
	DVec<U8>	mDataBuff;

	Packet()
	{
	}

	Packet( const DVec<U8> &dataBuff ) :
		mDataBuff(dataBuff)
	{
	}

/*
	virtual ~Packet()
	{
	}
*/
	const U8 *GetDataPtrRecv() const
	{
		if ( mDataBuff.size() )
			return &mDataBuff[0];
		else
			return NULL;
	}

	U8 *GetDataPtrSend()
	{
		if ( mDataBuff.size() >= sizeof(U32) )
			return &mDataBuff[sizeof(U32)];
		else
			return NULL;
	}

	size_t GetDataSize() const
	{
		return mDataBuff.size();
	}

private:
	Packet( const Packet &from ) {}
	void operator =( const Packet &from ) {}
};

//===============================================================
/// 
//===============================================================
class PacketManager : public DTH::ThreadedBase
{
	class Queue
	{
	public:
		DVec<Packet *>			mInQueue;
		DUT::CriticalSection	mInQueueCS;
		DVec<Packet *>			mQueue;

		//==================================================================
		void FlushInQueue()
		{
			if NOT( mInQueue.size() )
				return;

			DUT::CriticalSection::Block	lock( mInQueueCS );

			for (size_t i=0; i < mInQueue.size(); ++i)
				mQueue.push_back( mInQueue[i] );

			mInQueue.clear();
		}
	};

	SOCKET					mSocket;
	Queue					mSendList;

	DUT::CriticalSection	mRecvOutQueueCS;
	DVec<Packet *>			mRecvOutQueue;

	bool					mFatalError;

public:
	//===============================================================
	PacketManager( SOCKET socket );
	~PacketManager();

	bool IsConnected() const;

	void Send( const void *pData, size_t dataSize );

	template<class T>
	void SendValue( const T &val )
	{
		Send( &val, sizeof(val) );
	}

	Packet *SendBegin( size_t dataSize );
	void SendEnd( Packet *pPacket );

	Packet *GetNextPacket( bool doRemove );
	Packet *WaitNextPacket( bool doRemove, U32 timeoutMS=0 );

	Packet *GetNextPacketMatchID32(
		bool doRemove,
		U32 matchArray[],
		size_t matchArrayN );

	Packet *GetNextPacketMatchID8(
		bool doRemove,
		U8 matchArray[],
		size_t matchArrayN );

	Packet *WaitNextPacketMatchID32(
			bool doRemove,
			U32 matchArray[],
			size_t matchArrayN,
			U32 timeoutMS=0 );

	void RemovePacket( Packet *pPacket );
	void RemoveAndDeletePacket( Packet *pPacket );

	void DeletePacket( Packet * &pPacket )
	{
		DSAFE_DELETE( pPacket )
	}

private:
		void threadMain();
	
	void threadOnSockError( Packet &entry );
};

//==================================================================
}

#endif
