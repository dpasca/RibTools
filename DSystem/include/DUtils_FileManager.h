//==================================================================
/// DUtils_FileManager.h
///
/// Created by Davide Pasca - 2009/8/2
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DUTILS_FILEMANAGER_H
#define DUTILS_FILEMANAGER_H

#include "DNetwork.h"
#include "DNetwork_PacketManager.h"
#include "DUtils_MemFile.h"

//==================================================================
namespace DUT
{

//==================================================================
class FileManager
{
	SOCKET				mSock;
	bool				mIsConnectionBroken;
	bool				mIsServing;
	DNET::PacketManager	*mpPkMan;

public:
	FileManager();
	FileManager( SOCKET sock, bool serving );
	~FileManager();

	void SendData( const void *pData, size_t dataSize );

	bool GetFile( MemFile &memfile, const char *pFileName );
	bool GetData( void *pDestData, size_t destSize );

	bool IsBroken() const { return mIsConnectionBroken; }

private:
};

//==================================================================
}

#endif
