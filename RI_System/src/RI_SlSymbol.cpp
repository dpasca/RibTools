/*
 *  RI_SlSymbol.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/03/16.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Base.h"
#include "RI_SlSymbol.h"

//==================================================================
namespace RI
{

//==================================================================
SlSymbolList::~SlSymbolList()
{
	for (size_t i=0; i < size(); ++i)
	{
		void	*pDefVal = (*this)[i].mpDefaultVal;

		if NOT( pDefVal )
			continue;

		if ( (*this)[i].mIsVarying )
		{
			switch ( (*this)[i].mType )
			{
			case SlSymbol::FLOAT:	delete [] ((float *)pDefVal); break;
			case SlSymbol::POINT:	delete [] ((Point3 *)pDefVal ); break;
			case SlSymbol::COLOR:	delete [] ((Color *)pDefVal ); break;
			case SlSymbol::STRING:	delete [] ((char *)pDefVal ); break;
			case SlSymbol::VECTOR:	delete [] ((Vector3 *)pDefVal ); break;
			case SlSymbol::NORMAL:	delete [] ((Vector3 *)pDefVal ); break;
			case SlSymbol::MATRIX:	delete [] ((Matrix44 *)pDefVal ); break;

			default:
				DASSERT( 0 );
				break;
			}
		}
		else
		{
			switch ( (*this)[i].mType )
			{
			case SlSymbol::FLOAT:	delete ((float *)pDefVal); break;
			case SlSymbol::POINT:	delete ((Point3 *)pDefVal ); break;
			case SlSymbol::COLOR:	delete ((Color *)pDefVal ); break;
			case SlSymbol::STRING:	delete ((char *)pDefVal ); break;
			case SlSymbol::VECTOR:	delete ((Vector3 *)pDefVal ); break;
			case SlSymbol::NORMAL:	delete ((Vector3 *)pDefVal ); break;
			case SlSymbol::MATRIX:	delete ((Matrix44 *)pDefVal ); break;

			default:
				DASSERT( 0 );
				break;
			}
		}
	}
}

//==================================================================
}