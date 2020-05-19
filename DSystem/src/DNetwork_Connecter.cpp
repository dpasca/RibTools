//==================================================================
/// DNetwork_Connecter.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

// #define D_LOGGING

#include <memory.h>
#include "DUtils_Base.h"
#include "DNetwork_Connecter.h"
#include "dlog.h"
#include "DExceptions.h"

#if defined(WIN32)
# include <WinSock2.h>
#endif

//==================================================================
namespace DNET
{

//==================================================================
Connecter::Connecter( const char *pIPName, u_short port ) :
	mIsConnected(false)
{
	if ( pIPName )
	{
		switch ( StartConnect( pIPName, port ) )
		{
		case STARTRET_OK:
			break;

		case STARTRET_INVALID_ADDR:
			DEX_RUNTIME_ERROR( "Invalid network address '%s'", pIPName );
			break;

		default:
		case STARTRET_GENERIC_ERROR:
			DEX_RUNTIME_ERROR( "Failed to start connection to %s'", pIPName );
			break;
		}
	}
}

//==================================================================
Connecter::StartRet Connecter::StartConnect( const char *pIPName, u_short port )
{
	sockaddr_in	sockAddr;

	memset( &sockAddr, 0, sizeof(sockAddr) );

	u_long	addr;		// in_addr_t ?

	addr = inet_addr( pIPName );
	if ( addr == INADDR_NONE )
	{
		struct hostent	*pHostent;

		if NOT( pHostent = gethostbyname( pIPName ) )
		{
			DLOG( "Invalid network address '%s'", pIPName );
			return STARTRET_INVALID_ADDR;
		}

		addr = *(long *)pHostent->h_addr;
	}

	memcpy( &sockAddr.sin_addr, &addr, sizeof(long) );

	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons( port );

	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if ( mSock == INVALID_SOCKET )
	{
		DLOG( "Failed to create the socket" );
		return STARTRET_GENERIC_ERROR;
	}

	if NOT( SetNonBlocking( mSock ) )
	{
		DLOG( "Failed to set the socket as non-blocking" );
		closesocket( mSock );
		mSock = INVALID_SOCKET;
		return STARTRET_GENERIC_ERROR;
	}

	int connectRet = connect( mSock, (struct sockaddr *)&sockAddr, sizeof(sockAddr) );

	if ( connectRet == INVALID_SOCKET )
	{
		int	err = LastSockErr();

		if ( err == EWOULDBLOCK )
		{
			// fine.. we can wait..
		}
		else
		if ( err == EISCONN || err == EALREADY )
		{
			// i guess it's ok too
		}
		else
		{
			DLOG( "Failed to connect" );
			closesocket( mSock );
			mSock = INVALID_SOCKET;
			return STARTRET_GENERIC_ERROR;
		}
	}

	return STARTRET_OK;
}

//==================================================================
Connecter::~Connecter()
{
	if ( mSock != INVALID_SOCKET )
		closesocket( mSock );
}

//==================================================================
Connecter::IdleRet Connecter::IdleConnect()
{
	if ( mIsConnected )
		return IDLERET_CONNECTED;

	fd_set socks;
	FD_ZERO( &socks );
	FD_SET( mSock, &socks );

	SOCKET	highSock = mSock;

	struct timeval tv;
	tv.tv_sec	= 0;
	tv.tv_usec	= 1000;	// something minimal

	int goodSocks = select( highSock+1, NULL, &socks, NULL, &tv );

	if ( goodSocks == -1 )
	{
		return IDLERET_ERROR;
	}
	if ( goodSocks > 0 )	// or rather, "1" in this case..
	{
		mIsConnected = true;
		return IDLERET_CONNECTED;
	}
	else
		return IDLERET_WAITING;
}

//==================================================================
SOCKET Connecter::GetSocket()
{
	if ( mIsConnected )
	{
		SOCKET	sock = mSock;
		mSock = INVALID_SOCKET;

		return sock;
	}
	else
		return INVALID_SOCKET;
}

//==================================================================
}
