//==================================================================
/// RibRenderLib_Net_RenderBucketsClient.cpp
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib_Net_RenderBucketsClient.h"

//#define NO_LOCAL_RENDERING
#define BUCKETS_BATCH_SIZE	8

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// RenderBucketsClient
//==================================================================
RenderBucketsClient::RenderBucketsClient( DVec<Server> &servList ) :
	mpServList(&servList)
{

}

//==================================================================
bool RenderBucketsClient::isAnyServerAvailable() const
{
	for (size_t i=0; i < mpServList->size(); ++i)
	{
		Server &srv = (*mpServList)[i];

		if ( srv.IsConnected() )
			return true;
	}

	return false;
}

//==================================================================
void RenderBucketsClient::Render( RI::Hider &hider )
{
	DUT::QuickProf	prof( __FUNCTION__ );

	const DVec<RI::HiderBucket *> &buckets = hider.GetBuckets();

	int		bucketsN = (int)buckets.size();

	// loop through the buckets, rendering directly or assigning them
	// to other servers
	for (int buckRangeX1=0; buckRangeX1 < bucketsN; buckRangeX1 += BUCKETS_BATCH_SIZE)
	{
		// pick a range of "BUCKETS_BATCH_SIZE" buckets
		int buckRangeX2 = DMIN( buckRangeX1 + BUCKETS_BATCH_SIZE, bucketsN );

		// try to give the buckets to render to the server
	#if defined(NO_LOCAL_RENDERING)
		while NOT( dispatchToServer( buckRangeX1, buckRangeX2 ) )
		{
			// in case one would disconnect from all servers..
			if NOT( isAnyServerAvailable() )
			{
				printf( "! No servers available, rendering not possible !\n" );
				return;
			}

			checkServersData( hider, false );
			DUT::SleepMS( 10 );
		}
	#else
		if NOT( dispatchToServer( buckRangeX1, buckRangeX2 ) )
		{
			// otherwise render locally..
			#pragma omp parallel for
			for (int bi=buckRangeX1; bi < buckRangeX2; ++bi)
				RI::Framework::RenderBucket_s( hider, *buckets[ bi ] );
		}
	#endif

		checkServersData( hider, false );
	}

	// tell all servers that we are done rendering
	// ..and they should stop waiting for buckets
	sendRendDoneToNotBusy();

	// wait for all servers to complete !!
	while NOT( checkServersData( hider, true ) )
	{
		DUT::SleepMS( 10 );
	}
}

//==================================================================
bool RenderBucketsClient::checkServersData(
								RI::Hider &hider,
								bool replyDoneIfNotbusy )
{
	bool	allDone = true;

	for (size_t i=0; i < mpServList->size(); ++i)
	{
		Server &srv = (*mpServList)[i];

		if ( !srv.IsConnected() || srv.mBusyCnt == 0 )
			continue;

		DNET::Packet *pPacket = srv.mpPakMan->GetNextPacket( false );

		if ( pPacket )
		{
			DASSTHROW(
				pPacket->mDataBuff.size() >= sizeof(U32),
					("Received broken packet from a server !") );

			MsgID	msgID = GetMsgID( pPacket );

			if ( msgID == MSGID_BUCKETDATA )
			{
				const MsgBucketData	*pMsg =
					(const MsgBucketData *)pPacket->GetDataPtrRecv();

				const float *pPixData = (const float *)(pMsg + 1);

				size_t pixDataSize = pPacket->GetDataSize() -
											sizeof(MsgBucketData);

				printf( "NETLOG RECV MSGID_BUCKETDATA\n" );

				DASSERT(
					pixDataSize == hider.GetOutputBucketMemSize( pMsg->BucketIdx )
						);

				// store the pixel data
				hider.StoreOutputBucket(
						pMsg->BucketIdx,
						pPixData,
						pixDataSize );

				srv.mpPakMan->RemoveAndDeletePacket( pPacket );

				// bucket done
				srv.mBusyCnt -= 1;

				// waiting anymore buckets for this server ?
				if ( srv.mBusyCnt != 0 )
				{
					// if yes, then make sure we don't return an
					// all-done message
					allDone = false;
				}
				else // if all buckets are done for this server..
				if ( replyDoneIfNotbusy ) // do we want to relieve servers ?
				{	
					// send a goodbye message
					MsgRendDone	msg;
					srv.mpPakMan->SendValue( msg );
				}
			}
		}
		else
		{
			allDone = false;
		}
	}

	return allDone;
}

//==================================================================
Server * RenderBucketsClient::findFreeServer()
{
	for (size_t i=0; i < mpServList->size(); ++i)
	{
		Server &srv = (*mpServList)[i];

		DASSERT( srv.mBusyCnt >= 0 );

		if ( srv.IsConnected() && srv.mBusyCnt == 0 )
			return &srv;
	}

	return NULL;
}

//==================================================================
void RenderBucketsClient::sendRendDoneToNotBusy()
{
	for (size_t i=0; i < mpServList->size(); ++i)
	{
		Server &srv = (*mpServList)[i];

		if ( !srv.IsConnected() || srv.mBusyCnt )
			continue;

		MsgRendDone	msg;
		srv.mpPakMan->SendValue( msg );
	}
}

//==================================================================
bool RenderBucketsClient::dispatchToServer( int buckRangeX1, int buckRangeX2 )
{
	Server	*pUseServer = findFreeServer();
	if ( pUseServer )
	{
		int cnt = buckRangeX2 - buckRangeX1;

		MsgRendBuckes	msg;
		msg.BucketStart = (U32)buckRangeX1;
		msg.BucketEnd	= (U32)buckRangeX2;
		pUseServer->mpPakMan->SendValue( msg );
		pUseServer->mBusyCnt += cnt;	// set as busy
		return true;
	}
	else
		return false;
}

//==================================================================
}
//==================================================================
}
