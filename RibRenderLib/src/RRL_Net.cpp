//==================================================================
/// RRL_Net.cpp
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RRL_Net.h"
#include "DSystem/include/DNetwork_Connecter.h"
#include "DSystem/include/DUtils_MemFile.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//===============================================================
void ConnectToServers( DVec<Server> &srvList, U32 timeoutMS )
{
	DVec<DNET::Connecter *>	pConnecters;
	
	pConnecters.resize( srvList.size() );

	// fill up the connection objects
	for (size_t i=0; i < srvList.size(); ++i)
	{
		DNET::Connecter *pConn;

		try {
			pConn =	DNEW DNET::Connecter(
								srvList[i].mAddressName.c_str(),
								srvList[i].mPortToCall );

			srvList[i].mIsValid = true;
			pConnecters[i] = pConn;
			printf( "Found %s..\n", srvList[i].mAddressName.c_str() );
		}
		catch ( ... )
		{
			printf( "Error: Could not resolve %s\n", srvList[i].mAddressName.c_str() );
			srvList[i].mIsValid = false;
			pConnecters[i] = NULL;
		}
	}

	DUT::TimeOut	timeOut( timeoutMS );

	// loop until there is some connection waiting
	bool	isSomeConnectionWaiting = false;
	do
	{
		for (size_t i=0; i < pConnecters.size(); ++i)
		{
			DNET::Connecter *pConn = pConnecters[i];

			if ( !pConn || pConn->IsConnected() )
				continue;

			switch ( pConn->IdleConnect() )
			{
			case DNET::Connecter::IDLERET_WAITING:
				isSomeConnectionWaiting = true;
				break;

			case DNET::Connecter::IDLERET_CONNECTED:
				DASSERT( srvList[i].mpPakMan == NULL );
				srvList[i].Init( 
							DNEW DNET::PacketManager( pConn->GetSocket() ) );

				printf( "* Successfully connected to %s..\n", srvList[i].mAddressName.c_str() );
				break;

			case DNET::Connecter::IDLERET_ERROR:
				printf( "! Failed to connect to %s..\n", srvList[i].mAddressName.c_str() );
				srvList[i].mIsValid = false;
				break;

			default:
				DASSERT( 0 );
				break;
			}
		}

		if ( isSomeConnectionWaiting )
		{
			DUT::SleepMS( 1 );	// avoid overloading the CPU !
		}

		if ( timeOut.IsExpired() )
			break;

	} while ( isSomeConnectionWaiting );

	// delete the connection objects
	for (size_t i=0; i < pConnecters.size(); ++i)
		DSAFE_DELETE( pConnecters[i] );
}


}

//==================================================================
}
