//==================================================================
/// DUtils_Str.h
///
/// Created by Davide Pasca - 2011/12/11
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef DUTILS_STR_H
#define DUTILS_STR_H

#include "DStr.h"

#if defined(__OBJC__)
# include <Foundation/NSString.h>
#endif

//==================================================================
namespace DUT
{

//==================================================================
inline bool IsWhite( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}

//==================================================================
void StrStripBeginEndWhite( char *pStr );
const char *StrStrI( const char *pStr, const char *pSearch );
bool StrStartsWithI( const char *pStr, const char *pSearch );
bool StrEndsWithI( const char *pStr, const char *pSearch );
void StrToUpper( DStr &str );
void StrToUpper( char *pStr );
void StrToLower( DStr &str );
void StrToLower( char *pStr );
inline DStr StrMakeLower( const DStr &str ) { DStr ret(str); StrToLower( ret ); return ret; }
inline DStr StrMakeUpper( const DStr &str ) { DStr ret(str); StrToUpper( ret ); return ret; }
const char *StrFindFirstOf( const char *pStr, char searchCh );
const char *StrFindLastOf( const char *pStr, char searchCh );
const char *StrFindLastOfMulti( const char *pStr, const char *pChars );
const char *StrTok_StrQuot( char *pSrcStr, char **ppContext, bool *pOut_HadQuots=NULL );
void StrConvertCEscape( char *pStr );
void StrSplitLine( char *pLine, DVec<char *> &out_pPtrs, const char *pSeps, bool sepMakesEmptyStr );
DStr StrGetDirPath( const char *pStr );
const char *StrSeekToFilename( const char *pStr );

inline bool CharIsWhite( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}

size_t VecStr_find_by_idx_case( DVec<DStr> &v, const char *pVal );
void VecStr_push_back_unique_case( DVec<DStr> &v, const char *pVal );

#if defined(__OBJC__)
inline NSString *MakeNSString( const char *pSrc )
{
    return
        [NSString stringWithCString: pSrc encoding:NSUTF8StringEncoding];
}
inline NSString *MakeNSString( const DStr &str )
{
    return
        [NSString stringWithCString: str.c_str() encoding:NSUTF8StringEncoding];
}
#endif

//==================================================================
}

#endif
