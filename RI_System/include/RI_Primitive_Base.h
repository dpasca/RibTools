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
		Vector4	homoPos = Vector3( vert.x, vert.y, vert.z ) * mMtxLocalHomo;

		float	oow = 1.0f / homoPos.w;

		float sx = mHalfXRes + mHalfXRes * oow * homoPos.x;
		float sy = mHalfYRes - mHalfYRes * oow * homoPos.y;
		float sz = oow * homoPos.z;
		
		//printf( "  vtx-scr: %f %f %f %f\n", sx, sy, sz, oow );
	#ifdef __gl_h_
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

public:
	Primitive( Type type ) :
		mType(type),
		mpAttribs(NULL),
		mpTransform(NULL)
	{
	}

	virtual ~Primitive()
	{
	}
	
	virtual void	MakeBound( Bound &out_bound ) = 0;

	virtual bool	IsSplitable() const			{ return false;	}
	virtual void	Split( FrameworkBase &fwork, bool uSplit, bool vSplit )	{}
	virtual void	EvalP(
						float uGrid,
						float vGrid,
						Point3 &out_pt,
						const Matrix44 &mtxObjectCurrent ) const {}

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

	virtual void Render( GState &gstate )
	{
	}
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
