//==================================================================
/// RibRenderLib.cpp
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RibRenderLib.h"
#include "DSystem/include/DNetwork_Connecter.h"
#include "RI_System/include/RI_Parser.h"

//==================================================================
namespace RRL
{

//==================================================================
class TimeOut
{
	I64	mStartTicks;
	U32	mTimeOutMS;

public:
	TimeOut( U32 timeoutMS ) :
		mTimeOutMS(timeoutMS),
		mStartTicks(DUT::GetTimeTicks())
	{
	}

	bool IsExpired() const
	{
		I64 delta = DUT::GetTimeTicks() - mStartTicks;

		return DUT::TimeTicksToMS( delta ) > mTimeOutMS;
	}
};

//===============================================================
void ConnectToServers( DVec<ServerEntry> &srvList, U32 timeoutMS )
{
	DVec<DNET::Connecter *>	pConnecters;
	
	pConnecters.resize( srvList.size() );

	// fill up the connection objects
	for (size_t i=0; i < srvList.size(); ++i)
	{
		DNET::Connecter *pConn;

		try {
			pConn =	DNEW DNET::Connecter(
								srvList[i].mAddressName.c_str(),
								srvList[i].mPortToCall );

			srvList[i].mIsValid = true;
			pConnecters[i] = pConn;
		}
		catch ( ... )
		{
			srvList[i].mIsValid = false;
			pConnecters[i] = NULL;
		}
	}

	TimeOut	timeOut( timeoutMS );

	// loop until there is some connection waiting
	bool	isSomeConnectionWaiting = false;
	do
	{
		for (size_t i=0; i < pConnecters.size(); ++i)
		{
			DNET::Connecter *pConn = pConnecters[i];

			if ( pConn->IsConnected() )
				continue;

			switch ( pConn->TryConnect() )
			{
			case DNET::Connecter::RETVAL_WAITING:
				isSomeConnectionWaiting = true;
				break;

			case DNET::Connecter::RETVAL_CONNECTED:
				srvList[i].mSock = pConn->GetSocket();
				srvList[i].mpFilemanager = DNEW DUT::FileManager( srvList[i].mSock, false );
				break;

			case DNET::Connecter::RETVAL_ERROR:
				srvList[i].mIsValid = false;
				break;

			default:
				DASSERT( 0 );
				break;
			}
		}

		if ( isSomeConnectionWaiting )
		{
		#if defined WIN32
			Sleep( 1 );	// avoid overloading the CPU !
		#endif
		}

		if ( timeOut.IsExpired() )
			break;

	} while ( isSomeConnectionWaiting );

	// delete the connection objects
	for (size_t i=0; i < pConnecters.size(); ++i)
		DSAFE_DELETE( pConnecters[i] );
}

//==================================================================
/// Render
//==================================================================
Render::Render( const char			*pFileName,
			    RI::Machine			&machine,
				DUT::FileManager	&fileManager,
				bool				verbose )
{
	DUT::MemFile	file;

	if NOT( fileManager.GetFile( file, pFileName ) )
	{
		DASSTHROW( 0, ( "Could not open the file in input. Quitting !\n" ) );
	}

	RI::Parser	parser;

	for (size_t i=0; i <= file.GetDataSize(); ++i)
	{
		if ( i == file.GetDataSize() )
			parser.AddChar( 0 );
		else
			parser.AddChar( (char)file.GetData()[i] );

		while ( parser.HasNewCommand() )
		{
			DStr			cmdName;
			RI::ParamList	cmdParams;
			int				cmdLine;

			parser.FlushNewCommand( &cmdName, &cmdParams, &cmdLine );

			if ( verbose )
			{
				printf( "CMD %s ", cmdName.c_str() );

				if ( cmdParams.size() )
					printf( "(%i params)", cmdParams.size() );

				puts( "" );
			}


			try {
				machine.AddCommand( cmdName, cmdParams );
			} catch ( std::runtime_error ex )
			{
				printf( "ERROR at line: %i\n", cmdLine );
				break;
			}
		}
	}
}

/*
//==================================================================
static void renderBucketsClient( DVec<ServerEntry> &srvList )
{
	size_t	buckIdx = 0;
	size_t	bucketsN;

	while ( buckIdx < bucketsN )
	{
		size_t buckIdx2 = buckIdx + 4;

		buckIdx2 = DMIN( buckIdx2, bucketsN );

		size_t	spanN = buckIdx2 - buckIdx;

		for (size_t si=0; si < srvList.size(); ++si)
		{
			if NOT( srvList[si].mIsWorking )
			{
				// send job: buckets from buckIdx to buckIdx + spanN
				buckIdx = buckIdx2;
				break;
			}
		}
	
		for (size_t si=0; si < srvList.size(); ++si)
		{
			if ( srvList[si].mIsWorking )
			{
				srvList[si]->mpFilemanager->GetData();

				// send job: buckets from buckIdx to buckIdx + spanN
				buckIdx = buckIdx2;
				break;
			}
		}
	}
	
}
*/

//==================================================================
}
