//==================================================================
/// RibRender.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibRender.h"
#include "DispWindows.h"

//==================================================================
static bool getCmdParams( int argc, char **argv, CmdParams &out_cmdPars )
{
	out_cmdPars.pInFileName	 = argv[1];

	out_cmdPars.baseDir = DUT::GetDirNameFromFPathName( out_cmdPars.pInFileName );

	if NOT( GetServersList( argc, argv, out_cmdPars.servList ) )
	{
		printf( "Error in the server list\n" );
		return false;
	}

	for (int i=2; i < argc; ++i)
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
static void printUsage( int argc, char **argv )
{
	printf( "\n==== RibRender -- (built of - " __DATE__ " - " __TIME__ ") ====\n" );

	printf( "\n%s <rib file> <output JPEG file> [options]\n", argv[0] );

	printf( "\nOptions:\n" );
	printf( "\t-help | --help | -h             -- Show this help\n" );
	printf( "\t-server <address>:<port>        -- Specify an IP and port number for a render server\n" );
	printf( "\t-forcedlongdim <size in pixels> -- Force the largest dimension's rendering size in pixels\n" );

	printf( "\nExamples:\n" );
	printf( "\t%s TestScenes/Airplane.rib airplane.jpg\n", argv[0] );
	printf( "\t%s TestScenes/Airplane.rib airplane.jpg -longdimsize 1024 -server 192.168.1.107 -server 192.168.1.108\n", argv[0] );
}

//==================================================================
static DispWindows	_gsDispWindows;

//==================================================================
static void handleDisplays( const DVec<RI::Options::Display *> &pDisplays )
{
	// for every display
	for (size_t i=0; i < pDisplays.size(); ++i)
	{
		const RI::Options::Display	&disp = *pDisplays[i];

		if ( disp.IsFrameBuff() )
		{
			_gsDispWindows.AddWindow( disp );
		}
		else
		if ( disp.IsFile() )
		{
			DispDriverFile	file( disp.mName.c_str(), disp.mImage );
		}
	}
}

//==================================================================
static int clientMain( int argc, char **argv )
{
	if ( argc < 2 )
	{
		printUsage( argc, argv );
		return -1;
	}

	CmdParams	cmdPars;
	if NOT( getCmdParams( argc, argv, cmdPars ) )
		return -1;

/*
	const char *pOutExt = DUT::GetFileNameExt( cmdPars.pOutFileName );

	if ( 0 != strcasecmp( pOutExt, "jpg" ) &&
		 0 != strcasecmp( pOutExt, "jpeg" ) )
	{
		printf( "Error: output file name must have a 'jpg' or 'jpeg' extension\n" );
		return -1;
	}
*/

	char	defaultResDir[2048];
	char	defaultShadersDir[4096];

	DStr	exePath = DUT::GetDirNameFromFPathName( argv[0] );

	if ( exePath.length() )
		sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
	else
		strcpy_s( defaultResDir, "Resources" );

	sprintf_s( defaultShadersDir, "%s/Shaders", defaultResDir );

	RI::Hider::Params			hiderParams;
	RI::FileManagerDisk			fileManagerDisk;

	RI::Framework::Params	fwParams;
	fwParams.mFallBackFileDisplay		= true;
	fwParams.mFallBackFBuffDisplay		= false;
	fwParams.mpHiderParams				= &hiderParams;
	fwParams.mInFNameForDefaultOutFName	= cmdPars.pInFileName;

	DVec<RI::Options::Display *>	pDisplays;

	if NOT( cmdPars.servList.size() )
	{
		try
		{
			RI::Framework			framework( fwParams );

			RRL::Render::Params	params;
			params.mTrans.mState.mpFramework		= &framework;
			params.mTrans.mState.mpFileManager		= &fileManagerDisk;
			params.mTrans.mState.mBaseDir			= cmdPars.baseDir;
			params.mTrans.mState.mDefaultShadersDir	= defaultShadersDir;
			params.mTrans.mForcedLongDim			= cmdPars.forcedlongdim;
			params.mpFileName						= cmdPars.pInFileName;

			RRL::Render	render( params, pDisplays );
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
		InitServers( cmdPars, defaultShadersDir );

		try
		{
			RRL::NET::RenderBucketsClient	rendBuckets( cmdPars.servList );
	
			fwParams.mpRenderBuckets = &rendBuckets;

			RI::Framework			framework( fwParams );

			RRL::Render::Params	params;
			params.mTrans.mState.mpFramework		= &framework;
			params.mTrans.mState.mpFileManager		= &fileManagerDisk;
			params.mTrans.mState.mBaseDir			= cmdPars.baseDir;
			params.mTrans.mState.mDefaultShadersDir	= defaultShadersDir;
			params.mTrans.mForcedLongDim			= cmdPars.forcedlongdim;
			params.mpFileName						= cmdPars.pInFileName;

			RRL::Render	render( params, pDisplays );
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

	handleDisplays( pDisplays );

	RRL::FreeDisplays( pDisplays );

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
