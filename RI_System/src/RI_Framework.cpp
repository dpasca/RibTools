/*
 *  RI_Framework.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Base.h"
#include "RI_Framework.h"
#include "RI_State.h"

#include <GLUT/glut.h>

//==================================================================
namespace RI
{

//==================================================================
/// Framework
//==================================================================
void Framework::SetOutput( u_int width, u_int height )
{
	glutReshapeWindow( width, height );
}

//==================================================================
void Framework::WorldBegin( const Options &opt )
{
	for (size_t i=0; i < mpPrims.size(); ++i)
		delete mpPrims[i];
	mpPrims.clear();

	mOptions = opt;
}

//==================================================================
void Framework::Insert(	Primitive			*pPrim,
						const Attributes	&attr,
						const Transform		&xform )
{
	if ( mAttrsRev.Sync( *attr.mpRevision ) )
		mpUniqueAttribs.push_back( new Attributes( attr ) );

	if ( mTransRev.Sync( *xform.mpRevision ) )
		mpUniqueTransform.push_back( new Transform( xform ) );

	pPrim->SetStates(
				mpUniqueAttribs.back(),
				mpUniqueTransform.back()
				);

	//printf( "Prim xform rev %i\n",
	//		mpUniqueTransform.back()->mpRevision->mRTrackRevisionCount );

	mpPrims.push_back( pPrim );
}

//==================================================================
void Framework::WorldEnd( const Matrix44 &mtxWorldCamera )
{
	glutReshapeWindow( mOptions.mXRes, mOptions.mYRes );

	GState	gstate;

	for (size_t i=0; i < mpPrims.size(); ++i)
	{	
		Primitive	*pPrim = mpPrims[i];

		gstate.Setup(
				mOptions,
				*pPrim->mpAttribs,
				*pPrim->mpTransform,
				mtxWorldCamera );

		pPrim->Render( gstate );
	}

	for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	delete mpUniqueAttribs[i];
	for (size_t i=0; i < mpUniqueTransform.size(); ++i)	delete mpUniqueTransform[i];
	for (size_t i=0; i < mpPrims.size(); ++i)			delete mpPrims[i];
	
	mpUniqueAttribs.clear();
	mpUniqueTransform.clear();
	mpPrims.clear();
}



//==================================================================
}
