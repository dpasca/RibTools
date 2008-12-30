/*
 *  DTypes.h
 *  ribparser
 *
 *  Created by Davide Pasca on 08/19/11.
 *  Copyright 2008 Davide Pasca. All rights reserved.
 *
 */

#ifndef DTYPES_H
#define DTYPES_H

#include <vector>
#include <string>

//===============================================================
#define NOT(_X_)				(!(_X_))
#define DASSERT(_X_)			assert(_X_)
#define DASSTHROW(_X_,_MSG_)	{ bool ok=_X_; if ( !ok ) DAssThrow(ok, _MSG_); }

//typedef std::vector	DVec;
#define DVec		std::vector
typedef std::string	DStr;

void DAssThrow( bool ok, const char *pMsg );

#endif