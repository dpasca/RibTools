//==================================================================
/// RSLC_Register.cpp
///
/// Created by Davide Pasca - 2009/11/3
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DSystem/include/DTypes.h"
#include "RSLC_Register.h"

//==================================================================
namespace RSLC
{

//==================================================================
std::string Register::GetName() const
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
	default:
		strcpy_s( regBase, "UNK_" );
		//DASSERT( 0 );
		break;
	}

	if NOT( IsVarying() )
		strcat_s( regBase, "u" );

	return DUT::SSPrintFS( "$%s%i", regBase, GetRegIdx() );
}

//==================================================================
}
