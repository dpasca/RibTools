/*
 *  RI_FrameworkREYES.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/08.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_FRAMEWORKREYES_H
#define RI_FRAMEWORKREYES_H

#include "RI_Base.h"
#include "RI_Primitive.h"
#include "DContainers.h"
#include "RI_Options.h"
#include "RI_FrameworkBase.h"
#include "RI_HiderREYES.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;

//==================================================================
/// FrameworkREYES
//==================================================================
class FrameworkREYES : public FrameworkBase
{
	HiderREYES			mHiderREYES;
	Options				mOptions;	//temporary, will remove

private:
	DVec<Attributes*>	mpUniqueAttribs;
	DVec<Transform*>	mpUniqueTransform;

	RevisionChecker		mAttrsRev;
	RevisionChecker		mTransRev;

public:
	FrameworkREYES( RenderOutputBase *pRenderOutput );

	void WorldBegin(
				const Options &opt,
				const Matrix44 &mtxWorldCamera );

	void Insert( Primitive			*pPrim,
				 const Attributes	&attr,
				 const Transform	&xform );

	void InsertSplitted(	
						Primitive			*pSplitPrim,
						Primitive			&srcPrim
						);

	void Remove( Primitive *pPrim );

	void WorldEnd();
};

//==================================================================
}

#endif