//==================================================================
/// DNetwork_Listener.h
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DNETWORK_LISTENER_H
#define DNETWORK_LISTENER_H

#include "DNetwork_Base.h"

//==================================================================
namespace DNET
{

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
