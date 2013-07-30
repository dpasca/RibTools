//==================================================================
/// DNetwork_Connecter.cpp
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include <memory.h>
#include "DUtils_Base.h"
#include "DNetwork_Connecter.h"

//==================================================================
namespace DNET
{

//==================================================================
Connecter::Connecter( const char *pIPName, int port ) :
	mIsConnected(false)
{
	memset( &mSockAddr, 0, sizeof(mSockAddr) );

	u_long	addr;		// in_addr_t ?

	addr = inet_addr( pIPName );
	if ( addr == INADDR_NONE )
	{
		struct hostent	*pHostent;

		if NOT( pHostent = gethostbyname( pIPName ) )
		{
			// invalid address
			DASSTHROW( 0, ("Invalid address") );
			//return false;
		}

		addr = *(long *)pHostent->h_addr;
	}

	memcpy( &mSockAddr.sin_addr, &addr, sizeof(long) );

	mSockAddr.sin_family = AF_INET;
	mSockAddr.sin_port = htons( port );

	mSock = socket(AF_INET, SOCK_STREAM, 0);
	if ( mSock == INVALID_SOCKET )
	{
		DASSTHROW( 0, ("Failed to create the socket") );
		//return -1;
	}

	if NOT( SetNonBlocking( mSock ) )
	{
		closesocket( mSock );
		mSock = INVALID_SOCKET;
		DASSTHROW( 0, ("Failed to set the socket as non-blocking") );
	}

	int connectRet = connect( mSock, (struct sockaddr *)&mSockAddr, sizeof(mSockAddr) );

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
			DASSTHROW( 0, ("Failed to connect()") );
		}
	}
	else
	{
		// great !
	}
}

//==================================================================
Connecter::~Connecter()
{
	if ( mSock != INVALID_SOCKET )
		closesocket( mSock );
}

//==================================================================
Connecter::RetVal Connecter::TryConnect()
{
	if ( mIsConnected )
		return RETVAL_CONNECTED;

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
		return RETVAL_ERROR;
	}
	if ( goodSocks > 0 )	// or rather, "1" in this case..
	{
		mIsConnected = true;
		return RETVAL_CONNECTED;
	}
	else
		return RETVAL_WAITING;
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
