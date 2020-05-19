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

#if defined(_MSC_VER)
#define D_OVERRIDE_NEW_DEL
//#defined D_FILE_LINE_NEW_DEL
#endif

//==================================================================
#if defined(D_FILE_LINE_NEW_DEL) && defined(D_OVERRIDE_NEW_DEL)

	#define DNEW				new			(__FILE__, __LINE__)
	#define DDELETE(_X_)		delete		(__FILE__, __LINE__, (_X_))
	#define DDELETE_ARRAY(_X_)	delete []	(__FILE__, __LINE__, (_X_))

	void *operator new( size_t size, const char *pFile, int line );
	void *operator new [] ( size_t size, const char *pFile, int line );
	void operator delete( void *p, const char *pFile, int line );
	void operator delete [] ( void *p, const char *pFile, int line );

#else

	#define DNEW				new
	#define DDELETE(_X_)		delete		(_X_)
	#define DDELETE_ARRAY(_X_)	delete []	(_X_)

#endif

#if defined(D_OVERRIDE_NEW_DEL)

	void *operator new( size_t size );
	void *operator new [] ( size_t size );
	void operator delete( void *p );
	void operator delete [] ( void *p );

#endif

#define DSAFE_DELETE(_X_)		{ if ( _X_ ) { DDELETE		(_X_); (_X_) = 0; } }
#define DSAFE_DELETE_ARRAY(_X_)	{ if ( _X_ ) { DDELETE_ARRAY(_X_); (_X_) = 0; } }

#endif
