//==================================================================
/// RI_Framework.h
///
/// Created by Davide Pasca - 2009/02/08
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_FRAMEWORK_H
#define RI_FRAMEWORK_H

#include "RI_Base.h"
#include "RI_Options.h"
#include "RI_HiderST.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;

//==================================================================
class RenderBucketsBase
{
public:
    virtual ~RenderBucketsBase() {}

    virtual void Render( Hider &hider ) = 0;
/*
private:
    RenderBucketsBase( const RenderBucketsBase &from ) {}
    void operator =( const RenderBucketsBase &from ) {}
*/
};

//==================================================================
/// Framework
//==================================================================
class Framework
{
#if defined(DEBUG) || defined(_DEBUG)
    DVec<DStr>	mDbg_SrcFileNames;
#endif

public:
    class Params
    {
    public:
        bool					mFallBackFileDisplay;
        bool					mFallBackFBuffDisplay;
        RenderBucketsBase		*mpRenderBuckets;
        const Hider::Params		*mpHiderParams;
        std::string				mInFNameForDefaultOutFName;

        Params() :
            mFallBackFileDisplay(false),
            mFallBackFBuffDisplay(false),
            mpRenderBuckets(NULL),
            mpHiderParams(NULL)
        {
        }
    };

    Params				mParams;

    const SymbolList	*mpGlobalSyms;
    Hider				mHider;

private:
    Options				mOptions;	//temporary, will remove

private:
    DVec<Attributes*>	mpUniqueAttribs;
    DVec<Transform*>	mpUniqueTransform;

    RevisionChecker		mAttrsRev;
    RevisionChecker		mTransRev;

public:
    Framework( const Params &params );

    void SetGlobalSyms( const SymbolList *pGlobalSyms )
    {
        mpGlobalSyms = pGlobalSyms;

        mHider.mpGlobalSyms = pGlobalSyms;
    }

    void WorldBegin(
                const Options &opt,
                const Matrix44 &mtxWorldCamera );

    void Insert(	PrimitiveBase		*pPrim,
                    const Attributes	&attr,
                    const Transform		&xform );

#if defined(DEBUG) || defined(_DEBUG)
    void Dbg_MarkLastPrim( const char *pSrcFileName, int srcLine );
#endif

    void WorldEnd();

    static void RenderBucket_s( Hider &hider, HiderBucket &bucket );

private:
    void	worldEnd_simplify();
    void	worldEnd_splitAndAddToBuckets();
    void	worldEnd_setupDisplays();
};

//==================================================================
}

#endif