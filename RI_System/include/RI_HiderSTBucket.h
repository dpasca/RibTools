//==================================================================
/// RI_HiderSTBucket.h
///
/// Created by Davide Pasca - 2009/11/27
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_HIDERSTBUCKET_H
#define RI_HIDERSTBUCKET_H

#include "RI_Buffer2D.h"
#include "RI_HiderSampleCoordsBuffer.h"
#include "RI_Primitive_Base.h"

//==================================================================
namespace RI
{

class SimplePrimitiveBase;

//==================================================================
/// HiderBucket
//==================================================================
class HiderBucket
{
public:
    int							mX1;
    int							mY1;
    int							mX2;
    int							mY2;
    Buffer2D<NOUTCOLS>			mCBuff;
    DVec<SimplePrimitiveBase *>	mpPrims;
    HiderSampleCoordsBuffer		*mpSampCoordsBuff;

public:
    HiderBucket( int x1, int y1, int x2, int y2, HiderSampleCoordsBuffer *pSampCoordsBuff ) :
        mX1(x1),
        mY1(y1),
        mX2(x2),
        mY2(y2),
        mpSampCoordsBuff(pSampCoordsBuff)
    {
    }

    ~HiderBucket()
    {
        for (size_t i=0; i < mpPrims.size(); ++i)
            if ( mpPrims[i] )
                mpPrims[i]->Release();
    }

/*
    HiderPixel *InitBaseSamples_s( u_int wd, u_int he, u_int subs )
    {
        HiderPixel	*pPixels = DNEW HiderPixel [ wd * he ];
    }
*/

    void BeginRender()
    {
        u_int wd = (u_int)(mX2 - mX1);
        u_int he = (u_int)(mY2 - mY1);

        mCBuff.Setup( wd, he );
        mCBuff.Clear();
    }

    void EndRender( Buffer2D<NOUTCOLS> &outCBuff )
    {
        outCBuff.CopyRect( mX1, mY1, mCBuff );
        mCBuff.Free();
    }

    bool Contains( int x, int y ) const
    {
        return x >= mX1 && y >= mY1 && x < mX2 && y < mY2;
    }

    bool Intersects( int minX, int minY, int maxX, int maxY ) const
    {
        return
            minX < mX2 && maxX >= mX1 &&	// >= just to be sure.. ummm
            minY < mY2 && maxY >= mY1;
    }

    u_int GetWd() const	{ return (u_int)(mX2 - mX1); }
    u_int GetHe() const	{ return (u_int)(mY2 - mY1); }

    DVec<SimplePrimitiveBase *>	&GetPrimList()	{ return mpPrims;	}
};

//==================================================================
}

#endif
