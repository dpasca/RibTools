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

#include <sys/types.h>
#include <vector>
#include <string>

//===============================================================
#define NOT(_X_)				(!(_X_))
#define _countof(_X_)			(sizeof(_X_)/sizeof((_X_)[0]))

#define DSAFE_DELETE(_X_)		{ if ( _X_ ) { delete (_X_); (_X_) = 0; } }
#define DSAFE_DELETE_ARRAY(_X_)	{ if ( _X_ ) { delete [] (_X_); (_X_) = 0; } }

#define DNPOS					((size_t)-1)

#define	DMIN(_A_,_B_)			((_A_) < (_B_) ? (_A_) : (_B_))
#define	DMAX(_A_,_B_)			((_A_) > (_B_) ? (_A_) : (_B_))

typedef std::string	DStr;

typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;

#endif