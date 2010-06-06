//==================================================================
/// RI_LightSource.h
///
/// Created by Davide Pasca - 2009/3/25
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_LIGHTSOURCE_H
#define RI_LIGHTSOURCE_H

#include "RI_Base.h"
#include "RI_SVM_Shader.h"

//==================================================================
namespace RI
{

//==================================================================
class Transform;

// LightSource "distantlight" 2 "intensity" [1] "lightcolor" [0.5 0.2 0.0] "from" [-1000 500 1000] "to" [0 0 0]
//==================================================================
/// LightSource
//==================================================================
class LightSourceT
{
public:
	RCOwn<SVM::ShaderInst>	moShaderInst;

public:
	enum Type
	{
		TYPE_DISTANT,
		TYPE_AMBIENT,
	};
public:

	u_int	mID;
	bool	mIsAmbient;
/*
	Type	mType;
	float	mIntesity;
	Color	mColor;
	Point3	mLocFromPos;
	Point3	mLocToPos;

	struct  // union would be nice
	{
		struct 
		{
			Vec3f	mDirWS;
			Vec3f	mDirCS;
		} mDistant;

		struct  
		{
			Point3	mFromPos;
			Point3	mToPos;
		} mCone;

	} mRend;
*/

	LightSourceT();
	~LightSourceT()
	{
	}

//	void UpdateRend( const Transform &xform, const Matrix44 &mtxWorldCam );
};

//==================================================================
}

#endif
