/*
 *  RI_Primitive_Base.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/18.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_PRIMITIVE_BASE_H
#define RI_PRIMITIVE_BASE_H

#include "DMath.h"
#include "RI_Param.h"
#include "RI_Symbol.h"
#include "RI_MicroPolygonGrid.h"

const static int NSUBDIVS = 16;
#define PUTPRIMNAME	//puts

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;
class FrameworkBase;
class MicroPolygonGrid;

//==================================================================
/// GVert
//==================================================================
struct GVert
{
	union 
	{
		struct {
			float	x, y, z;
			float	u, v;
		};
		
		float	vec[5];
	};
};

//==================================================================
/// GState
//==================================================================
class GState
{
	Matrix44	mMtxLocalHomo;
	float		mHalfXRes;
	float		mHalfYRes;

public:
	//==================================================================
	void Setup(
			const Options		&opt,
			const Attributes	&attr,
			const Transform		&xform,
			const Matrix44		&mtxWorldCamera );

	//==================================================================
	inline void AddVertex( const GVert &vert )
	{
		Vector4	homoPos = MultiplyV3W1M( Vector3( vert.x, vert.y, vert.z ), mMtxLocalHomo );

		//printf( "  vtx-scr: %f %f %f %f\n", sx, sy, sz, oow );
	#ifdef __gl_h_
		float	oow = 1.0f / homoPos.w;

		float sx = mHalfXRes + mHalfXRes * oow * homoPos.x;
		float sy = mHalfYRes - mHalfYRes * oow * homoPos.y;
		float sz = oow * homoPos.z;

		//glColor3f( (int)(vert.u*8)/8.0f + vert.v/8, 0, 0 );
		glColor3f( vert.u, vert.v, 0 );
		glVertex3f( sx, sy, oow );
	#endif
	}
};

//==================================================================
/// Primitive
//==================================================================
class Primitive
{
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

		POLYGON,
	};

	Type				mType;
	Attributes			*mpAttribs;
	Transform			*mpTransform;
	
	// some prims may no need these.. like for the polygons
	// ..but for now we add them anyway !
	float				mURange[2];
	float				mVRange[2];
	u_int				mSplitCnt;

public:
	Primitive( Type type ) :
		mType(type),
		mpAttribs(NULL),
		mpTransform(NULL),
		mSplitCnt(0)
	{
		mURange[0] = 0;
		mURange[1] = 1;
		mVRange[0] = 0;
		mVRange[1] = 1;
	}
/*
	Primitive( const Primitive *pSrc ) :
		mType(		pSrc->mType),
		mpAttribs(	pSrc->mpAttribs),
		mpTransform(pSrc->mpTransform),
		mURange(	pSrc->mURange),
		mVRange(	pSrc->mVRange)
	{
	}
*/
	virtual ~Primitive()
	{
	}
	
	virtual Primitive	*Clone() const = 0;
	
	virtual void	MakeBound( Bound &out_bound ) = 0;

	virtual bool	IsSplitable() const			{ return true;	}
	virtual void	Split( FrameworkBase &fwork, bool uSplit, bool vSplit );

	virtual Point3	&EvalP(
						float uGrid,
						float vGrid,
						Point3 &out_pt ) const { return out_pt; }

	virtual bool	IsDiceable(
						MicroPolygonGrid &g,
						class HiderBase *pHider,
						bool &out_uSplit,
						bool &out_vSplit )
					{
						return false;
					}

	virtual void	Dice( MicroPolygonGrid &g );

	void SetStates(
		Attributes	*pAttribs,
		Transform	*pTransform
	)
	{
		mpAttribs	= pAttribs	;
		mpTransform	= pTransform;
	}

	void CopyStates( Primitive &fromPrim )
	{
		mpAttribs	= fromPrim.mpAttribs	;
		mpTransform	= fromPrim.mpTransform;
	}
	
	bool IsUsable() const
	{
		return mpAttribs != 0 && mpTransform != 0;
	}

	void MarkUnusable()
	{
		mpAttribs	= 0;
		mpTransform	= 0;
	}
/*
	virtual void Render( GState &gstate )
	{
	}
*/
};

//==================================================================
class DiceablePrim : public Primitive
{
public:
	DiceablePrim( Type type ) :
		Primitive(type)
	{
	}

		bool	IsDiceable(
						MicroPolygonGrid &g,
						HiderBase *pHider,
						bool &out_uSplit,
						bool &out_vSplit );
};

//==================================================================
bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					DVec<Vector3> &out_vectorP,
					int fromIdx=1 );

bool ParamsFindP(	ParamList &params,
					const SymbolList &staticSymbols,
					Vector3	*pOut_vectorP,
					int		expectedN,
					int		fromIdx=1 );

//==================================================================
}

#endif
