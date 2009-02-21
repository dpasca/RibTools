/*
 *  Engine1.h
 *  Polygoner1
 *
 *  Created by Davide Pasca on 09/02/22.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef ENGINE1_H
#define ENGINE1_H

#include <float.h>
#include <limits.h>
#include "DContainers.h"
#include "DMatrix44.h"

//===============================================================
class Mesh;

//===============================================================
class BBox
{
public:
	Vector3	mBox[1];
	
	BBox( bool doInitialize )
	{
		if ( doInitialize )
		{
			mBox[0].Set(  FLT_MAX,  FLT_MAX,  FLT_MAX );
			mBox[1].Set( -FLT_MAX, -FLT_MAX, -FLT_MAX );
		}
	}

	void Expand( const Vector3 &v )
	{
		mBox[0].x = DMIN( mBox[0].x, v.x );
		mBox[0].y = DMIN( mBox[0].y, v.y );
		mBox[0].z = DMIN( mBox[0].z, v.z );
		mBox[1].x = DMAX( mBox[1].x, v.x );
		mBox[1].y = DMAX( mBox[1].y, v.y );
		mBox[1].z = DMAX( mBox[1].z, v.z );
	}

	void Expand( const BBox &b )
	{
		Expand( b.mBox[0] );
		Expand( b.mBox[1] );
	}
};

//===============================================================
class ObjBase
{
public:
	enum ObjType
	{
		OT_NODE,
		OT_MESH,
		OT_N
	};
	
	ObjType	mType;
	DStr	mName;
	
	ObjBase( ObjType type, const char *pName ) :
		mType(type), mName(pName)
	{
	}

	virtual ~ObjBase() {}
};

//===============================================================
class Node : public ObjBase
{
public:
	Node		*mpParent;
	Node		*mpNext;
	Node		*mpChild;

	Matrix44	mMtxParentLocal;
	Matrix44	mMtxWorldLocal;
	
	DVec<Mesh *>	mpMeshes;

	Node( const char *pName ) :
		ObjBase(ObjBase::OT_NODE, pName),
		mpParent(NULL),
		mpNext(NULL),
		mpChild(NULL),
		mMtxParentLocal(true),
		mMtxWorldLocal(true)
	{
	}
};

//===============================================================
class Mesh : public ObjBase
{
public:
	enum VertType
	{
		VT_POS,
		VT_TX0,
		VT_NOR,
		VT_COL,
		VT_N
	};

	DVec<float>	mVerts[VT_N];
	DVec<int>	mIndices[VT_N];
	Vector3		mPosBBox;
	
	Mesh( const char *pName ) :
		ObjBase(ObjBase::OT_MESH, pName)
	{
	}
};

//===============================================================
class Container
{
public:
	DVec<ObjBase *>	mpObjects[ObjBase::OT_N];
	
	void AddObject( ObjBase *pObj )
	{
		mpObjects[pObj->mType].push_back( pObj );
	}
};

//===============================================================
class RendBuff
{
public:
	u_int	mWd;
	u_int	mHe;
	
	
};

//===============================================================
void RendMesh( const Mesh *pMesh, const Matrix44 &mtxProjLocal );

#endif
