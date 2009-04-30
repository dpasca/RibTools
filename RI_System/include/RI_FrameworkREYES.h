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
#include "RI_Options.h"
#include "RI_HiderREYES.h"

//==================================================================
namespace RI
{

class Attributes;
class Transform;

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
/// FrameworkREYES
//==================================================================
class FrameworkREYES
{
public:
	RenderOutputBase	*mpRenderOutput;
	SymbolList			*mpStatics;
	HiderREYES			mHider;

private:
	Options				mOptions;	//temporary, will remove

private:
	DVec<Attributes*>	mpUniqueAttribs;
	DVec<Transform*>	mpUniqueTransform;

	RevisionChecker		mAttrsRev;
	RevisionChecker		mTransRev;

public:
	FrameworkREYES(	RenderOutputBase *pRenderOutput,
					const HiderREYES &hiderParams );

	void SetStatics( SymbolList *pStatics )
	{
		mpStatics = pStatics;

		mHider.mpStatics = pStatics;
	}

	void WorldBegin(
				const Options &opt,
				const Matrix44 &mtxWorldCamera );

	void Insert(	PrimitiveBase		*pPrim,
					const Attributes	&attr,
					const Transform		&xform );

	void WorldEnd();

private:
	static void renderBucket_s( HiderREYES &hider, Bucket *pBuckets );

	void	worldEnd_simplify();
	void	worldEnd_splitAndAddToBuckets();
	void	worldEnd_renderBuckets();
};

//==================================================================
}

#endif