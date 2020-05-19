//==================================================================
/// DMemory.cpp
///
/// Created by Davide Pasca - 2009/5/9
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include "DTypes.h"
#include "DMemory.h"
#include "DExceptions.h"

#ifdef _MSC_VER
	#include <malloc.h>

#elif defined(__MACH__)
	//#include <mm_malloc.h>

#else
	#include <mm_malloc.h>

#endif

#if defined(D_FILE_LINE_NEW_DEL) && defined(D_OVERRIDE_NEW_DEL)

//==================================================================
void *operator new( size_t size, const char *pFile, int line )
{
    void *p = _mm_malloc( size, 64 );

	if NOT( p )
		DEX_BAD_ALLOC( "new failed !" );

    return p;
}

//==================================================================
void *operator new [] ( size_t size, const char *pFile, int line )
{
    void *p = _mm_malloc( size, 64 );

	if NOT( p )
		DEX_BAD_ALLOC( "new [] failed !" );

    return p;
}

//==================================================================
void operator delete( void *p, const char *pFile, int line )
{
	if ( p )
		_mm_free( p );
}

//==================================================================
void operator delete [] ( void *p, const char *pFile, int line )
{
	if ( p )
		_mm_free( p );
}
#endif

#if defined(D_OVERRIDE_NEW_DEL)

//==================================================================
void *operator new( size_t size )
{
    void *p = _mm_malloc( size, 64 );

	if NOT( p )
		DEX_BAD_ALLOC( "new failed !" );

    return p;
}

//==================================================================
void *operator new [] ( size_t size )
{
    void *p = _mm_malloc( size, 64 );

	if NOT( p )
		DEX_BAD_ALLOC( "new [] failed !" );

    return p;
}

//==================================================================
void operator delete( void *p )
{
	if ( p )
		_mm_free( p );
}

//==================================================================
void operator delete [] ( void *p )
{
	if ( p )
		_mm_free( p );
}

#endif
