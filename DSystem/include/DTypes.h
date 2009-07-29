//==================================================================
/// DTypes.h
///
/// Created by Davide Pasca - 2008/19/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DTYPES_H
#define DTYPES_H

#include <sys/types.h>
#include <vector>
#include <string>

#include "DSafeCrt.h"

//===============================================================
#ifdef _MSC_VER
inline int strcasecmp( const char *a, const char *b )
{
	return _stricmp( a, b );
}

#define strtok_r	strtok_s

#endif

//===============================================================
#define NOT(_X_)				(!(_X_))

#ifndef _countof
#define _countof(_X_)			(sizeof(_X_)/sizeof((_X_)[0]))
#endif

#define DNPOS					((size_t)-1)

#define	DMIN(_A_,_B_)			((_A_) < (_B_) ? (_A_) : (_B_))
#define	DMAX(_A_,_B_)			((_A_) > (_B_) ? (_A_) : (_B_))

namespace D
{

template <class T>
T Clamp( const T& val, const T& left, const T& right )
{
	if ( val < left )	return left;	else
	if ( val > right )	return right;	else
						return val;
}

}

typedef std::string	DStr;

typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;

#if defined(_MSC_VER)
typedef unsigned __int64	U64;
typedef	__int64				I64;
#else
typedef unsigned long long	U64;
typedef	long long			I64;
#endif

//===============================================================
//#ifdef _MSC_VER

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;

//#endif

#endif
