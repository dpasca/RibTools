//==================================================================
/// RibRender.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibRender.h"

//==================================================================
static bool getServersList( int argc, char **argv, DVec<RRL::NET::Server> &list )
{
	list.clear();

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-server", argv[i] ) )
		{
			if ( (i+1) >= argc )
			{
				printf( "Missing server definition.\n" );
				return false;
			}

			RRL::NET::Server	*pServEntry = list.grow();

			char	*pContext = NULL;
			
			char *pToken = strtok_r( argv[i+1], ":", &pContext );
			if ( pToken )
			{
				pServEntry->mAddressName = pToken;
				if ( pToken = strtok_r( NULL, ":", &pContext ) )
				{
					pServEntry->mPortToCall = atoi( pToken );

					if ( pServEntry->mPortToCall <= 0 && pServEntry->mPortToCall >= 65536 )
					{
						printf( "Invalid port range.\n" );
						return false;
					}
				}
			}
			else
				pServEntry->mAddressName = argv[i+1];

			i += 1;
		}
	}

	return true;
}

//==================================================================
struct CmdParams
{
	const char				*pInFileName;
	const char				*pOutFileName;
	int						forcedlongdim;
	DVec<RRL::NET::Server>	servList;

	DStr					baseDir;

	CmdParams() :
		pInFileName		(NULL),
		pOutFileName	(NULL),
		forcedlongdim	(-1)
	{
	}
};

//==================================================================
static bool getCmdParams( int argc, char **argv, CmdParams &out_cmdPars )
{
	out_cmdPars.pInFileName	 = argv[2];
	out_cmdPars.pOutFileName = argv[3];

	out_cmdPars.baseDir = DUT::GetDirNameFromFPathName( out_cmdPars.pInFileName );

	if NOT( getServersList( argc, argv, out_cmdPars.servList ) )
	{
		printf( "Error in the server list\n" );
		return false;
	}

	for (int i=1; i < argc; ++i)
	{
		if ( 0 == strcasecmp( "-forcedlongdim", argv[i] ) )
		{
			if ( (i+1) >= argc )
			{
				printf( "Missing value for %s.\n", argv[i] );
				return false;
			}

			out_cmdPars.forcedlongdim = atoi( argv[ i + 1 ] );

			if ( out_cmdPars.forcedlongdim <= 0 ||
				 out_cmdPars.forcedlongdim > 16384 )
			{
				printf( "Invalid value for %s.\n", argv[i] );
			}
		}
	}

	return true;
}

//==================================================================
static void initServers( CmdParams &cmdPars, const char *defaultShadersDir )
{
	// prepare the template job header for the servers
	RRL::NET::MsgRendJob	netRendJob;
	strcpy_s( netRendJob.FileName	, cmdPars.pInFileName );
	strcpy_s( netRendJob.BaseDir	, cmdPars.baseDir.c_str() );
	strcpy_s( netRendJob.DefaultResourcesDir, defaultShadersDir );
	netRendJob.ForcedLongDim = cmdPars.forcedlongdim;
	netRendJob.ForcedWd = -1;
	netRendJob.ForcedHe = -1;

	// connect to all servers
	printf( "Connecting to servers...\n" );
	RRL::NET::ConnectToServers( cmdPars.servList, 3 * 1000 );

	// for each server..
	for (size_t i=0; i < cmdPars.servList.size(); ++i)
	{
		// if the connection was successful
		if ( cmdPars.servList[i].IsConnected() )
		{
			// print a nice message and start by sending a job header..
			printf( "Successfully connected to %s:%i\n", cmdPars.servList[i].mAddressName.c_str(), cmdPars.servList[i].mPortToCall );
			cmdPars.servList[i].mpPakMan->Send( &netRendJob, sizeof(netRendJob) );
		}
		else
		{
			printf( "Failed to connect to %s:%i\n", cmdPars.servList[i].mAddressName.c_str(), cmdPars.servList[i].mPortToCall );
		}
	}
}

//==================================================================
static void printUsage( int argc, char **argv )
{
	printf( "Usage:\n" );
	
	printf( "\t%s <rib file> <output JPEG file> [options]\n", argv[0] );

	printf( "\nOptions:\n" );
	printf( "\t-help | --help | -h             -- Show this help\n" );
	printf( "\t-server <address>:<port>        -- Specify an IP and port number for a render server\n" );
	printf( "\t-forcedlongdim <size in pixels> -- Force the largest dimension's rendering size in pixels\n" );

	printf( "\nExamples:\n" );
	printf( "\t%s TestScenes/Airplane.rib airplane.jpg\n", argv[0] );
	printf( "\t%s TestScenes/Airplane.rib airplane.jpg -longdimsize 1024 -server 192.168.1.107 -server 192.168.1.108\n", argv[0] );
}

//==================================================================
static int clientMain( int argc, char **argv )
{
	if ( argc < 4 )
	{
		printUsage( argc, argv );
		return -1;
	}

	CmdParams	cmdPars;
	if NOT( getCmdParams( argc, argv, cmdPars ) )
		return -1;

	const char *pOutExt = DUT::GetFileNameExt( cmdPars.pOutFileName );

	if ( 0 != strcasecmp( pOutExt, "jpg" ) &&
		 0 != strcasecmp( pOutExt, "jpeg" ) )
	{
		printf( "Error: output file name must have a 'jpg' or 'jpeg' extension\n" );
		return -1;
	}

	char	defaultResDir[2048];
	char	defaultShadersDir[4096];

	DStr	exePath = DUT::GetDirNameFromFPathName( argv[0] );

	if ( exePath.length() )
		sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
	else
		strcpy_s( defaultResDir, "Resources" );

	sprintf_s( defaultShadersDir, "%s/Shaders", defaultResDir );

	DispDriverFile		rendOut( cmdPars.pOutFileName );
	RI::Hider::Params	hiderParams;
	RI::FileManagerDisk	fileManagerDisk;

	if NOT( cmdPars.servList.size() )
	{
		try
		{
			RI::Framework::Params	fwParams;
			fwParams.mpDispDriverFile			= &rendOut;
			fwParams.mFallBackToExisitngDriver	= true;
			fwParams.mpHiderParams				= &hiderParams;
			RI::Framework			framework( fwParams );

			RRL::Render::Params	params;
			params.mTrans.mState.mpFramework		= &framework;
			params.mTrans.mState.mpFileManager		= &fileManagerDisk;
			params.mTrans.mState.mBaseDir			= cmdPars.baseDir;
			params.mTrans.mState.mDefaultShadersDir	= defaultShadersDir;
			params.mTrans.mForcedLongDim			= cmdPars.forcedlongdim;
			params.mpFileName						= cmdPars.pInFileName;

			RRL::Render	render( params );
		}
		catch ( std::bad_alloc )
		{
			printf( "Out of Memory !!!\n" );
			return -1;
		}
		catch ( ... )
		{
			return -1;
		}
	}
	else
	{
		initServers( cmdPars, defaultShadersDir );

		try
		{
			RRL::NET::RenderBucketsClient	rendBuckets( cmdPars.servList );
	
			RI::Framework::Params	fwParams;
			fwParams.mpDispDriverFile			= &rendOut;
			fwParams.mFallBackToExisitngDriver	= true;
			fwParams.mpRenderBuckets			= &rendBuckets;
			fwParams.mpHiderParams				= &hiderParams;
			RI::Framework					framework( fwParams );

			RRL::Render::Params	params;
			params.mTrans.mState.mpFramework		= &framework;
			params.mTrans.mState.mpFileManager		= &fileManagerDisk;
			params.mTrans.mState.mBaseDir			= cmdPars.baseDir;
			params.mTrans.mState.mDefaultShadersDir	= defaultShadersDir;
			params.mTrans.mForcedLongDim			= cmdPars.forcedlongdim;
			params.mpFileName						= cmdPars.pInFileName;

			RRL::Render	render( params );
		}
		catch ( std::bad_alloc )
		{
			printf( "Out of Memory !!!\n" );
			return -1;
		}
		catch ( ... )
		{
			return -1;
		}
	}

    return 0;
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
	if ( argc < 2 )
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

	return clientMain( argc, argv );
}
