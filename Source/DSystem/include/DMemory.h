//==================================================================
/// DMemory.h
///
/// Created by Davide Pasca - 2009/5/9
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef DMEMORY_H
#define DMEMORY_H

#include <new>
#include <memory>

//==================================================================
#if defined(_MSC_VER)

#define DNEW				new			(__FILE__, __LINE__)
#define DDELETE(_X_)		delete		(__FILE__, __LINE__, (_X_))
#define DDELETE_ARRAY(_X_)	delete []	(__FILE__, __LINE__, (_X_))

#else

#define DNEW				new
#define DDELETE(_X_)		delete		(_X_)
#define DDELETE_ARRAY(_X_)	delete []	(_X_)

#endif

#define DSAFE_DELETE(_X_)		{ if ( _X_ ) { DDELETE		(_X_); (_X_) = 0; } }
#define DSAFE_DELETE_ARRAY(_X_)	{ if ( _X_ ) { DDELETE_ARRAY(_X_); (_X_) = 0; } }

//==================================================================
void *operator new( size_t size, const char *pFile, int line );
void *operator new [] ( size_t size, const char *pFile, int line );
void operator delete( void *p, const char *pFile, int line );
void operator delete [] ( void *p, const char *pFile, int line );

void *operator new( size_t size );
void *operator new [] ( size_t size );
void operator delete( void *p );
void operator delete [] ( void *p );

//==================================================================
namespace DMEM
{

//==================================================================
}

#if defined(_MSC_VER) && _MSC_VER < 1600	// before VS 2010 ?
	#define unique_ptr	auto_ptr	// WARNING: not STL-safe
#endif

#endif
