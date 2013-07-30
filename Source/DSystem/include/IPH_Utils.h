//==================================================================
/// IPH_Utils.h
///
/// Created by Davide Pasca - 2010/12/28
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if !defined(TARGET_OS_IPHONE)
	#error This is for iPhone only !
#endif

#ifndef IPH_UTILS_H
#define IPH_UTILS_H

//==================================================================
namespace IPHUT
{

void OpenURL( const char *pURLStr );
bool CanOpenURL( const char *pURLStr );

void ShowAlert( const char *pTitle, const char *pText );

//==================================================================
}


#endif
