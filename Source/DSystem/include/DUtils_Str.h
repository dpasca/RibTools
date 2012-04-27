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
const char *StrFindLastOf( const char *pStr, char searchCh );
const char *StrTok_StrQuot( char *pSrcStr, char **ppContext );
DStr StrGetDirPath( const char *pStr );

inline bool CharIsWhite( char ch )
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\f';
}

size_t VecStr_find_by_idx_case( DVec<DStr> &v, const char *pVal );
void VecStr_push_back_unique_case( DVec<DStr> &v, const char *pVal );

//==================================================================
}

#endif
