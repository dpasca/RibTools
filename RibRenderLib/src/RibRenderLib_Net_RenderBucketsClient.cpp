//==================================================================
/// RibRenderLib_Net_RenderBucketsClient.cpp
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib_Net_RenderBucketsClient.h"

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
void RenderBucketsClient::Render( RI::HiderREYES &hider )
{
	DUT::QuickProf	prof( __FUNCTION__ );

	const DVec<RI::Bucket *> &buckets = hider.GetBuckets();

	int		bucketsN = (int)buckets.size();

	for (int buckRangeX1=0; buckRangeX1 < bucketsN; buckRangeX1 += 4)
	{
		// pick a range of 4 buckets
		int buckRangeX2 = DMIN( buckRangeX1 + 4, bucketsN );

		// try to give the buckets to render to the server
		if NOT( dispatchToServer( buckRangeX1, buckRangeX2 ) )
		{
			// otherwise render locally..
			//#pragma omp parallel for
			for (int bi=buckRangeX1; bi < buckRangeX2; ++bi)
				RI::FrameworkREYES::RenderBucket_s( hider, *buckets[ bi ] );
		}

		checkServersData();
	}

	// wait for all servers to deliver !!
	while NOT( checkServersData() )
	{
		Sleep( 10 );
	}
}

//==================================================================
bool RenderBucketsClient::checkServersData()
{
	bool	allDone = true;

	for (size_t i=0; i < mpServList->size(); ++i)
	{
		Server &srv = (*mpServList)[i];

		if ( !srv.IsConnected() || !srv.mIsBusy )
			continue;

		DNET::Packet *pPacket = srv.mpFilemanager->mpPkMan->GetNextPacket();

		if ( pPacket )
		{
			DASSTHROW(
				pPacket->mDataBuff.size() >= sizeof(U32),
					("Received broken packet from a server !") );

			MsgID	msgID = (MsgID)((const U32 *)&pPacket->mDataBuff[0])[0];
			if ( msgID == MSGID_BUCKETDATA )
			{
				srv.mIsBusy = false;
			}
			else
			{
				DASSTHROW(
					0,
						("Received broken packet from a server !") );
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

		if ( srv.IsConnected() && !srv.mIsBusy )
			return &srv;
	}

	return NULL;
}

//==================================================================
bool RenderBucketsClient::dispatchToServer( int buckRangeX1, int buckRangeX2 )
{
	Server	*pUseServer = findFreeServer();
	if ( pUseServer )
	{
		MsgRendBuckes	msg;
		msg.BucketStart = (U32)buckRangeX1;
		msg.BucketEnd	= (U32)buckRangeX2;
		pUseServer->mpFilemanager->SendData( &msg, sizeof(msg) );
		pUseServer->mIsBusy = true;	// set as busy
		return true;
	}
	else
		return false;
}

//==================================================================
}
//==================================================================
}
