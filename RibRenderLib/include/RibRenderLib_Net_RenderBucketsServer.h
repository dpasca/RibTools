//==================================================================
/// RibRenderLib_Net_RenderBucketsServer.h
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_NET_RENDERBUCKETSSERVER_H
#define RIBRENDERLIB_NET_RENDERBUCKETSSERVER_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DUtils.h"
#include "RI_System/include/RI_Framework.h"

#include "RibRenderLib_Net.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// RenderBucketsServer
//==================================================================
class RenderBucketsServer : public RI::RenderBucketsBase
{
	DNET::PacketManager	*mpPakMan;

public:
	RenderBucketsServer( DNET::PacketManager &pakMan );
	void Render( RI::Hider &hider );

private:
	void rendBucketsRange( RI::Hider &hider, int buckRangeX1, int buckRangeX2 );
	void sendBucketsData( RI::Hider &hider, int buckRangeX1, int buckRangeX2 );
};

//==================================================================
}
//==================================================================
}

#endif
