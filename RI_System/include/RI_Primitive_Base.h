//==================================================================
/// RI_Primitive_Base.h
///
/// Created by Davide Pasca - 2009/4/30
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_PRIMITIVE_BASE_H
#define RI_PRIMITIVE_BASE_H

#include "DMath/include/DMath.h"
#include "RI_Param.h"
#include "RI_Symbol.h"
#include "RI_MicroPolygonGrid.h"
#include "RI_Primitive_Utils.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;
class Hider;
class WorkGrid;

//==================================================================
/// PrimitiveBase
//==================================================================
class PrimitiveBase
{
    RefCount	mRefCnt;

#if defined(DEBUG) || defined(_DEBUG)
public:
    const char	*mpDbg_SrcArchive;
    int			mDbg_SrcLine;
private:
#endif

public:
    enum Type
    {
        CONE,
        CYLINDER,
        SPHERE,
        HYPERBOLOID,
        PARABOLOID,
        TORUS,

        PATCHBILINEAR,
        PATCHBICUBIC,
        PATCHMESH,

        NUPATCH,

        POLYGON,

        POINTSGENERALPOLYGONS,
    };

    Type				mType;
    Attributes			*mpAttribs;
    Transform			*mpTransform;

public:
/*
    // a copy constructor 
    PrimitiveBase( const PrimitiveBase &from ) :
        mRefCnt(0),
        mType(from.mType),
        mpAttribs(from.mpAttribs),
        mpTransform(from.mpTransform)
    {
    }
*/

    PrimitiveBase( Type type ) :
#if defined(DEBUG) || defined(_DEBUG)
        mpDbg_SrcArchive(NULL),
        mDbg_SrcLine(-1),
#endif
        mType(type),
        mpAttribs(NULL),
        mpTransform(NULL)
    {
    }

    virtual ~PrimitiveBase()
    {
    }

    PrimitiveBase *Borrow()
    {
        mRefCnt.AddRef();
        return this;
    }

    void Release()
    {
        if ( 0 == mRefCnt.SubRef() )
            DDELETE( this );
    }
    
    virtual bool	IsComplex() const = 0;
    virtual bool	IsSplitable() const = 0;

    void SetStates(
        Attributes	*pAttribs,
        Transform	*pTransform
    )
    {
        mpAttribs	= pAttribs	;
        mpTransform	= pTransform;
    }

    void CopyStates( const PrimitiveBase &fromPrim )
    {
        mpAttribs	= fromPrim.mpAttribs	;
        mpTransform	= fromPrim.mpTransform;
    }
    
    bool IsUsed() const
    {
        return mRefCnt.GetCount() != 0;
    }
};

//==================================================================
/// ComplexPrimitiveBase
//==================================================================
class ComplexPrimitiveBase : public PrimitiveBase
{

public:
    ComplexPrimitiveBase( PrimitiveBase::Type type ) :
        PrimitiveBase(type)
    {
    }

    virtual ~ComplexPrimitiveBase() {}

        bool	IsComplex() const		{ return true;	}
        bool	IsSplitable() const		{ return false;	}

    virtual void	Simplify( Hider &hider ) = 0;
};

//==================================================================
/// SimplePrimitiveBase
//==================================================================
class SimplePrimitiveBase : public PrimitiveBase
{
public:
    float	mURange[2];
    float	mVRange[2];
    u_int	mSplitCnt;
    int		mDiceGridWd;
    int		mDiceGridHe;

public:
    SimplePrimitiveBase( PrimitiveBase::Type type ) :
        PrimitiveBase(type),
        mSplitCnt(0),
        mDiceGridWd(-1),
        mDiceGridHe(-1)
    {
        mURange[0] = 0;
        mURange[1] = 1;
        mVRange[0] = 0;
        mVRange[1] = 1;
    }

    SimplePrimitiveBase(
                PrimitiveBase::Type type,
                float umin,
                float umax,
                float vmin,
                float vmax ) :
        PrimitiveBase(type),
        mSplitCnt(0),
        mDiceGridWd(-1),
        mDiceGridHe(-1)
    {
        mURange[0] = umin;
        mURange[1] = umax;
        mVRange[0] = vmin;
        mVRange[1] = vmax;
    }

    virtual ~SimplePrimitiveBase() {}

        bool	IsComplex() const		{ return false;	}
        bool	IsSplitable() const		{ return true;	}

    virtual SimplePrimitiveBase	*Clone() const = 0;

    enum CheckSplitRes
    {
        CHECKSPLITRES_CULL,
        CHECKSPLITRES_DICE,
        CHECKSPLITRES_SPLIT,
    };

    CheckSplitRes CheckForSplit(
                            const Hider &hider,
                            int			out_bound2d[4],
                            bool		&out_uSplit,
                            bool		&out_vSplit );

    void	Split( Hider &hider, bool uSplit, bool vSplit );

    // WARNING: we assume dicing no larger than 3^2 !!!
    // ..make sure about this everywhere MakeBound() uses MakeBoundFromUVRangeN
    static const size_t	MAKE_BOUND_FROM_UV_RANGE_DIM = 4;
    static const size_t	MAX_MAKE_BOUND_OUT_SIZE =
                            MAX_PADDED_VERTS_FOR_MakeBoundFromUVRangeN( MAKE_BOUND_FROM_UV_RANGE_DIM );

    // make a 3D bound, return false if the bound cannot be made
    virtual void	MakeBound( Bound &out_bound, Float3_ *out_pPo ) const = 0;

    inline Float3_	&EvalP(
                        const Float2_ &uv,
                        Float3_ &out_pt ) const
    {
        Eval_dPdu_dPdv( uv, out_pt, NULL, NULL );
        return out_pt;
    }

    inline void EvalP(
                        const Float_ *pUArray,
                        const Float_ *pVArray,
                        Float3_ *out_pPts,
                        size_t	ptsN ) const
    {
        size_t	blksN =  DMT_SIMD_BLOCKS( ptsN );
        for (size_t i=0; i < blksN; ++i)
        {
            Float2_	uv( pUArray[i], pVArray[i] );
            Eval_dPdu_dPdv( uv, out_pPts[i], NULL, NULL );
        }
    }

/*
    virtual void	Eval_dPdu_dPdv(
                        float u,
                        float v,
                        Point3 &out_pt,
                        Float3 *out_dPdu,
                        Float3 *out_dPdv ) const
    {
        DASSERT( 0 );
    }
*/

    virtual void	Eval_dPdu_dPdv(
                        const Float2_ &uv,
                        Float3_ &out_pt,
                        Float3_ *out_dPdu,
                        Float3_ *out_dPdv ) const = 0;

    inline void Eval_dPdu_dPdv(
                        const Float_ *pUArray,
                        const Float_ *pVArray,
                        Float3_ *out_pPts,
                        Float3_ *out_dPdu,
                        Float3_ *out_dPdv,
                        size_t	ptsN ) const
    {
        size_t	blksN =  DMT_SIMD_BLOCKS( ptsN );
        for (size_t i=0; i < blksN; ++i)
        {
            Float2_	uv( pUArray[i], pVArray[i] );
            Eval_dPdu_dPdv( uv, out_pPts[i], &out_dPdu[i], &out_dPdv[i] );
        }
    }

    virtual void	Dice(
                        WorkGrid &g,
                        bool doColorCoded ) const;

/*
    inline Float2 CalcLocalUV( const Float2 &gridUV ) const
    {
        return
            Float2(
                DMix( mURange[0], mURange[1], gridUV.x() ),
                DMix( mVRange[0], mVRange[1], gridUV.y() )
            );
    }
*/
    inline Float2 CalcLocalUV( float u, float v ) const
    {
        return
            Float2(
                DMix( mURange[0], mURange[1], u ),
                DMix( mVRange[0], mVRange[1], v )
            );
    }

    inline float CalcLocalU( const float t ) const
    {
        return DMix( mURange[0], mURange[1], t );
    }

private:
    void fillUVsArray(
                Float2_ locUV[],
                Float2_ locDUDV[],
                float du,
                float dv,
                u_int xDim,
                u_int yDim ) const;

    void fillUVsArray(
                Float2_ locUV[],
                float du,
                float dv,
                u_int xDim,
                u_int yDim ) const;
};

//==================================================================
bool ParamsFindP(	ParamList &params,
                    const SymbolList &globalSymbols,
                    DVec<Float3> &out_vectorP,
                    int fromIdx=1 );

bool ParamsFindP(	ParamList &params,
                    const SymbolList &globalSymbols,
                    Float3	*pOut_vectorP,
                    int		expectedN,
                    int		fromIdx=1 );

//==================================================================
}

#endif
