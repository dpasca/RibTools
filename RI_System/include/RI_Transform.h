/*
 *  RI_Transform.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_TRANSFORM_H
#define RI_TRANSFORM_H

#include "RI_Base.h"
#include "DMath/include/DMatrix44.h"

//==================================================================
namespace RI
{

//==================================================================
/// Transform
//==================================================================
class Transform
{
public:
    Transform() : mMatrix(true)	{ }

    // avoid initialization of default values and just copy..
    Transform( const Transform &fromObj ) {	*this = fromObj; }
    ~Transform() {}

    //==================================================================
    void Init( RevisionTracker *pRevision )
    {
        mpRevision = pRevision;
    }

    const Matrix44 &GetMatrix() const	{	return mMatrix;		}

    void SetIdentity()
    {
        mpRevision->BumpRevision();
        mMatrix.Identity();
    }

    void ConcatTransform( const Matrix44 &m )
    {
        mpRevision->BumpRevision();

        // check if not identity ?
        mMatrix = m * mMatrix;
    }

    void CopyRowMajor( const float *pSrcMtx )
    {
        mpRevision->BumpRevision();
        mMatrix.CopyRowMajor( pSrcMtx );
    }

public:
    RevisionTracker	*mpRevision;

private:
    Matrix44	mMatrix;
};

//==================================================================
}

#endif
