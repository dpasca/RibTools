//==================================================================
/// IPH_Utils.mm
///
/// Created by Davide Pasca - 2010/12/28
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#if defined(TARGET_OS_IPHONE)

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include "IPH_Utils.h"

//==================================================================
namespace IPHUT
{

//==================================================================
void OpenURL( const char *pURLStr )
{
	NSString *pGayString = [NSString stringWithUTF8String:pURLStr];

	[[UIApplication sharedApplication]
			openURL:
				[NSURL URLWithString:pGayString]];
}

//==================================================================
bool CanOpenURL( const char *pURLStr )
{
    NSString *pGayString = [NSString stringWithUTF8String:pURLStr];

    return YES ==
            [[UIApplication sharedApplication]
                 canOpenURL:
                    [NSURL URLWithString:pGayString]];
}


//==================================================================
void ShowAlert( const char *pTitle, const char *pText )
{
	NSString *pTitleNS = [NSString stringWithUTF8String: pTitle];
	NSString *pTextNS  = [NSString stringWithUTF8String: pText];

	UIAlertView* alert =
        [[[UIAlertView alloc]
            initWithTitle: pTitleNS
            message: pTextNS
            delegate: NULL
            cancelButtonTitle: @"OK"
            otherButtonTitles: NULL]
            autorelease];

	[alert show];
}

//==================================================================
}

#endif

