//==================================================================
/// RI_Attributes.h
///
/// Created by Davide Pasca - 2008/12/17
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_ATTRIBUTES_H
#define RI_ATTRIBUTES_H

#include "RI_Base.h"
#include "RI_Param.h"
#include "RI_Tokens.h"
#include "RI_Symbol.h"
#include "RI_SVM_Shader.h"
#include "RI_LightSource.h"

//==================================================================
namespace RI
{

//==================================================================
class State;
class Texture;

//==================================================================
/// Attributes
//==================================================================
class Attributes
{
public:
    State				*mpState;

private:
    const SymbolList	*mpGlobalSyms;
    ResourceManager		*mpResManager;

public:
    RevisionTracker		*mpRevision;

public:
    //==================================================================
    Bound				mBound;				// DoBound()
    Bound				mDetail;			// Detail()
                
    float				mMinVisible;		// DetailRange()
    float				mLowerTransition;	//
    float				mUpperTransition;	//
    float				mMaxVisible;		//
                    
    const Symbol		*mpTypeApproximation;	// GeometricApproximation()
    float				mValueApproximation;

    bool				mOrientationFlipped;// Orientation()
    int					mSides;				// Sides()

    const Symbol		*mpUBasis;			// Basis()
    const Symbol		*mpVBasis;
    const RtBasis		*mpCustomUBasis;
    const RtBasis		*mpCustomVBasis;

    int					mUSteps;
    int					mVSteps;

    Color				mColor;
    Color				mOpacity;
    RCSha<SVM::ShaderInst>	moSurfaceSHI;
    RCSha<SVM::ShaderInst>	moDisplaceSHI;

    DVec<U16>			mActiveLights;

public:
    Attributes();
    Attributes( const Attributes &attributes );

    Attributes& operator=(const Attributes& rhs);

    void Init(
            State			*pState,
            SymbolList		*pGlobalSyms,
            ResourceManager	*pResManager,
            RevisionTracker	*pRevision );

    ~Attributes();

    // for conveninece
    const SymbolList &GetGlobalSymList() const
    {
        return *mpGlobalSyms;
    }

    // get a light source given the index in the active lights list
    const LightSourceT *GetLight( size_t actLightIdx ) const;

private:
    void copyFrom(const Attributes& rhs);

public:
    void cmdBound( const Bound &bound );
    void cmdDetail( const Bound &detail );
    void cmdDetailRange(float	minVisible,
                        float	lowerTransition,
                        float	upperTransition,
                        float	maxVisible );
    void cmdGeometricApproximation(RtToken typeApproximation,
                                   float valueApproximation );
    
    void cmdOrientation( RtToken orientation );
    void cmdSides( int sides );
    void cmdBasis(RtToken ubasis, const float *pCustomUBasis, int ustep,
                  RtToken vbasis, const float *pCustomVBasis, int vstep );
                  
    const RtBasis &GetUBasis() const { return mpCustomUBasis ? *mpCustomUBasis : *(const RtBasis *)mpUBasis->GetConstantData(); }
    const RtBasis &GetVBasis() const { return mpCustomVBasis ? *mpCustomVBasis : *(const RtBasis *)mpVBasis->GetConstantData(); }

    void cmdColor( const Color &color );
    void cmdOpacity( const Color &color );
    void cmdLightSource( ParamList &params );
    void cmdSurface( ParamList &params );
    void cmdDisplacement( ParamList &params );

    Texture *GetTexture( const char *pTextureName ) const;

private:
    void getShaderParams( ParamList &params, size_t fromIdx, SVM::ShaderInst &shaderInst, const Matrix44 &mtxLocalCam );
};

//==================================================================
}


#endif