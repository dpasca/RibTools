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
#include "DSystem/include/DUtils_FileManager.h"

#include "RI_System/include/RI_FrameworkREYES.h"

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
	DUT::FileManager	*mpFileManager;

public:
	RenderBucketsServer( DUT::FileManager &fileManager );
	void Render( RI::HiderREYES &hider );

private:
	void rendBucketsRange( RI::HiderREYES &hider, int buckRangeX1, int buckRangeX2 );
	void sendBucketsData( RI::HiderREYES &hider, int buckRangeX1, int buckRangeX2 );
};

//==================================================================
}
//==================================================================
}

#endif
