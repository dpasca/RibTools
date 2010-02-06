//==================================================================
/// RSLC_Prepro.h
///
/// Created by Davide Pasca - 2010/2/5
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_H
#define RSLC_PREPRO_H

#include "DSystem/include/DContainers.h"
#include "DSystem/include/DIO_FileManager.h"

//==================================================================
namespace RSLC
{

//==================================================================
class Prepro
{
public:
	class Map
	{
	public:
		DVec<DStr>	mFNames;
		DVec<u_int>	mOrigLine;
		DVec<u_int>	mOrigFileIdx;

		void GetOrigin( u_int globalLine, u_int &out_localLine, DStr &out_fname )
		{
			DASSERT( globalLine < mOrigLine.size() && mOrigLine.size() == mOrigFileIdx.size() );

			out_localLine = mOrigLine[ globalLine ];
			out_fname = mFNames[ mOrigFileIdx[ globalLine ] ];
		}
	};

public:
	Prepro(
		DIO::FileManagerBase	&fmanager,
		const DVecRO<U8>		&inText,
		const char				*pBaseInclude,
		Map						&out_map,
		DVec<U8>				&out_text );
};

//==================================================================
}

#endif
