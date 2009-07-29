//==================================================================
/// DNetwork.h
///
/// Created by Davide Pasca - 2009/7/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DNETWORK_H
#define DNETWORK_H

#if defined(WIN32)

#include <WinSock2.h>

#elif defined(__linux__)

#include <sys/types.h>
#include <sys/socket.h>

typedef int	SOCKET;
static const SOCKET	INVALID_SOCKET = -1;

#endif

#include "DTypes.h"

//==================================================================
namespace DNET
{

bool InitializeSocket();

//==================================================================
class Listener
{
	U32		mPort;
	SOCKET	mListenSock;

public:
	enum IdleRetType
	{
		IRT_NOTHING,
		IRT_CONNECTED,
		IRT_ERROR,
		IRT_N
	};

public:
	Listener( U32 port );
	~Listener();

	bool		Start();
	void		Stop();
	IdleRetType Idle( SOCKET &out_acceptedSock, U32 waitMSec );
};

//==================================================================
}

#endif
