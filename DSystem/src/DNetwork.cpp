//==================================================================
/// DNetwork.cpp
///
/// Created by Davide Pasca - 2009/7/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DNetwork.h"

#if defined(WIN32)

#pragma comment(lib, "wsock32.lib")

#endif

//==================================================================
namespace DNET
{

//==================================================================
bool InitializeSocket()
{
	static bool	initialized;

	if ( initialized )
		return true;

#if defined(WIN32)
	WSAData	data;

	if ( WSAStartup( MAKEWORD(2,2), &data ) != 0 )
	{
		return false;
	}
#endif

	initialized = true;

	return true;
}

//==================================================================
Listener::Listener( U32 port ) :
	mPort(port),
	mListenSock(INVALID_SOCKET)
{
}

//==================================================================
Listener::~Listener()
{
	Stop();
}

//==================================================================
bool Listener::Start()
{
	struct sockaddr_in	in_sa;

	mListenSock = socket(PF_INET, SOCK_STREAM, 0);

	if ( mListenSock == INVALID_SOCKET )
		return false;

	memset( &in_sa, 0, sizeof(in_sa) );
	in_sa.sin_family = AF_INET;
	in_sa.sin_addr.s_addr = htonl( INADDR_ANY );
	in_sa.sin_port        = htons( mPort );

	if ( -1 == bind( mListenSock, (struct sockaddr *)&in_sa, sizeof(in_sa) ) )
	{
		closesocket( mListenSock );
		return false;
	}

	if ( -1 == listen( mListenSock, SOMAXCONN ) )
	{
		closesocket( mListenSock );
		return false;
	}

	u_long	blockflg = 1;
	if ( ioctlsocket( mListenSock, FIONBIO, &blockflg ) < 0 )
	{
		closesocket( mListenSock );
		return false;
	}

	return true;
}

//==================================================================
void Listener::Stop()
{
	if ( mListenSock != INVALID_SOCKET )
	{
		closesocket( mListenSock );
		mListenSock = INVALID_SOCKET;
	}
}

//==================================================================
Listener::IdleRetType Listener::Idle( SOCKET &out_acceptedSock, U32 waitMSec )
{
	struct timeval	tv;
	fd_set			rdset;

	out_acceptedSock = INVALID_SOCKET;

	if ( mListenSock == INVALID_SOCKET )
		return IRT_NOTHING;

	FD_ZERO( &rdset );
	FD_SET( mListenSock, &rdset );

	tv.tv_sec = 0;
	tv.tv_usec = waitMSec * 1000;

	if ( select( 1, &rdset, NULL, 0, &tv ) == -1 )
	{
		Stop();
		return IRT_ERROR;
	}

	if ( FD_ISSET( mListenSock, &rdset ) )
	{
		out_acceptedSock = accept( mListenSock, 0, 0 );

		if ( out_acceptedSock != INVALID_SOCKET )
		{
			// make non-blocking
			u_long	blockflg = 1;
			if ( ioctlsocket( out_acceptedSock, FIONBIO, &blockflg ) < 0 )
			{
				closesocket( out_acceptedSock );
				out_acceptedSock = INVALID_SOCKET;
				return IRT_ERROR;
			}

			return IRT_CONNECTED;
		}
	}

	return IRT_NOTHING;
}

//==================================================================
}
