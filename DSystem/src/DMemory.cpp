//==================================================================
/// DMemory.cpp
///
/// Created by Davide Pasca - 2009/5/9
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#include <exception>
#include "DTypes.h"
#include "DMemory.h"

#ifdef _MSC_VER
	#include <malloc.h>

#elif defined(__MACH__)
	#include <mm_malloc.h>

#else
	#include <mm_malloc.h>

#endif

//==================================================================
void *operator new( size_t size, const char *pFile, int line )
{
    void *p = _mm_malloc( size, 64 );
	if NOT( p )
		throw std::bad_alloc();

    return p;
}

//==================================================================
void *operator new [] ( size_t size, const char *pFile, int line )
{
    void *p = _mm_malloc( size, 64 );
	if NOT( p )
		throw std::bad_alloc();

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

//==================================================================
void *operator new( size_t size )
{
    void *p = _mm_malloc( size, 64 );
	if NOT( p )
		throw std::bad_alloc();

    return p;
}

//==================================================================
void *operator new [] ( size_t size )
{
    void *p = _mm_malloc( size, 64 );
	if NOT( p )
		throw std::bad_alloc();

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

//==================================================================
namespace DMEM
{

//==================================================================
}
