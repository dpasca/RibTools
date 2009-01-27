/*
 *  DUtils.h
 *  RibTools
 *
 *  Created by Davide Pasca on 08/12/31.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef DUTILS_H
#define DUTILS_H

#ifdef NDEBUG
	#define DASSERT(_X_)			((void)0)
#else
	#define DASSERT(_X_)			\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssThrow( ok, __FILE__, __LINE__, NULL );\
			} }
#endif

#define DASSTHROW(_X_,_MSG_)		\
			{\
				bool ok=(_X_);\
				if ( !ok ) {\
					DUT::DAssThrow( ok, __FILE__, __LINE__, DUT::SSPrintF _MSG_ );\
			} }

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, void * &out_pData, size_t &out_dataSize );

char *SSPrintF( const char *pFmt, ... );

void DAssThrow( bool ok, const char *pFile, int line, char *pNewCharMsg );

//==================================================================
}

#endif