//==================================================================
/// DIO_FileManager.h
///
/// Created by Davide Pasca - 2009/8/8
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DIO_FILEMANAGER_H
#define DIO_FILEMANAGER_H

#include "DSystem/include/DUtils_MemFile.h"
#include "DSystem/include/DUtils_Files.h"

//==================================================================
namespace DIO
{

//==================================================================
class FileManagerBase
{
public:
	//FileManagerBase() {}
	virtual ~FileManagerBase() {}

	virtual void GrabFile( const char *pFileName, DVec<U8> &out_vec ) = 0;
	virtual bool FileExists( const char *pFileName ) = 0;

	void GrabFile( const char *pFileName, DUT::MemFile &mf )
	{
		DVec<U8>	vec;
		GrabFile( pFileName, vec );
		mf.InitExclusiveOwenership( vec );
	}
};

//==================================================================
class FileManagerDisk : public FileManagerBase
{
public:
		~FileManagerDisk() {}

		void GrabFile( const char *pFileName, DVec<U8> &out_vec )
		{
			if NOT( DUT::GrabFile( pFileName, out_vec ) )
				DEX_RUNTIME_ERROR( "Could not grab the file '%s'", pFileName );
		}

		bool FileExists( const char *pFileName )
		{
			return DUT::FileExists( pFileName );
		}
};

//==================================================================
}

#endif
