//==================================================================
/// RRL_Net_Server.h
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_NET_SERVER_H
#define RRL_NET_SERVER_H

#include "DSystem/include/DNetwork_PacketManager.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

class FileManagerNet;
class FileServer;

//===============================================================
/// Server
//==================================================================
class Server
{
public:
	DStr				mAddressName;
	int					mPortToCall;
	DNET::PacketManager	*mpPakMan;
	FileManagerNet		*mpFilemanager;
	FileServer			*mpFileServer;
	bool				mIsValid;
	int					mBusyCnt;

	Server();
	~Server();

	void Init( DNET::PacketManager *pPakMan );
	bool IsConnected() const;
};

//==================================================================
}
//==================================================================
}

#endif
