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
#include "DMatrix44.h"

//==================================================================
namespace RI
{

//==================================================================
/// Transform
//==================================================================
class Transform : public RevisionTracker
{
	Matrix44	mMatrix;

public:
	Transform() : mMatrix(true)	{ }

	// avoid initialization of default values and just copy..
	Transform( const Transform &fromObj ) {	*this = fromObj; }
	~Transform() {}

	const Matrix44 &GetMatrix() const	{	return mMatrix;		}

	void SetIdentity()
	{
		RevisionTracker::BumpRevision();
		mMatrix.Identity();
	}

	void ConcatTransform( const Matrix44 &m )
	{
		RevisionTracker::BumpRevision();

		// check if not identity ?
		mMatrix = m * mMatrix;
	}

	void CopyRowMajor( const float *pSrcMtx )
	{
		RevisionTracker::BumpRevision();
		mMatrix.CopyRowMajor( pSrcMtx );
	}
};

//==================================================================
}

#endif
