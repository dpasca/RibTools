//==================================================================
/// RibRender.cpp
///
/// Created by Davide Pasca - 2009/4/12
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "RibToolsBase/include/RibToolsBase.h"
#include "DispWindows.h"
#include "RibRender.h"

//==================================================================
static void printUsage( int argc, char **argv )
{
    printf( "\n==== " APPNAME " v" APPVERSION " -- (" __DATE__ " - " __TIME__ ") ====\n" );

    printf( "\n%s <rib file> [options]\n", argv[0] );

    printf( "\nOptions:\n" );
    printf( "    -help | --help | -h             -- Show this help\n" );
    printf( "    -server <address>:<port>        -- Specify an IP and port number for a render server\n" );
    printf( "    -forcedlongdim <size in pixels> -- Force the largest dimension's rendering size in pixels\n" );
    printf( "    -colorgrids                     -- Show grids in false colors (for debugging)\n" );

    printf( "\nExamples:\n" );
    printf( "    %s TestScenes/Airplane.rib\n", argv[0] );
    printf( "    %s TestScenes/Airplane.rib -server 192.168.1.107 -server 192.168.1.108:30000\n", argv[0] );
    printf( "    %s TestScenes/Airplane.rib -forcedlongdim 1024\n", argv[0] );
}

//==================================================================
static bool getCmdParams( int argc, char **argv, CmdParams &out_cmdPars )
{
    for (int i=1; i < argc; ++i)
    {
        if (0 == strcasecmp( "-help", argv[i] ) ||
            0 == strcasecmp( "--help", argv[i] ) ||
            0 == strcasecmp( "-h", argv[i] ) )
        {
            printUsage( argc, argv );
            exit( 0 );
            return false;	// not needed really
        }
    }

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
        else
        if ( 0 == strcasecmp( "-colorgrids", argv[i] ) )
        {
            out_cmdPars.doColorGrids = true;
        }
    }

    return true;
}

//==================================================================
static DispWindows	_gsDispWindows;

//==================================================================
static void handleDisplays( void *pUserData, const RRL::DisplayList &pDisplays )
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

    _gsDispWindows.Init( argc, argv, APPNAME " v" APPVERSION );

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

    DStr	exePath = RTB::FindRibToolsDir( argv );

    if ( exePath.length() )
        sprintf_s( defaultResDir, "%s/Resources", exePath.c_str() );
    else
        strcpy_s( defaultResDir, "Resources" );

    sprintf_s( defaultShadersDir, "%s/Shaders", defaultResDir );

    RI::Hider::Params			hiderParams;
    DIO::FileManagerDisk		fileManagerDisk;

    RI::Framework::Params fwParams;
    fwParams.mFallBackFileDisplay		= true;
    fwParams.mFallBackFBuffDisplay		= false;
    fwParams.mpHiderParams				= &hiderParams;
    fwParams.mInFNameForDefaultOutFName	= cmdPars.pInFileName;

    RRL::Render::Params	params;
    params.mTrans.mState.mpFileManager		= &fileManagerDisk;
    params.mTrans.mState.mBaseDir			= cmdPars.baseDir;
    params.mTrans.mState.mDefaultShadersDir	= defaultShadersDir;
    params.mTrans.mForcedLongDim			= cmdPars.forcedlongdim;
    params.mpFileName						= cmdPars.pInFileName;
    params.mpOnFrameEndCB					= handleDisplays;

    // setup for color coded grids
    if ( cmdPars.doColorGrids )
    {
        hiderParams.mDbgColorCodedGrids = true;
        params.mTrans.mState.mForcedSurfaceShader = "constant";
    }

    try
    {
        if NOT( cmdPars.servList.size() )
        {
            RI::Framework	framework( fwParams );
            params.mTrans.mState.mpFramework = &framework;

            RRL::Render	render( params );
        }
        else
        {
            InitServers( cmdPars, defaultShadersDir );

            RRL::NET::RenderBucketsClient	rendBuckets( cmdPars.servList );

            fwParams.mpRenderBuckets = &rendBuckets;

            RI::Framework	framework( fwParams );
            params.mTrans.mState.mpFramework = &framework;

            RRL::Render	render( params );
        }
    }
    catch ( std::bad_alloc )
    {
        printf( "Out of Memory !!!\n" );
        return -1;
    }
    catch ( RI::Exception &e )
    {
        printf( "%s\nAborting.\n", e.GetMessage_().c_str() );
        return -1;
    }
    catch ( const std::exception &ex )
    {
        printf( "Exception: %s\n", ex.what() );
        return -1;
    }
    catch ( ... )
    {
        printf( "Unknown exception. Aborting.\n" );
        return -1;
    }

    if ( _gsDispWindows.HasWindows() )
    {
        _gsDispWindows.MainLoop();
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

    DUT::InstallFileManagerStd();

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
