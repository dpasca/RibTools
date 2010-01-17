//==================================================================
/// RibRenderFile_ServerMain.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRender.h"

//==================================================================
static int serverTask( SOCKET clientSock )
{
	printf( "Rendering for %i... yeah, right !!\n", clientSock );


	DNET::PacketManager			packetManager( clientSock );

	RRL::NET::MsgRendJob		netRendJob;

	try {
		U32 ids[] = { RRL::NET::MSGID_RENDJOB };
		DNET::Packet *pPack = packetManager.WaitNextPacketMatch( true, ids, _countof(ids) );

		if NOT( pPack )
		{
			printf( "Failed to read the NetRendJob data\n" );
			return -1;
		}

		netRendJob = *(const RRL::NET::MsgRendJob *)&pPack->mDataBuff[0];
		packetManager.DeletePacket( pPack );
	} catch (...)
	{
		printf( "Failed to read the NetRendJob data\n" );
		return -1;
	}

	printf( "Got a nice NetRendJob !\n" );
	printf( "\tFileName  = %s\n", netRendJob.FileName );
	printf( "\tBaseDir   = %s\n", netRendJob.BaseDir );
	printf( "\tDefResDir = %s\n", netRendJob.DefaultResourcesDir );
	printf( "\tForcedLongSize  = %i\n", netRendJob.ForcedLongDim );
	printf( "\tForcedWd  = %i\n", netRendJob.ForcedWd );
	printf( "\tForcedHe  = %i\n", netRendJob.ForcedHe );

	RRL::NET::FileManagerNet		fileManagerNet( packetManager );

	RRL::NET::RenderBucketsServer	rendBuckets( packetManager );

	RI::Hider::Params	hiderParams;
	RI::Framework		framework( NULL, NULL, false, &rendBuckets, hiderParams );

	RRL::Render::Params	params;
	params.mTrans.mState.mpFramework		= &framework;
	params.mTrans.mState.mpFileManager		= &fileManagerNet;
	params.mTrans.mState.mBaseDir			= netRendJob.BaseDir;
	params.mTrans.mState.mDefaultShadersDir	= netRendJob.DefaultResourcesDir;
	params.mTrans.mForcedLongDim			= netRendJob.ForcedLongDim;
	params.mTrans.mForcedWd					= netRendJob.ForcedWd;
	params.mTrans.mForcedHe					= netRendJob.ForcedHe;
	params.mpFileName						= netRendJob.FileName;

	try
	{
		RRL::Render	render( params );
	}
	catch ( ... )
	{
		printf( "Render failed !\n" );
		return -1;
	}

	printf( "Render complete.\n" );

	// send rendOut to fileManager()

	return 0;
}

//==================================================================
int ServerMain( int argc, char **argv )
{
	int	port = 32323;

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-port", argv[i] ) )
		{
			if ( (i+1) >= argc )
			{
				printf( "Missing port value.\n" );
				return -1;
			}

			port = atoi( argv[i+1] );
			if ( port <= 0 && port >= 65536 )
			{
				printf( "Invalid port range.\n" );
				return -1;
			}

			i += 1;
		}
	}

	DNET::Listener	listener( port );

	if NOT( listener.Start() )
	{
		printf( "Failed to open port %i\n", port );
		return -1;
	}
	printf( "Listening on port %i for connections..\n", port );


	while ( true )
	{
		SOCKET	acceptedSock;

		switch ( listener.Idle( acceptedSock, 500 ) )
		{
		case DNET::Listener::IRT_CONNECTED:
			listener.Stop();

			printf( "Accepted socket %i\n", acceptedSock );
			serverTask( acceptedSock );

			closesocket( acceptedSock );

			if NOT( listener.Start() )
			{
				printf( "Failed to open port %i\n", port );
				return -1;
			}
			printf( "Listening on port %i for connections..\n", port );
			break;

		case DNET::Listener::IRT_ERROR:
			printf( "Error while listening !\n" );
			break;
		}
	}

    return 0;
}
