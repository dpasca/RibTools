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

//==================================================================
/// FrameworkBase
//==================================================================
class FrameworkBase
{
public:
	SymbolList	*mpStatics;

protected:
	HiderBase	*mpHider;

public:
	FrameworkBase() : mpStatics(NULL), mpHider(NULL) {}
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