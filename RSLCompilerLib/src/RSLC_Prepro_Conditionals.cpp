//==================================================================
/// RSLC_Prepro_Conditionals.cpp
///
/// Created by Davide Pasca - 2010/2/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Prepro_Base.h"
#include "RSLC_Prepro_Utils.h"
#include "RSLC_Exceptions.h"
#include "RSLC_Prepro_Conditionals.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
void HandleIfDef(
                DVec<Fat8>	&text,
                size_t		i,
                size_t		lineEnd,
                size_t		startPoint,
                FatBase		&fatBase,
                SymbolsMap	&symbols,
                ActiveStack	&actStack,
                bool		expectFound )
{
    if ( actStack.size() && actStack.back() == false )
    {
        // push another false, cut the line and return
        actStack.push_back( false );
        CutVectorInclusive( text, startPoint, lineEnd );
        return;
    }

    // process it
    DStr	symName;
    i = GetAlphaNumBetweenSpaces( text, i, lineEnd, fatBase, symName );

    // do we have this sample ?
    bool found = (symbols.end() != symbols.find( symName.c_str() ));

    // push the status into the stack
    actStack.push_back( found == expectFound );

    CutVectorInclusive( text, startPoint, lineEnd );
}

//==================================================================
void HandleEndIf(
                DVec<Fat8>	&text,
                size_t		i,
                size_t		lineEnd,
                size_t		startPoint,
                FatBase		&fatBase,
                ActiveStack	&actStack )
{
    if ( actStack.size() == 0 )
        throw Exception(
                    fatBase,
                    text[i],
                    "#endif not matching any #if/#ifdef/#ifndef" );

    actStack.pop_back();
    CutVectorInclusive( text, startPoint, lineEnd );
}

//==================================================================
void HandleElse(
                DVec<Fat8>	&text,
                size_t		i,
                size_t		lineEnd,
                size_t		startPoint,
                FatBase		&fatBase,
                ActiveStack	&actStack )
{
    if ( actStack.size() == 0 )
        throw Exception(
                    fatBase,
                    text[i],
                    "#else not matching any #if/#ifdef/#ifndef" );

    actStack.back() = !actStack.back();
    CutVectorInclusive( text, startPoint, lineEnd );
}

//==================================================================
}
//==================================================================
}
