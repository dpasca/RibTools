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
	bool		mIsConnected;

public:
	enum StartRet
	{
		STARTRET_OK,
		STARTRET_INVALID_ADDR,
		STARTRET_GENERIC_ERROR,
		STARTRET_N
	};

	enum IdleRet
	{
		IDLERET_WAITING,
		IDLERET_CONNECTED,
		IDLERET_ERROR,
		IDLERET_N,
	};

public:
	//==================================================================
	Connecter( const char *pIPName=NULL, u_short port=0 );
	~Connecter();

	StartRet StartConnect( const char *pIPName, u_short port );

	//bool HasStarted() const { return mSock != INVALID_SOCKET;	}
	bool IsConnected() const { return mIsConnected;	}

	IdleRet IdleConnect();
	SOCKET GetSocket();
};


//==================================================================
}

#endif
