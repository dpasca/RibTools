/*
 *  RI_HiderREYES.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_HIDERREYES_H
#define RI_HIDERREYES_H

#include "RI_Options.h"
#include "RI_HiderBase.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;
class Primitive;

//==================================================================
/// HiderREYES
//==================================================================
class HiderREYES : public HiderBase
{
	Options				mOptions;
	DVec<Primitive *>	mpPrims;

public:
	void WorldBegin(
			const Options &opt,
			const Matrix44 &mtxWorldCamera );

	void Insert(
				Primitive			*pPrim,
				 const Attributes	&attr,
				 const Transform	&xform );

	void InsertSplitted(	
						Primitive	*pSplitPrim,
						Primitive	&srcPrim
						);

	void Remove( Primitive *pPrim );

	void WorldEnd();
	
	void Hide( MicroPolygonGrid &g );

	DVec<Primitive *>	&GetPrimList()	{ return mpPrims;	}
};

//==================================================================
}

#endif