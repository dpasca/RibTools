//==================================================================
/// RRL_Net_FileServer.h
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_NET_FILESERVER_H
#define RRL_NET_FILESERVER_H

#include "DSystem/include/DNetwork_PacketManager.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
class FileServer : DTH::ThreadedBase
{
	DNET::PacketManager	*mpPakMan;

public:
	FileServer( DNET::PacketManager *pPakMan );
	~FileServer()
	{
		DTH::ThreadedBase::KillThread();
	}

private:
	void threadMain();
};

//==================================================================
}
//==================================================================
}

#endif
