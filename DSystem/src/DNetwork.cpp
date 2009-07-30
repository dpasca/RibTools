//==================================================================
/// DNetwork.cpp
///
/// Created by Davide Pasca - 2009/7/30
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <memory.h>

#include "DNetwork.h"

#if defined(WIN32)

#pragma comment(lib, "wsock32.lib")

#elif defined(__linux__)

#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

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
static bool setNonBlocking( SOCKET sock )
{
#if defined(WIN32)
	u_long	blockflg = 1;
	if ( -1 == ioctlsocket( mListenSock, FIONBIO, &blockflg ) )
		return false;

#elif defined(__linux__)
	if ( -1 == fcntl( sock, F_SETFL, O_NONBLOCK ) )
		return false;

#endif

	return true;
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
		mListenSock = -1;
		return false;
	}

	if ( setNonBlocking( mListenSock ) )
	{
		closesocket( mListenSock );
		mListenSock = -1;
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
			if ( setNonBlocking( mListenSock ) )
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
