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
LightSourceT::LightSourceT()
{
	mIntesity	= 1;
	mColor		= Color( 1 );
	mLocFromPos	= Point3( 0, 0, 0 );
	mLocToPos	= Point3( 0, 0, 1 );
}

//==================================================================
void LightSourceT::UpdateRend( const Transform &xform, const Matrix44 &mtxWorldCam )
{
	if ( mType == TYPE_DISTANT )
	{
#if 0
		Vector3	diff =
					V3__V3W1_Mul_M44( mLocToPos,	xform.GetMatrix() ) -
					V3__V3W1_Mul_M44( mLocFromPos,	xform.GetMatrix() );

		mRend.mDistant.mDirWS = diff.GetNormalized();
#else

		const Matrix44 &mtxLocalWorld = xform.GetMatrix();

		Vec3f	diffLS = mLocToPos - mLocFromPos;
		Vec3f	diffWS = V3__V3W1_Mul_M44( diffLS, mtxLocalWorld );
		Vec3f	diffCS = V3__V3W1_Mul_M44( diffWS, mtxWorldCam );

		mRend.mDistant.mDirWS = -diffWS.GetNormalized();
		mRend.mDistant.mDirCS = -diffCS.GetNormalized();
#endif
	}
}

//==================================================================
}
