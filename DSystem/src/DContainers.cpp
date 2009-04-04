/*
 *  DContainers.cpp
 *  ribparser
 *
 *  Created by Davide Pasca on 08/12/17.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifdef _MSC_VER
	#include <malloc.h>

#elif defined(__MACH__)
	#include <mm_malloc.h>

#endif

#include "DContainers.h"

#if defined(_mm_malloc) && defined(_mm_free)

//===============================================================
void *operator new( size_t size ) 
{ 
	void *p = _mm_malloc( size, 16 );
	DASSTHROW( p != NULL, ("new failed allocating %u values !", size) )

	return p; 
}

//===============================================================
void *operator new [] ( size_t size ) 
{ 
	void *p = _mm_malloc( size, 16 );
	DASSTHROW( p != NULL, ("new failed allocating %u values !", size) )

	return p; 
}

//===============================================================
void operator delete( void *p ) 
{
	if ( p )
		_mm_free( p );
}

//===============================================================
void operator delete [] ( void *p ) 
{
	if ( p )
		_mm_free( p );
}

#endif
