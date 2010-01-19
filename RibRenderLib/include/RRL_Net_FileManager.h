//==================================================================
/// RRL_Net_FileManager.h
///
/// Created by Davide Pasca - 2009/8/9
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RRL_NET_FILEMANAGER_H
#define RRL_NET_FILEMANAGER_H

#include "RI_System/include/RI_FileManager.h"
#include "DSystem/include/DNetwork_PacketManager.h"

//==================================================================
namespace RRL
{
//==================================================================
namespace NET
{

//==================================================================
/// FileManagerNet
//==================================================================
class FileManagerNet : public RI::FileManagerBase
{
	friend class Server;

	DUT::LongCriticalSection	mLongCS;

	DNET::PacketManager			*mpPakMan;

public:
	FileManagerNet( DNET::PacketManager &packManager );

		void GrabFile( const char *pFileName, DVec<U8> &out_vec );
		bool FileExists( const char *pFileName );
};

//==================================================================
}
//==================================================================
}

#endif
