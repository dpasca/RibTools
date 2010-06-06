//==================================================================
/// RRL_Net.h
///
/// Created by Davide Pasca - 2009/8/6
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_NET_H
#define RRL_NET_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"
#include "DSystem/include/DUtils.h"
#include "RRL_Net_Server.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//===============================================================
enum MsgID
{
	MSGID_FILEREQ		 = 333,
	MSGID_FILEREQANS_DATA,
	MSGID_FILEREQANS_FAIL,
	MSGID_FILEEXISTREQ,
	MSGID_FILEEXISTANSYES,
	MSGID_FILEEXISTANSNO,
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
	I32		ForcedLongDim;
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

//==================================================================
inline MsgID GetMsgID( const DNET::Packet *pPacket )
{
	return (MsgID)*((const U32 *)&pPacket->mDataBuff[0]);
}

//==================================================================
void ConnectToServers( DVec<Server> &srvList, U32 timeoutMS );

//==================================================================
}
//==================================================================
}

#include "RRL_Net_FileManager.h"
#include "RRL_Net_FileServer.h"
#include "RRL_Net_RenderBucketsClient.h"
#include "RRL_Net_RenderBucketsServer.h"

#endif
