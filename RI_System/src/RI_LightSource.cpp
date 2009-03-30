//==================================================================
/// RI_LightSource
///
/// Created by Davide Pasca - 2009/3/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include "RI_Transform.h"
#include "RI_LightSource.h"

//==================================================================
namespace RI
{

//==================================================================
LightSource::LightSource()
{
	mIntesity	= 1;
	mColor		= Color( 1 );
	mLocFromPos	= Point3( 0, 0, 1 );
	mLocToPos	= Point3( 0, 0, 0 );
}

//==================================================================
void LightSource::UpdateRend( const Transform &xform )
{
	if ( mType == TYPE_DISTANT )
	{
		mRend.mDistant.mDir =
			(
				MultiplyV3M( mLocToPos,		xform.GetMatrix() ) -
				MultiplyV3M( mLocFromPos,	xform.GetMatrix() )
			).GetNormalized();
	}
}

//==================================================================
}
