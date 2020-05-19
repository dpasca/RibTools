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
#include "DSafeCrt.h"

#if !defined(ANDROID)
# include <string.h>
#if !defined(_MSC_VER)
# include <stdint.h>
#else
typedef unsigned char		uint8_t;
typedef unsigned short		uint16_t;
typedef unsigned int		uint32_t;
#endif
#endif

//===============================================================
#ifdef _MSC_VER
inline int strcasecmp( const char *a, const char *b )
{
	return _stricmp( a, b );
}

inline int strncasecmp( const char *a, const char *b, size_t len )
{
	return _strnicmp( a, b, len );
}

#define strtok_r	strtok_s

#endif

//===============================================================
#define NOT(_X_)				(!(_X_))

#ifndef _countof
#define _countof(_X_)			(sizeof(_X_)/sizeof((_X_)[0]))
#endif

#define	DMIN(_A_,_B_)			((_A_) < (_B_) ? (_A_) : (_B_))
#define	DMAX(_A_,_B_)			((_A_) > (_B_) ? (_A_) : (_B_))

//===============================================================
#if defined(_MSC_VER)
# define DFORCEINLINE __forceinline
#else
# if defined(DEBUG)
#  define DFORCEINLINE inline
# else
#  define DFORCEINLINE inline __attribute__((always_inline))
# endif
#endif

//===============================================================
namespace D
{

//===============================================================
template <class T>
T Lerp( const T& left, const T& right, float t )
{
	return (T)(left * (1-t) + right * t);
}

template <class T>
T Lerp( const T& left, const T& right, double t )
{
	return (T)(left * (1-t) + right * t);
}

//===============================================================
template <class T>
T Clamp( const T& val, const T& left, const T& right )
{
	if ( val < left )	return left;	else
	if ( val > right )	return right;	else
						return val;
}

//===============================================================
template <class T>
T Min( const T& left, const T& right )
{
	return DMIN( left, right );
}

//===============================================================
template <class T>
T Max( const T& left, const T& right )
{
	return DMAX( left, right );
}

//===============================================================
template <class T>
T Abs( const T& val )
{
	return val < 0 ? -val : val;
}

//===============================================================
template <class T>
T Sign( const T& val )
{
	return val < 0 ? (T)-1 : (T)1;
}

//===============================================================
template <class T>
void Swap( T& left, T& right )
{
	T tmp = left;
	left = right;
	right = tmp;
}

//===============================================================
}

typedef unsigned char		U8;
typedef unsigned short		U16;
typedef unsigned int		U32;
typedef			 int		I32;

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

#define DNPOS					((size_t)-1)
#define DNPOS32					((U32)-1)

//===============================================================
// Assume that only Visual Studio means that we have unaligned
// memory access.
#if defined(_MSC_VER)
	#define D_UNALIGNED_MEM_ACCESS
#endif

#if defined(_MSC_VER)
	#define SIZE_T_FMT	"%Id"
#else
	#define SIZE_T_FMT	"%zd"
#endif

#endif
