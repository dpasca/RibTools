//==================================================================
/// RSLC_Prepro_Conditionals.h
///
/// Created by Davide Pasca - 2010/2/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RSLC_PREPRO_CONDITIONALS_H
#define RSLC_PREPRO_CONDITIONALS_H

#include "RSLC_Prepro_Base.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{
	
//==================================================================
typedef DVec<bool>	ActiveStack;

//==================================================================
void HandleIfDef(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				size_t		startPoint,
				FatBase		&fatBase,
				SymbolsMap	&symbols,
				ActiveStack	&actStack,
				bool		expectFound
				);

//==================================================================
void HandleEndIf(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				size_t		startPoint,
				FatBase		&fatBase,
				ActiveStack	&actStack
				);

//==================================================================
void HandleElse(
				DVec<Fat8>	&text,
				size_t		i,
				size_t		lineEnd,
				size_t		startPoint,
				FatBase		&fatBase,
				ActiveStack	&actStack
				);

//==================================================================
}
//==================================================================
}

#endif
