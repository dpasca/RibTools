//==================================================================
/// RSLC_Register.cpp
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DTypes.h"
#include "DSystem/include/DUtils.h"
#include "RSLC_Register.h"

//==================================================================
namespace RSLC
{

//==================================================================
DStr Register::GetName() const
{
    char	regBase[16] = {0};

    switch ( GetVarType() )
    {
    case VT_FLOAT:	regBase[0] = 's'; break;
    case VT_POINT:	regBase[0] = 'v'; break;
    case VT_COLOR:	regBase[0] = 'v'; break;
    case VT_STRING:	regBase[0] = 'x'; break;
    case VT_VECTOR:	regBase[0] = 'v'; break;
    case VT_NORMAL:	regBase[0] = 'v'; break;
    case VT_MATRIX:	regBase[0] = 'm'; break;
    case VT_BOOL:	regBase[0] = 'b'; break;

    case VT_UNKNOWN:
        strcpy_s( regBase, "VT_UNKNOWN_" );
        break;

    case VT_VOID:
        strcpy_s( regBase, "VT_VOID_" );
        break;

    default:
        strcpy_s( regBase, "UNK_" );
        DASSERT( 0 );
        break;
    }

    const char *pStrFmt;

    if ( IsVarying() )
        pStrFmt = "$%s%i";
    else
        pStrFmt = "$%su%i";

    return DUT::SSPrintFS( pStrFmt, regBase, GetRegIdx() );
}

//==================================================================
}
