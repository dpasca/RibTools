//==================================================================
/// DNetwork_Connecter.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DNETWORK_CONNECTER_H
#define DNETWORK_CONNECTER_H

#include "DNetwork_Base.h"

//==================================================================
namespace DNET
{

//==================================================================
class Connecter
{
	SOCKET		mSock;
	sockaddr_in	mSockAddr;
	bool		mIsConnected;

public:
	enum RetVal
	{
		RETVAL_WAITING,
		RETVAL_CONNECTED,
		RETVAL_ERROR,
		RETVAL_N,
	};

public:
	//==================================================================
	Connecter( const char *pIPName, int port );
	~Connecter();

	bool IsConnected() const { return mIsConnected;	}

	RetVal TryConnect();
	SOCKET GetSocket();

};


//==================================================================
}

#endif
