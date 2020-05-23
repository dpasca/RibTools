//==================================================================
/// RI_HiderST.h
///
/// Created by Davide Pasca - 2009/4/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_HIDERST_H
#define RI_HIDERST_H

#include "RI_Options.h"
#include "RI_Buffer2D.h"
#include "RI_HiderSTBucket.h"
#include "RI_MicroPolygon.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class PrimitiveBase;

//==================================================================
/// Hider
//==================================================================
class Hider
{
public:
    const SymbolList	*mpGlobalSyms;

private:
    Matrix44			mMtxWorldCamera;
    Matrix44			mMtxWorldProj;
    float				mHalfXRes;
    float				mHalfYRes;

public:
    //==================================================================
    struct Params
    {
        int		mDbgOnlyBucketAtX;
        int		mDbgOnlyBucketAtY;
        bool	mDbgShowBuckets;
        bool	mDbgColorCodedGrids;
        bool	mDbgRasterizeVerts;

        Params() :
            mDbgOnlyBucketAtX(-1),
            mDbgOnlyBucketAtY(-1),
            mDbgShowBuckets(false),
            mDbgColorCodedGrids(false),
            mDbgRasterizeVerts(false)
        {
        }
    };

private:
    friend class Framework;

    Options					mOptions;
    Buffer2D<NOUTCOLS>		mFinalBuff;
    DVec<HiderBucket *>		mpBuckets;
    HiderSampleCoordsBuffer	mSampCoordBuffs[4];
    Params					mParams;
    DVec<PrimitiveBase *>	mpPrims;

public:
    Hider( const Params &params );
    ~Hider();

    void WorldBegin(
            const Options &opt,
            const Matrix44 &mtxWorldCamera );

    void Insert( PrimitiveBase *pPrim );

    void InsertSimple(	
                    SimplePrimitiveBase			*pSimplePrim,
                    const ComplexPrimitiveBase	&srcPrim
                    );

    void InsertSplitted(	
                    SimplePrimitiveBase			*pDesPrim,
                    const SimplePrimitiveBase	&srcPrim
                    );

    void InsertForDicing( SimplePrimitiveBase *pPrim, const int bound2d[4] );

    void WorldEnd();
    
    float RasterEstimate( const Bound &b, const Matrix44 &mtxLocalWorld, int out_box2D[4]  ) const;
    Float_ RasterLengthSqr( const Float3_ &ptA, const Float3_ &ptB, const Matrix44 &mtxLocalWorld ) const;

    void Bust(	const HiderBucket	&bucket,
                ShadedGrid			&shadGrid,
                const WorkGrid		&workGrid,
                DVec<HiderPixel>	&pixels,
                u_int				screenWd,
                u_int				screenHe ) const;

    void Hide(
                    DVec<HiderPixel>	&pixels,
                    HiderBucket			&buck );

    u_int		GetOutputDataStride() const	{ return mFinalBuff.GetWd() * NOUTCOLS;	}
    u_int		GetOutputDataWd() const		{ return mFinalBuff.GetWd();			}
    u_int		GetOutputDataHe() const		{ return mFinalBuff.GetHe();			}

    const float *GetOutputData( u_int x1, u_int y1 ) const
    {
        return mFinalBuff.GetSamplePtrR( (int)x1, (int)y1 );
    }

    float *GetOutputDataRW( u_int x1, u_int y1 )
    {
        return mFinalBuff.GetSamplePtrRW( (int)x1, (int)y1 );
    }

    const DVec<HiderBucket *>	&GetBuckets() {	return mpBuckets; }

    size_t	GetOutputBucketMemSize( size_t buckIdx ) const;
    void	CopyOutputBucket( size_t buckIdx, float *pDest, size_t destMaxSize ) const;

    void	StoreOutputBucket( size_t buckIdx, const float *pSrc, size_t srcSize );

private:
    bool	makeRasterBound(
                        const Bound &b,
                        const Matrix44 &mtxLocalWorld,
                        float out_bound2d[4] ) const;

    HiderSampleCoordsBuffer *findOrAddSampCoordBuff( u_int wd, u_int he, u_int subPixelDimLog2 );

    //void pointsTo2D( Point2 *pDes, const Point3 *pSrc, u_int n );
};

//==================================================================
}

#endif