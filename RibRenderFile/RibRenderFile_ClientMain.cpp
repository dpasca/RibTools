//==================================================================
/// RibRenderFile_ClientMain.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderFile.h"

//==================================================================
static bool getServersList( int argc, char **argv, DVec<RRL::ServerEntry> &list )
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

			RRL::ServerEntry	*pServEntry = list.grow();

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
/// RenderBucketsStd
//==================================================================
class RenderBucketsClient : public RI::RenderBucketsBase
{
	DVec<RRL::ServerEntry>	*mpServList;

public:
	//==================================================================
	RenderBucketsClient( DVec<RRL::ServerEntry> &servList ) :
		mpServList(&servList)
	{
	}

	//==================================================================
	void Render( RI::HiderREYES &hider )
	{
		DUT::QuickProf	prof( __FUNCTION__ );

		const DVec<RI::Bucket *>	&buckets = hider.GetBuckets();

		// TODO -
		// - Loop buckets
		// -- Give a range of buckets to each server
		// -- Get results from servers (move RenderOutput UpdateRange here ?)
		//
		// - Implement the corresponding server side of things..
	
		int	bucketsN = (int)buckets.size();

		// --- dice primitives accumulated in the buckets
		#pragma omp parallel for
		for (int bi=0; bi < bucketsN; ++bi)
		{
			RI::FrameworkREYES::RenderBucket_s( hider, *buckets[ bi ] );
		}
	}
};

//==================================================================
int ClientMain( int argc, char **argv )
{
	if ( argc < 3 )
	{
		PrintUsage( argc, argv );
		return -1;
	}

	const char *pOutExt = DUT::GetFileNameExt( argv[2] );

	if ( 0 != strcasecmp( pOutExt, "jpg" ) &&
		 0 != strcasecmp( pOutExt, "jpeg" ) )
	{
		printf( "Error: output file name must have a 'jpg' or 'jpeg' extension\n" );
		return -1;
	}

	char	defaultResDir[2048];
	char	defaultShadersDir[4096];

	DStr	exePath = DUT::GetDirNameFromFPathName( argv[0] );
	DStr	baseDir = DUT::GetDirNameFromFPathName( argv[1] );

	if ( exePath.length() )
		sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
	else
		strcpy_s( defaultResDir, "Resources" );

	sprintf_s( defaultShadersDir, "%s/Shaders", defaultResDir );

	DVec<RRL::ServerEntry> servList;
	if NOT( getServersList( argc, argv, servList ) )
	{
		printf( "Error in the server srvList\n" );
		return -1;
	}

	RenderOutputFile		rendOut( argv[2] );
	RI::HiderREYES::Params	hiderParams;
	DUT::FileManager		fileManager;

	if NOT( servList.size() )
	{
		RI::FrameworkREYES		framework( &rendOut, NULL, hiderParams );
		RI::Machine				machine( &framework, baseDir.c_str(), defaultShadersDir );

		try
		{
			RRL::Render	render( argv[1], machine, fileManager );
		}
		catch ( ... )
		{
			return -1;
		}
	}
	else
	{
		// prepare the template job header for the servers
		RI::NetRendJob	netRendJob;
		memset( &netRendJob, 0, sizeof(netRendJob) );
		strcpy_s( netRendJob.FileName	, argv[1] );
		strcpy_s( netRendJob.BaseDir	, baseDir.c_str() );
		strcpy_s( netRendJob.DefaultResourcesDir, defaultShadersDir );
		netRendJob.ForcedWd = -1;
		netRendJob.ForcedHe = -1;

		// connect to all servers
		printf( "Connecting to servers...\n" );
		RRL::ConnectToServers( servList, 3 * 1000 );

		// for each server..
		for (size_t i=0; i < servList.size(); ++i)
		{
			// if the connection was successful
			if ( servList[i].IsConnected() )
			{
				// print a nice message and start by sending a job header..
				printf( "Successfully connected to %s:%i\n", servList[i].mAddressName.c_str(), servList[i].mPortToCall );
				servList[i].mpFilemanager->SendData( &netRendJob, sizeof(netRendJob) );
			}
			else
			{
				printf( "Failed to connect to %s:%i\n", servList[i].mAddressName.c_str(), servList[i].mPortToCall );
			}
		}

		RenderBucketsClient		rendBuckets( servList );

		RI::FrameworkREYES		framework( &rendOut, &rendBuckets, hiderParams );
		RI::Machine				machine( &framework, baseDir.c_str(), defaultShadersDir );

		try
		{
			RRL::Render	render( argv[1], machine, fileManager );
		}
		catch ( ... )
		{
			return -1;
		}
	}

    return 0;
}
