//==================================================================
/// RibRenderServer.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderServer.h"

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

	RI::Hider::Params		hiderParams;
	RI::Framework::Params	fwParams;
	fwParams.mpRenderBuckets			= &rendBuckets;
	fwParams.mpHiderParams				= &hiderParams;
	RI::Framework		framework( fwParams );

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
static int serverMain( int argc, char **argv )
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

//==================================================================
static void printUsage( int argc, char **argv )
{
	printf( "\n==== "APPNAME" v"APPVERSION" -- (" __DATE__ " - " __TIME__ ") ====\n" );
	
	printf( "\n%s [options]\n", argv[0] );

	printf( "\nOptions:\n" );
	printf( "    -help | --help | -h  -- Show this help\n" );
	printf( "    -port <port>         -- Wait for connection at port <port>\n" );

	printf( "\nExamples:\n" );
	printf( "    %s\n", argv[0] );
	printf( "    %s -port 31111\n", argv[0] );
}

//==================================================================
int main( int argc, char **argv )
{
#if 0
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF |
					_CRTDBG_DELAY_FREE_MEM_DF |
					_CRTDBG_CHECK_ALWAYS_DF |
					_CRTDBG_LEAK_CHECK_DF|
					_CRTDBG_CHECK_CRT_DF );
#endif

	DNET::InitializeSocket();	// bha !

	// enough params ?
	if ( argc > 3 )
	{
		printUsage( argc, argv );
		return 0;
	}

	// looking for help ?
	for (int i=1; i < argc; ++i)
	{
		if (
			0 == strcasecmp( "-help", argv[i] ) ||
			0 == strcasecmp( "--help", argv[i] ) ||
			0 == strcasecmp( "-h", argv[i] )
			)
		{
			printUsage( argc, argv );
			return 0;
		}
	}

	// which mode ?
	return serverMain( argc, argv );
}
