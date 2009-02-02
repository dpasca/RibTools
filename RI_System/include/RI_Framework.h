/*
 *  RI_Framework.h
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_FRAMEWORK_H
#define RI_FRAMEWORK_H

#include "RI_Base.h"
#include "RI_Primitive.h"
#include "DContainers.h"
#include "RI_Options.h"

//==================================================================
namespace RI
{

class Options;
class Attributes;
class Transform;

//==================================================================
/// Framework
//==================================================================
class Framework
{
	DVec<Primitive *>	mpPrims;
	
public:
	SymbolList			*mpStatics;

private:
	Options				mOptions;

	DVec<Attributes*>	mpUniqueAttribs;
	DVec<Transform*>	mpUniqueTransform;

	RevisionChecker		mAttrsRev;
	RevisionChecker		mTransRev;
				
public:
	Framework() :
		mpStatics(NULL)
	{
	}

	void Init( SymbolList *pStatics )
	{
		mpStatics = pStatics;
	}

	void SetOutput( u_int width, u_int height );
	
	void WorldBegin( const Options &opt );

	void Insert( Primitive			*pPrim,
				 const Attributes	&attr,
				 const Transform	&xform );

	void InsertSplitted(	
						Primitive			*pSplitPrim,
						Primitive			&srcPrim
						);

	void WorldEnd( const Matrix44 &mtxWorldCamera );
};

//==================================================================
}

#endif