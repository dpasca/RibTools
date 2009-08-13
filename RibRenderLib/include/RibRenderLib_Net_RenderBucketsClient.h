//==================================================================
/// RibRenderLib_Net_RenderBucketsClient.h
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_NET_RENDERBUCKETSCLIENT_H
#define RIBRENDERLIB_NET_RENDERBUCKETSCLIENT_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DUtils.h"

#include "RI_System/include/RI_FrameworkREYES.h"

#include "RibRenderLib_Net.h"

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
	void Render( RI::HiderREYES &hider );

private:
	Server *findFreeServer();
	bool dispatchToServer( int buckRangeX1, int buckRangeX2 );
	bool checkServersData();
	bool isAnyServerAvailable() const;
};

//==================================================================
}
//==================================================================
}

#endif
