//==================================================================
/// RI_State.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_STATE_H
#define RI_STATE_H

#include "RI_Base.h"
#include "DSystem/include/DContainers.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_FrameworkREYES.h"
#include "RI_Primitive.h"
#include "RI_LightSource.h"
#include "RI_FileManager.h"

//==================================================================
namespace RI
{

//==================================================================
/*
Options
*	WorldBegin/End
*	FrameBegin/End
	
Attributes
*	AttributeBegin/End
*	WorldBegin/End
*	FrameBegin/End
*	SolidBegin/End
*	ObjectBegin/End

Transform
*	TransformBegin/End
*	AttributeBegin/End
*	WorldBegin/End
*	FrameBegin/End
*	SolidBegin/End
*	ObjectBegin/End
*/

//==================================================================
//
//==================================================================
class State
{
public:
	struct Params
	{
		FrameworkREYES	*mpFramework		;
		FileManagerBase	*mpFileManager		;
		DStr			mBaseDir			;
		DStr			mDefaultShadersDir	;
		DStr			mForcedSurfaceShader;

		Params() :
			mpFramework			(NULL),
			mpFileManager		(NULL)
		{
		}
	};

private:
	SymbolList				mGlobalSyms;
	Stack<Mode>				mModeStack;
	CopyStack<Options	>	mOptionsStack;
	CopyStack<Attributes>	mAttributesStack;
	CopyStack<Transform	>	mTransformOpenStack;
	CopyStack<Transform	>	mTransformCloseStack;
	
	RevisionTracker			mOptionsRevTrack;
	RevisionTracker			mAttribsRevTrack;
	RevisionTracker			mTransOpenRevTrack;
	RevisionTracker			mTransCloseRevTrack;

	DVec<LightSourceT *>	mpLightSources;

	Matrix44				mMtxWorldCamera;

public:
	Params					mParams;
private:
	ResourceManager			mResManager;

	enum OpType
	{
		OPTYPE_OPTS,
		OPTYPE_ATRB,
		OPTYPE_PRIM,
		OPTYPE_STD_XFORM,	// excludes CoordinateSystem and TransformPoints
	};

public:
	State( const Params &params );
	~State();

	FileManagerBase &GetFileManager()		{	return *mParams.mpFileManager;				}
	const char *GetBaseDir()		const	{	return mParams.mBaseDir.c_str();			}
	const char *GetDefShadersDir() const	{	return mParams.mDefaultShadersDir.c_str();	}

	void	Begin( RtToken name );
	void	End();

	void	FrameBegin( int frame );
	void	FrameEnd();
	void	WorldBegin();
	void	WorldEnd();
	void	AttributeBegin();
	void	AttributeEnd();
	void	TransformBegin();
	void	TransformEnd();
	void	SolidBegin( RtToken operation );
	void	SolidEnd();
	ObjectHandle ObjectBegin();
	void	ObjectEnd();
	void	MotionBegin( int n, const float times[] );
	void	MotionEnd();

	// attributes
	void DoBound( const Bound &bound );

	void Detail( const Bound &detail );

	void DetailRange(float	minVisible,
					 float	lowerTransition,
					 float	upperTransition,
					 float	maxVisible );
					 
	void GeometricApproximation(RtToken typeApproximation,
								   float valueApproximation );

	void Orientation( RtToken orientation );
	void Sides( int sides );
	void Basis( RtToken ubasis, const float *pCustomUBasis, int ustep,
				RtToken vbasis, const float *pCustomVBasis, int vstep );
				
	void ColorSet( const Color &col );
	void Opacity( const Color &col );
	void AreaLightSource( ParamList &params );
	void LightSource( ParamList &params );
	void Declare( ParamList &params );
	void Surface( ParamList &params );
	void Displacement( ParamList &params );

	// options
	void Format( int xRes, int yRes, float pixelRatio );
	void FrameAspectRatio( float ratio );
	void ScreenWindow( float left, float right, float bottom, float top );
	void CropWindow( float xMin, float xMax, float yMin, float yMax );
	void Projection( ParamList &params );
	void Clipping( float near, float farr );
	void DepthOfField( float fStop, float focalLength, float focalDistance );
	void Shutter( float openShutter, float closeShutter );

	// transforms
	void Identity();
	void ConcatTransform( const Matrix44 &mtxLeft );
	void TransformCmd( const float *pMtx );
	void Scale( float sx, float sy, float sz );
	void Rotate( float angDeg, float ax, float ay, float az );
	void Translate( float tx, float ty, float tz );

	// primitives
	void Cylinder( float radius, float zmin, float zmax, float thetamax );
	void Cone( float height, float radius, float thetamax );
	void Sphere( float radius, float zmin, float zmax, float thetamax );
	void Hyperboloid( const Vec3f &p1, const Vec3f &p2, float thetamax );
	void Paraboloid( float rmax, float zmin, float zmax, float thetamax );
	void Torus( float maxRadius, float minRadius,
			   float phimin, float phimax,
			   float thetamax );

	void Patch( RtToken type, ParamList &params );
	void PatchMesh( RtToken type, ParamList &params );
	void NuPatch(
			int		nu		,
			int		uorder	,
			const float	*pUknot	,
			float	umin	,
			float	umax	,
			int		nv		,
			int		vorder	,
			const float	*pVknot	,
			float	vmin	,
			float	vmax	,
			ParamList &params
		);

	void Polygon( ParamList &params );
	void PointsGeneralPolygons( ParamList &params );

	void ErrHandler( Error errCode );
	void ErrHandler( Error errCode, const char *pFmt, ... );

	void WarnPrintf( const char *pFmt, ... );
	void EXCEPTPrintf( const char *pFmt, ... );

	size_t AddLightSource( LightSourceT *pLSource );
	const DVec<LightSourceT *>	&GetLightSources()	{	return mpLightSources;	}

	const Matrix44 &GetCurTransformOpenMtx() const	{	return mTransformOpenStack.top().GetMatrix();	}
	const Matrix44 &GetCurTransformCloseMtx() const	{	return mTransformCloseStack.top().GetMatrix();	}

	const Matrix44 &GetWorldCameraMtx() const		{	return mMtxWorldCamera;	}

private:
	bool checkPopMode( Mode expectedMode );
	bool verifyOpType( OpType optype );
	bool verifyBasis( RtToken basis, int steps );

	void pushMode( Mode mode )
	{
		mModeStack.push( mode );
	}

	void popMode( Mode expectedMode )
	{
		checkPopMode( expectedMode );
	}

	enum StackFlag
	{
		SF_OPTS = 1,
		SF_ATRB = 2,
		SF_TRAN = 4
	};

	inline void pushStacks( const u_int flags );
	inline void popStacks( const u_int flags );
	
	inline void insertPrimitive( PrimitiveBase *pPrim );
	
	void makeDefaultShaders( const char *pBasePath );
	void addDefShader( const char *pBasePath, const char *pSName );
};

}


#endif