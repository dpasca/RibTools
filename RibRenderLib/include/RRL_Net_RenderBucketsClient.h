//==================================================================
/// RRL_Net_RenderBucketsClient.h
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_NET_RENDERBUCKETSCLIENT_H
#define RRL_NET_RENDERBUCKETSCLIENT_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DUtils.h"

#include "RI_System/include/RI_Framework.h"

#include "RRL_Net.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// RenderBucketsClient
//==================================================================
class RenderBucketsClient : public RI::RenderBucketsBase
{
	DVec<Server>	*mpServList;

public:
	RenderBucketsClient( DVec<Server> &servList );
	void Render( RI::Hider &hider );

private:
	Server *findFreeServer();
	bool dispatchToServer( int buckRangeX1, int buckRangeX2 );

	bool checkServersData(
					RI::Hider &hider,
					bool replyDoneIfNotbusy );

	bool isAnyServerAvailable() const;
	void sendRendDoneToNotBusy();
};

//==================================================================
}
//==================================================================
}

#endif
