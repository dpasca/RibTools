//==================================================================
/// RibRenderLib.h
///
/// Created by Davide Pasca - 2009/8/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RIBRENDERLIB_H
#define RIBRENDERLIB_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DNetwork.h"

#include "RI_System/include/RI_Render.h"
#include "RI_System/include/RI_Net_Protocol.h"
#include "RI_System/include/RI_Framework.h"

#include "RRL_Translator.h"
#include "RRL_Net.h"

#include "DispDrivers/include/DispDriverFile.h"
#include "DispDrivers/include/DispDriverFBuffOGL.h"

//==================================================================
namespace RRL
{

typedef DVec<RI::Options::Display *>	DisplayList;

//==================================================================
/// Render
//==================================================================
class Render
{
public:
	class Params
	{
	public:
		const char			*mpFileName;
		bool				mVerbose;
		Translator::Params	mTrans;

		Params() :
			mpFileName(NULL),
			mVerbose(false)
		{
		}
	};

public:
	Render( Params &params, DisplayList &out_pDisplays );
};

//==================================================================
void FreeDisplays( DisplayList &pDisplays );

//==================================================================
}

#endif
