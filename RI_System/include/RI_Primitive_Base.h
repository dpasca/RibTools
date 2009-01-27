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

const static int NSUBDIVS = 16;
#define PUTPRIMNAME	//puts

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;

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

	//const Options		*mpOpts;
	//const Attributes	*mpAttrs;
	//const Transform		*mpXForm;

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

		POLYGON,
	};
	
	Type		mType;
	Options		*mpOptions;
	Attributes	*mpAttribs;
	Transform	*mpTransform;
	
public:
	Primitive( Type type ) :
		mType(type),
		mpOptions(NULL),
		mpAttribs(NULL),
		mpTransform(NULL)
	{
	}
	
	virtual ~Primitive()
	{
	}

	void SetStates(
		Options		*pOptions,
		Attributes	*pAttribs,
		Transform	*pTransform
	)
	{
		mpOptions	= pOptions	;
		mpAttribs	= pAttribs	;
		mpTransform	= pTransform;
	}

	virtual void Render( GState &gstate )
	{
	}
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