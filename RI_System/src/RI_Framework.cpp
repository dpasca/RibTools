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
void Framework::WorldBegin()
{
	for (size_t i=0; i < mpPrims.size(); ++i)
		delete mpPrims[i];
	mpPrims.clear();
}

//==================================================================
void Framework::Insert(	Primitive			*pPrim,
						const Options		&opt,
						const Attributes	&attr,
						const Transform		&xform )
{
	if ( mOptsRev.Sync( opt ) )
		mpUniqueOptions.push_back( new Options( opt ) );
	
	if ( mAttrsRev.Sync( attr ) )
		mpUniqueAttribs.push_back( new Attributes( attr ) );

	if ( mTransRev.Sync( xform ) )
		mpUniqueTransform.push_back( new Transform( xform ) );

	pPrim->SetStates(
				mpUniqueOptions.back(),
				mpUniqueAttribs.back(),
				mpUniqueTransform.back()
				);

	mpPrims.push_back( pPrim );
}

//==================================================================
void Framework::WorldEnd( const Matrix44 &mtxWorldCamera )
{
	GState	gstate;

	for (size_t i=0; i < mpPrims.size(); ++i)
	{	
		Primitive	*pPrim = mpPrims[i];

		gstate.Setup(
				*pPrim->mpOptions,
				*pPrim->mpAttribs,
				*pPrim->mpTransform,
				mtxWorldCamera );

		pPrim->Render( gstate );
	}

	for (size_t i=0; i < mpUniqueOptions.size(); ++i)	delete mpUniqueOptions[i];
	for (size_t i=0; i < mpUniqueAttribs.size(); ++i)	delete mpUniqueAttribs[i];
	for (size_t i=0; i < mpUniqueTransform.size(); ++i)	delete mpUniqueTransform[i];
	for (size_t i=0; i < mpPrims.size(); ++i)			delete mpPrims[i];
	
	mpUniqueOptions.clear();
	mpUniqueAttribs.clear();
	mpUniqueTransform.clear();
	mpPrims.clear();
}



//==================================================================
}
