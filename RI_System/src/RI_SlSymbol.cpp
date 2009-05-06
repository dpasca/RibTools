//==================================================================
/// RI_SlSymbol.cpp
///
/// Created by Davide Pasca - 2009/3/16
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "stdafx.h"
#include <exception>
#include "RI_Base.h"
#include "RI_SlSymbol.h"

//==================================================================
namespace RI
{

//==================================================================
static void *allocStream( size_t size )
{
	void *p = _aligned_malloc( size, RI_SIMD_FLT_LEN * sizeof(float) );
	if NOT( p )
		throw std::bad_alloc();

	return p;
}

//==================================================================
static void freeStream( void *p )
{
	_aligned_free( p );
}

//==================================================================
void *SlSymbol::AllocClone( size_t size )
{
	DASSTHROW( mpDefaultVal == NULL, ("Shouldn't have a defualt value !!!") );

	void	*pOutData = NULL;

	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case SlSymbol::FLOAT:	pOutData = allocStream( sizeof(SlScalar)*blocksN ); break;
	case SlSymbol::POINT:	pOutData = allocStream( sizeof(SlVec3)*blocksN ); break;
	case SlSymbol::COLOR:	pOutData = allocStream( sizeof(SlColor)*blocksN ); break;
	case SlSymbol::VECTOR:	pOutData = allocStream( sizeof(SlVec3)*blocksN ); break;
	case SlSymbol::NORMAL:	pOutData = allocStream( sizeof(SlVec3)*blocksN ); break;
	//case SlSymbol::STRING:	pOutData = new char		[ size ]; break;
	//case SlSymbol::MATRIX:	pOutData = new Matrix44 [ size ]; break;
	default:
		DASSERT( 0 );
		break;
	}

	return pOutData;
}

//==================================================================
void SlSymbol::AllocData()
{
	DASSTHROW( mpDefaultVal == NULL, ("Shouldn't have a defualt value !!!") );

	mpDefaultVal = AllocClone( mArraySize );
}

//==================================================================
void SlSymbol::FreeClone( void *pData )
{
	if NOT( pData )
		return;

	switch ( mType )
	{
	case SlSymbol::FLOAT:	freeStream( pData ); break;
	case SlSymbol::POINT:	freeStream( pData ); break;
	case SlSymbol::COLOR:	freeStream( pData ); break;
	case SlSymbol::VECTOR:	freeStream( pData ); break;
	case SlSymbol::NORMAL:	freeStream( pData ); break;
	//case SlSymbol::STRING:	freeStream( pData ); break;
	//case SlSymbol::MATRIX:	freeStream( pData ); break;
	default:
		DASSERT( 0 );
		break;
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