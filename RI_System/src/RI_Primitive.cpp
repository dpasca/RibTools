/*
 *  RI_Primitive.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/04.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "stdafx.h"
#include "DMath.h"
#include "RI_Options.h"
#include "RI_Attributes.h"
#include "RI_Transform.h"
#include "RI_Primitive.h"

//==================================================================
namespace RI
{

//==================================================================
/// GState
//==================================================================
void GState::Setup(
				const Options		&opt,
				const Attributes	&attr,
				const Transform		&xform,
				const Matrix44		&mtxWorldCamera )
{
	//mpOpts	= &opt;
	//mpAttrs	= &attr;
	//,mpXForm(&xform)

	mMtxLocalHomo	= xform.GetMatrix() * mtxWorldCamera * opt.mMtxViewHomo;
	mHalfXRes		= opt.mXRes * 0.5f;
	mHalfYRes		= opt.mYRes * 0.5f;
}

//==================================================================
}