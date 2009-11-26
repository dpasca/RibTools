//==================================================================
/// RibRenderLib_Net_RenderBucketsServer.cpp
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib_Net_RenderBucketsServer.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// RenderBucketsServer
//==================================================================
RenderBucketsServer::RenderBucketsServer( DNET::PacketManager &pakMan ) :
	mpPakMan(&pakMan)
{
}

//==================================================================
void RenderBucketsServer::Render( RI::HiderREYES &hider )
{
	while ( true )
	{
		U32 ids[] = { MSGID_RENDBUCKETS, MSGID_RENDDONE };
		DNET::Packet *pPacket = mpPakMan->WaitNextPacketMatch( true, ids, _countof(ids), 10 );

		if NOT( pPacket )
		{
			if ( mpPakMan->IsConnected() )
				continue;
			else
			{
				printf( "Error: Connection interrupted, but no DONE rendering message received\n" );
				break;
			}
		}

		MsgID	msgID = GetMsgID( pPacket );

		if ( msgID == MSGID_RENDBUCKETS )
		{
			printf( "NETLOG RECV MSGID_RENDBUCKETS\n" );

			const MsgRendBuckes	&msg = *((const MsgRendBuckes *)&pPacket->mDataBuff[0]);

			rendBucketsRange( hider, msg.BucketStart, msg.BucketEnd );

			sendBucketsData( hider, msg.BucketStart, msg.BucketEnd );
		}
		else
		if ( msgID == MSGID_RENDDONE )
		{
			printf( "RECV MSGID_RENDDONE\n" );
			break;
		}

		mpPakMan->DeletePacket( pPacket );
	}
}

//==================================================================
void RenderBucketsServer::rendBucketsRange( RI::HiderREYES &hider, int buckRangeX1, int buckRangeX2 )
{
	const DVec<RI::HiderBucket *> &buckets = hider.GetBuckets();

	DASSERT(
		buckRangeX1 >= 0 &&
		buckRangeX1 < buckRangeX2 &&
		buckRangeX2 <= (int)buckets.size() );

	#pragma omp parallel for
	for (int bi=buckRangeX1; bi < buckRangeX2; ++bi)
		RI::FrameworkREYES::RenderBucket_s( hider, *buckets[ bi ] );
}

//==================================================================
void RenderBucketsServer::sendBucketsData( RI::HiderREYES &hider, int buckRangeX1, int buckRangeX2 )
{
	const DVec<RI::HiderBucket *> &buckets = hider.GetBuckets();

	for (int bi=buckRangeX1; bi < buckRangeX2; ++bi)
	{
		size_t	buckMemSize = hider.GetOutputBucketMemSize( (size_t)bi );

		DNET::Packet *pPacket;
		pPacket = mpPakMan->SendBegin( sizeof(MsgBucketData) + buckMemSize );

			U8 *pSendData = pPacket->GetDataPtrSend();

			MsgBucketData	&msg = *(new ( pSendData ) MsgBucketData());
			pSendData += sizeof(MsgBucketData);
			msg.BucketIdx = bi;

			hider.CopyOutputBucket( (size_t)bi, (float *)pSendData, buckMemSize );

		printf( "NETLOG SEND MSGID_BUCKETDATA\n" );
		mpPakMan->SendEnd( pPacket );
	}
}

//==================================================================
}
//==================================================================
}
