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

#define DASSERT(_X_)			assert(_X_)
#define DASSTHROW(_X_,_MSG_)	{ bool ok=(_X_); if ( !ok ) { DUT::DAssThrow( ok, DUT::SSPrintF _MSG_ ); } }

//==================================================================
namespace DUT
{

//==================================================================
bool GrabFile( const char *pFileName, void * &out_pData, size_t &out_dataSize );

char *SSPrintF( const char *pFmt, ... );

void DAssThrow( bool ok, char *pNewCharMsg );

//==================================================================
}

#endif