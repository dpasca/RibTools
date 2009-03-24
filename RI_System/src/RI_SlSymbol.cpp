/*
 *  RI_SlSymbol.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/03/16.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "stdafx.h"
#include "RI_Base.h"
#include "RI_SlSymbol.h"

//==================================================================
namespace RI
{

//==================================================================
void *SlSymbol::AllocClone( size_t size )
{
	DASSERT( mpDefaultVal == NULL );

	void	*pOutData = NULL;

	switch ( mType )
	{
	case SlSymbol::FLOAT:	pOutData = new float	[ size ]; break;
	case SlSymbol::POINT:	pOutData = new Point3	[ size ]; break;
	case SlSymbol::COLOR:	pOutData = new Color	[ size ]; break;
	case SlSymbol::STRING:	pOutData = new char		[ size ]; break;
	case SlSymbol::VECTOR:	pOutData = new Vector3	[ size ]; break;
	case SlSymbol::NORMAL:	pOutData = new Vector3	[ size ]; break;
	case SlSymbol::MATRIX:	pOutData = new Matrix44 [ size ]; break;
	}
	
	return pOutData;
}

//==================================================================
void SlSymbol::AllocData()
{
	DASSERT( mpDefaultVal == NULL );

	mpDefaultVal = AllocClone( mArraySize );
}

//==================================================================
void SlSymbol::FreeClone( void *pData )
{
	if NOT( pData )
		return;

	switch ( mType )
	{
	case SlSymbol::FLOAT:	delete [] ((float *)pData); break;
	case SlSymbol::POINT:	delete [] ((Point3 *)pData ); break;
	case SlSymbol::COLOR:	delete [] ((Color *)pData ); break;
	case SlSymbol::STRING:	delete [] ((char *)pData ); break;
	case SlSymbol::VECTOR:	delete [] ((Vector3 *)pData ); break;
	case SlSymbol::NORMAL:	delete [] ((Vector3 *)pData ); break;
	case SlSymbol::MATRIX:	delete [] ((Matrix44 *)pData ); break;
	}
}

//==================================================================
void SlSymbol::FreeData()
{
	if NOT( mpDefaultVal )
		return;

	FreeClone( mpDefaultVal );
	mpDefaultVal = NULL;
}

//==================================================================
SlSymbolList::~SlSymbolList()
{
	for (size_t i=0; i < size(); ++i)
	{
		(*this)[i].FreeData();
	}
}

//==================================================================
}