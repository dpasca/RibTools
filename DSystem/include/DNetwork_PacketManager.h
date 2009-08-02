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
#include "DContainers.h"
#include "DCriticalSection.h"
#include "DNetwork_Base.h"

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
class PacketManager
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

	DVec<SOCKET>			mTroubledSocketsList;
	DUT::CriticalSection	mTroubledSocketsListCSection;

	bool					mFatalError;
	bool					mQuitRequest;
	bool					mQuitAck;

#if defined(WIN32)
	HANDLE					mThreadHandle;
#endif


public:
	//===============================================================
	PacketManager( SOCKET socket );
	~PacketManager();

	void Send( const void *pData, size_t dataSize );

	Packet *GetNextPacket();
	Packet *WaitNextPacket();

private:
	static void threadMain_s( void *pThis )
	{
		((PacketManager *)pThis)->threadMain();
	}

	void threadMain();
	void threadOnSockError( Packet &entry );
};

//==================================================================
}

#endif
