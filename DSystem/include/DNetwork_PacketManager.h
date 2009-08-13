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

	virtual ~Packet()
	{
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

	bool IsConnected() const { return mSocket != INVALID_SOCKET && !mFatalError; }

	void Send( const void *pData, size_t dataSize );

	template<class T>
	void SendValue( const T &val)
	{
		Send( &val, sizeof(val) );
	}

	U8 *SendBegin( size_t dataSize );
	void SendEnd();

	Packet *GetNextPacket( bool doRemove );
	Packet *WaitNextPacket( bool doRemove, U32 timeoutMS=0 );

	Packet *GetNextPacketMatch(
		bool doRemove,
		U32 matchArray[],
		size_t matchArrayN );

	Packet *WaitNextPacketMatch(
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
