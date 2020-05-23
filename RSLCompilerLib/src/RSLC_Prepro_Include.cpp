//==================================================================
/// RSLC_Prepro_Include.cpp
///
/// Created by Davide Pasca - 2010/2/15
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "RSLC_Exceptions.h"
#include "RSLC_Prepro_Utils.h"
#include "RSLC_Prepro_ClearChunk.h"
#include "RSLC_Prepro_Include.h"

//==================================================================
namespace RSLC
{
//==================================================================
namespace PREPRO
{

//==================================================================
void HandleInclude(
                DVec<Fat8>				&text,
                size_t					i,
                size_t					lineEnd,
                size_t					includePoint,
                DIO::FileManagerBase	&fmanager,
                FatBase					&fatBase,
                const char				*pCurShaderDir )
{
    SkipHWhites( text, i, lineEnd );

    U8	closingSymbol;
    if ( text[i].Ch == '"' )	closingSymbol = '"';	else
    if ( text[i].Ch == '<' )	closingSymbol = '>';
    else
        throw Exception( fatBase, text[i], "No parameter for 'include' ?" );

    size_t nameStartIdx = i + 1;

    DStr	includeName;

    for (++i; i < lineEnd; ++i)
    {
        if ( text[i].Ch == closingSymbol )
        {
            DVec<U8> incFileData;

            // try the plain name first
            if ( fmanager.FileExists( includeName.c_str() ) )
            {
                fmanager.GrabFile( includeName.c_str(), incFileData );
            }
            else
            {
                // try from the current shader's pathname
                DStr	curPathFName(
                            DStr( pCurShaderDir ) +
                            "/" +
                            includeName );

                if ( fmanager.FileExists( curPathFName.c_str() ) )
                {
                    fmanager.GrabFile( curPathFName.c_str(), incFileData );
                }
                else
                {
                    DEX_RUNTIME_ERROR(
                            "Could not find '%s' for inclusion",
                            includeName.c_str() );
                }
            }

            size_t insertEnd =
                fatBase.InsertNewFile(
                        text,
                        includePoint,
                        lineEnd,
                        includeName.c_str(),
                        &incFileData[0],
                        incFileData.size() );

            size_t newEnd = ClearChunk( text, includePoint, insertEnd );
            DASSERT( newEnd <= insertEnd );

            CutVectorInclusive( text, newEnd, insertEnd );

            return;
        }

        includeName += (char)text[i].Ch;
    }

    throw Exception( fatBase, text[nameStartIdx], "No closing symbol for 'include'" );
}

//==================================================================
}
//==================================================================
}
