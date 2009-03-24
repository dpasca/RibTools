/*
 *  RI_SlRunContext.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/20.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_SlRunContext.h"
#include "RI_MicroPolygonGrid.h"

//==================================================================
namespace RI
{

//==================================================================
/// SlRunContext
//==================================================================
void SlRunContext::InitializeSIMD( MicroPolygonGrid &g )
{
	mSIMDCount = g.GetPointsN();
	mpSIMDFlags = new int [ mSIMDCount ];
	for (u_int i=0; i < mSIMDCount; ++i)
		mpSIMDFlags[i] = 0;
		
	mpSymbols = &g.mSymbols;
}


//==================================================================
}

