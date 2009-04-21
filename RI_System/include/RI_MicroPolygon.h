//==================================================================
/// RI_MicroPolygon.h
///
/// Created by Davide Pasca - 2009/4/21
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_MICROPOLYGON_H
#define RI_MICROPOLYGON_H

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DContainers.h"
#include "DMath/include/DMath.h"
#include "RI_SlSymbol.h"

//==================================================================
namespace RI
{

//==================================================================
class ShadedVertex
{
	int		RefCount;

public:
	Point3	P;
	Point3	Pclose;
	Color	Ci;
	Color	Oi;

	void AddRef()	{	++RefCount;	}
	void SubRef()	{	--RefCount;	}
};

//==================================================================
class VertexProxy
{
	// WARNING: has no default constructor.. the pointer will be
	// junk at the beginning !
	ShadedVertex	*mpVertex;

public:
	void Borrow( ShadedVertex *pVert )
	{
		pVert->AddRef();
		mpVertex = pVert;
	}

	void Release()
	{
		mpVertex->SubRef();
		mpVertex = NULL;
	}
};

//==================================================================
class MicroPolygon
{
public:
	VertexProxy	mVerts[4];

	int			mXMin;
	int			mYMin;
	int			mXMax;
	int			mYMax;
	float		mZMin;
	float		mZMax;

public:
	MicroPolygon(
		ShadedVertex *pV0,
		ShadedVertex *pV1,
		ShadedVertex *pV2,
		ShadedVertex *pV3
		)
	{
		mVerts[0].Borrow( pV0 );
		mVerts[1].Borrow( pV1 );
		mVerts[2].Borrow( pV2 );
		mVerts[3].Borrow( pV3 );
	}
};

//==================================================================
}

#endif
