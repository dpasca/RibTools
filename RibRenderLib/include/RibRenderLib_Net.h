//==================================================================
/// RibRenderLib_Net.h
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_NET_H
#define RIBRENDERLIB_NET_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DUtils.h"
#include "DSystem/include/DUtils_FileManager.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//===============================================================
enum MsgID
{
	MSGID_FILEREQ,
	MSGID_RENDJOB,
	MSGID_RENDBUCKETS,
	MSGID_RENDDONE,
	MSGID_BUCKETDATA,
	MSGID_N
};

//===============================================================
/// Protocol stuff
//==================================================================
struct MsgRendJob
{
	U32		MsgID;
	char	FileName[512];
	char	BaseDir[512];
	char	DefaultResourcesDir[512];
	I32		ForcedWd;
	I32		ForcedHe;

	MsgRendJob()
	{
		memset( this, 0, sizeof(*this) );
		MsgID = MSGID_RENDJOB;
	}
};

//===============================================================
struct MsgRendBuckes
{
	U32		MsgID;
	U32		BucketStart;
	U32		BucketEnd;

	MsgRendBuckes()
	{
		memset( this, 0, sizeof(*this) );
		MsgID = MSGID_RENDBUCKETS;
	}
};

//===============================================================
struct MsgRendDone
{
	U32		MsgID;

	MsgRendDone()
	{
		MsgID = MSGID_RENDDONE;
	}
};

//===============================================================
struct MsgBucketData
{
	U32		MsgID;
	U32		BucketIdx;

	MsgBucketData()
	{
		memset( this, 0, sizeof(*this) );
		MsgID = MSGID_BUCKETDATA;
	}
};

//===============================================================
/// Server
//==================================================================
class Server
{
public:
	DStr				mAddressName;
	int					mPortToCall;
	DUT::FileManager	*mpFilemanager;
	bool				mIsValid;
	bool				mIsBusy;

	Server() :
		mPortToCall(32323),
		mpFilemanager(NULL),
		mIsValid(true),
		mIsBusy(false)
	{
	}

	~Server()
	{
		DSAFE_DELETE( mpFilemanager );
	}

	bool IsConnected() const
	{
		return mpFilemanager && mpFilemanager->mpPkMan->IsConnected();
	}
};

//==================================================================
void ConnectToServers( DVec<Server> &srvList, U32 timeoutMS );

//==================================================================
}
//==================================================================
}

#include "RibRenderLib_Net_RenderBucketsClient.h"
#include "RibRenderLib_Net_RenderBucketsServer.h"

#endif
