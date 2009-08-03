//==================================================================
/// RibRenderLib.h
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_H
#define RIBRENDERLIB_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DUtils_FileManager.h"
#include "DSystem/include/DNetwork.h"

#include "RI_System/include/RI_Render.h"
#include "RI_System/include/RI_Net_Protocol.h"
#include "RI_System/include/RI_Machine.h"
#include "RI_System/include/RI_FrameworkREYES.h"

//==================================================================
namespace RRL
{

//===============================================================
class ServerEntry
{
public:
	DStr				mAddressName;
	int					mPortToCall;
	SOCKET				mSock;
	DUT::FileManager	*mpFilemanager;
	bool				mIsValid;
	bool				mIsWorking;

	ServerEntry() :
		mPortToCall(32323),
		mSock(INVALID_SOCKET),
		mpFilemanager(NULL),
		mIsValid(true),
		mIsWorking(false)
	{
	}

	~ServerEntry()
	{
		DSAFE_DELETE( mpFilemanager );
	}

	bool IsConnected() const
	{
		return mSock != INVALID_SOCKET && mIsValid;
	}
};

//==================================================================
void ConnectToServers( DVec<ServerEntry> &srvList, U32 timeoutMS );

//==================================================================
/// Render
//==================================================================
class Render
{

public:
	Render(
		const char			*pFileName,
		RI::Machine			&machine,
		DUT::FileManager	&fileManager,
		bool				verbose=false );
};

//==================================================================
}

#endif
