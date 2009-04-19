//==================================================================
/// RI_LightSource
///
/// Created by Davide Pasca - 2009/3/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RI_Base.h"

//==================================================================
namespace RI
{

//==================================================================
class Transform;

// LightSource "distantlight" 2 "intensity" [1] "lightcolor" [0.5 0.2 0.0] "from" [-1000 500 1000] "to" [0 0 0]
//==================================================================
/// LightSource
//==================================================================
class LightSource
{
public:
	enum Type
	{
		TYPE_DISTANT,
		TYPE_AMBIENT,
	};
public:

	Type	mType;
	u_int	mID;
	float	mIntesity;
	Color	mColor;
	Point3	mLocFromPos;
	Point3	mLocToPos;

	struct  // union would be nice
	{
		struct 
		{
			Vector3	mDirWS;
			Vector3	mDirCS;
		} mDistant;

		struct  
		{
			Point3	mFromPos;
			Point3	mToPos;
		} mCone;

	} mRend;

	LightSource();

	void UpdateRend( const Transform &xform, const Matrix44 &mtxWorldCam );
};


//==================================================================
}
