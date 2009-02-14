/*
 *  RI_FrameworkBase.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_FRAMEWORKBASE_H
#define RI_FRAMEWORKBASE_H

#include "RI_Base.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;
class Primitive;
class HiderBase;
class SymbolList;

//==================================================================
class RenderOutputBase
{
public:
	virtual ~RenderOutputBase() {};

	virtual void Update( u_int w, u_int h, const float *pSrcData ) = 0;
	virtual void Blit() const = 0;
	virtual u_int GetCurWd() const = 0;
	virtual u_int GetCurHe() const = 0;
};

//==================================================================
/// FrameworkBase
//==================================================================
class FrameworkBase
{
public:
	RenderOutputBase	*mpRenderOutput;
	SymbolList			*mpStatics;

protected:
	HiderBase			*mpHider;

public:
	FrameworkBase( RenderOutputBase	*pRenderOutput ) :
		mpRenderOutput(pRenderOutput),
		mpStatics(NULL),
		mpHider(NULL)
	{
	}

	virtual ~FrameworkBase() {}

	void SetStatics( SymbolList *pStatics ) { mpStatics = pStatics;	}

	virtual void WorldBegin(
						const Options &opt,
						const Matrix44 &mtxWorldCamera ) = 0;

	virtual void Insert(
				Primitive			*pPrim,
				 const Attributes	&attr,
				 const Transform	&xform ) = 0;

	virtual void InsertSplitted(	
						Primitive			*pSplitPrim,
						Primitive			&srcPrim
						) = 0;

	virtual void Remove( Primitive *pPrim ) = 0;

	virtual void WorldEnd() = 0;
};

//==================================================================
}

#endif