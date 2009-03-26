//==================================================================
/// RI_LightSource
///
/// Created by Davide Pasca - 2009/3/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_LightSource.h"

//==================================================================
namespace RI
{

//==================================================================
LightSource::LightSource()
{
	mIntesity	= 1;
	mColor		= Color( 1 );
	mFromPos	= Point3( 0, 0, 1 );
	mToPos		= Point3( 0, 0, 0 );
}

//==================================================================
}
