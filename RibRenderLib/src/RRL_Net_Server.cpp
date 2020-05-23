//==================================================================
/// RRL_Net_Server.cpp
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RRL_Net_FileServer.h"
#include "RRL_Net_FileManager.h"
#include "RRL_Net_Server.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
Server::Server() :
    mPortToCall(32323),
    mpPakMan(NULL),
    mpFilemanager(NULL),
    mpFileServer(NULL),
    mIsValid(true),
    mBusyCnt(0)
{

}

//==================================================================
Server::~Server()
{
    DSAFE_DELETE( mpFilemanager );
    DSAFE_DELETE( mpFileServer );
    DSAFE_DELETE( mpPakMan );
}

//==================================================================
void Server::Init( DNET::PacketManager *pPakMan )
{
    mpPakMan = pPakMan;

    mpFilemanager = DNEW FileManagerNet( *mpPakMan );
    mpFileServer = DNEW FileServer( mpPakMan );
}

//==================================================================
bool Server::IsConnected() const
{
    return mpFilemanager && mpFilemanager->mpPakMan->IsConnected();
}

//==================================================================
}
//==================================================================
}
