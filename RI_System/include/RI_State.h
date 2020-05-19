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
#include "DSystem/include/DIO_FileManager.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Framework.h"
#include "RI_LightSource.h"

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
template <class T> class CopyStack
{
	DVec<T>	mVec;
public:
    CopyStack()          { mVec.resize(1); }
    void push()          { mVec.push_back( top() ); }
    void pop()           { mVec.pop_back(); }
    const T &top() const { return mVec.back(); }
          T &top()       { return mVec.back(); }
    void clear()         { mVec.clear(); }
};

//==================================================================
//
//==================================================================
class State
{
public:
	struct Params
	{
		Framework				*mpFramework		;
		DIO::FileManagerBase	*mpFileManager		;
		DStr					mBaseDir			;
		DStr					mDefaultShadersDir	;
		DStr					mForcedSurfaceShader;

		Params() :
			mpFramework			(NULL),
			mpFileManager		(NULL)
		{
		}
	};

private:
	SymbolList				mGlobalSyms;
    DVec<Mode>	            mModeStack;
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

	DIO::FileManagerBase &GetFileManager()	{	return *mParams.mpFileManager;				}
	const char *GetBaseDir()		const	{	return mParams.mBaseDir.c_str();			}
	const char *GetDefShadersDir() const	{	return mParams.mDefaultShadersDir.c_str();	}

	DStr		FindResFile( const char *pFileName, Options::SearchPathh spathType );

	Options			&GetCurOptions()			{	return mOptionsStack.top();	}
	const Options	&GetCurOptions() const		{	return mOptionsStack.top();	}

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
	void Clipping( float nearr, float farr );
	void DepthOfField( float fStop, float focalLength, float focalDistance );
	void Shutter( float openShutter, float closeShutter );
	// options.display
	void Display( const char *pName, const char *pType, const char *pMode, ParamList &params );
	void PixelSamples( int samplesX, int samplesY );

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
	void Hyperboloid( const Float3 &p1, const Float3 &p2, float thetamax );
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
	void PointsPolygons( ParamList &params );
	void PointsGeneralPolygons( ParamList &params );

	// --- Non RI commands

	void ErrHandler( Error errCode );
	void ErrHandler( Error errCode, const char *pFmt, ... );

	size_t AddLightSource( LightSourceT *pLSource );
	const DVec<LightSourceT *>	&GetLightSources()	{	return mpLightSources;	}

	const Matrix44 &GetCurTransformOpenMtx() const	{	return mTransformOpenStack.top().GetMatrix();	}
	const Matrix44 &GetCurTransformCloseMtx() const	{	return mTransformCloseStack.top().GetMatrix();	}

	const Matrix44 &GetWorldCameraMtx() const		{	return mMtxWorldCamera;	}

	SVM::Shader *GetShader( const char *pShaderName, const char *pAlternateName );

private:
	bool checkPopMode( Mode expectedMode );
	bool verifyOpType( OpType optype );
	bool verifyBasis( RtToken basis, int steps );

	void pushMode( Mode mode )
	{
		mModeStack.push_back( mode );
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