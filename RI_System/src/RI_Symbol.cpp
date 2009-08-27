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
#include "RI_Symbol.h"

//==================================================================
namespace RI
{

//==================================================================
static void *allocStream( size_t size )
{
	return (void *)DNEW char [ size ];
}

//==================================================================
static void freeStream( void *p )
{
	char *pp = (char *)p;

	DSAFE_DELETE_ARRAY( pp );
}

//==================================================================
void *SlSymbol::AllocClone( size_t size ) const
{
	//DASSTHROW( mpDefaultVal == NULL, ("Shouldn't have a default value !!!") );

	void	*pOutData = NULL;

	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case SlSymbol::FLOAT:	pOutData = allocStream( sizeof(SlScalar)*blocksN ); break;
	case SlSymbol::POINT:	pOutData = allocStream( sizeof(SlVec3)*blocksN ); 	break;
	case SlSymbol::COLOR:	pOutData = allocStream( sizeof(SlColor)*blocksN );	break;
	case SlSymbol::VECTOR:	pOutData = allocStream( sizeof(SlVec3)*blocksN );	break;
	case SlSymbol::NORMAL:	pOutData = allocStream( sizeof(SlVec3)*blocksN );	break;
	case SlSymbol::MATRIX:	pOutData = allocStream( sizeof(Matrix44)*size );	break;
	//case SlSymbol::STRING:	pOutData = new char		[ size ]; break;
	default:
		DASSERT( 0 );
		break;
	}

	return pOutData;
}

//==================================================================
void SlSymbol::FillDataWithDefault( void *pDestData, size_t size ) const
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case SlSymbol::FLOAT:	for (size_t i=0; i < blocksN; ++i) ((SlScalar *)pDestData)[i] = ((const SlScalar *)mpDefaultVal)[0]; break;
	case SlSymbol::POINT:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case SlSymbol::COLOR:	for (size_t i=0; i < blocksN; ++i) ((SlColor  *)pDestData)[i] = ((const SlColor  *)mpDefaultVal)[0]; break;
	case SlSymbol::VECTOR:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case SlSymbol::NORMAL:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case SlSymbol::MATRIX:	for (size_t i=0; i < blocksN; ++i) ((Matrix44 *)pDestData)[i] = ((const Matrix44 *)mpDefaultVal)[0]; break;
	//case SlSymbol::STRING: break;
	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void SlSymbol::FillData( void *pDestData, size_t size, const void *pSrcData ) const
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case SlSymbol::FLOAT:	{ SlScalar	tmp(*(const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlScalar*)pDestData)[i] = tmp; } break;
	case SlSymbol::POINT:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case SlSymbol::COLOR:	{ SlColor	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlColor	*)pDestData)[i] = tmp; } break;
	case SlSymbol::VECTOR:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case SlSymbol::NORMAL:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case SlSymbol::MATRIX:	{ Matrix44	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((Matrix44*)pDestData)[i] = tmp; } break;
	//case SlSymbol::STRING: break;
	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void *SlSymbol::AllocData( size_t size )
{
	DASSTHROW( mpValArray == NULL && mpDefaultVal == NULL, ("Already allocated !!!") );

	mArraySize = size;
	mpValArray = AllocClone( mArraySize );
	return mpValArray;
}

//==================================================================
void SlSymbol::AllocDefault( const void *pSrcData )
{
	DASSTHROW( mpValArray == NULL && mpDefaultVal == NULL, ("Already allocated !!!") );

	mpDefaultVal = AllocClone( 1 );

	switch ( mType )
	{
	case SlSymbol::FLOAT :
			((SlScalar *)mpDefaultVal)[0] = ((const float *)pSrcData)[0];
			break;

	case SlSymbol::POINT :
			((SlVec3 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec3 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec3 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case SlSymbol::COLOR :
			((SlColor *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlColor *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlColor *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case SlSymbol::VECTOR:
	case SlSymbol::NORMAL:
			((SlVec3 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec3 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec3 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case SlSymbol::STRING:
	case SlSymbol::MATRIX:
			DASSTHROW( mpValArray == NULL, ("Currently unsupported type !") );
			break;
	}
}

//==================================================================
void SlSymbol::FreeClone( void *pData ) const
{
	if ( pData )
		freeStream( pData );
}

//==================================================================
/// SlSymbolList
//==================================================================
SlSymbolList::SlSymbolList()
{
}

//==================================================================
SlSymbolList::SlSymbolList( const SlSymbolList &from )
{
	mpSymbols = from.mpSymbols;
}

//==================================================================
void SlSymbolList::operator=( const SlSymbolList &from )
{
	mpSymbols = from.mpSymbols;
}

//==================================================================
SlSymbolList::~SlSymbolList()
{
}

//==================================================================
SlSymbol * SlSymbolList::LookupVariable( const char *pName )
{
	for (size_t i=0; i < size(); ++i)
	{
		SlSymbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
const SlSymbol * SlSymbolList::LookupVariableI( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const SlSymbol	&symbol = (*this)[i];

		if ( 0 == strcasecmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
const SlSymbol * SlSymbolList::LookupVariable( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const SlSymbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
SlSymbol * SlSymbolList::LookupVariable( const char *pName, SlSymbol::Type type )
{
	for (size_t i=0; i < size(); ++i)
	{
		SlSymbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			DASSTHROW( symbol.mType == type,
						("%s type not matching ! It's %i, but expecting %i",
								__FUNCTION__,
								symbol.mType,
								type ) );

			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
const SlSymbol * SlSymbolList::LookupVariable( const char *pName, SlSymbol::Type type ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const SlSymbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			DASSTHROW( symbol.mType == type,
						("%s type not matching ! It's %i, but expecting %i",
								__FUNCTION__,
								symbol.mType,
								type ) );

			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
void *SlSymbolList::LookupVariableData( const char *pName, SlSymbol::Type type )
{
	SlSymbol	*pSym = LookupVariable( pName, type );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
const void *SlSymbolList::LookupVariableData( const char *pName, SlSymbol::Type type ) const
{
	const SlSymbol	*pSym = LookupVariable( pName, type );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
SlSymbol *SlSymbolList::Add( const SymbolParams &params, const void *pSrcData )
{
	if ( LookupVariableI( params.mpName ) )
	{
		DASSERT( 0 );
		return NULL;
	}
	SlSymbol	*pSym = Grow();
	pSym->mName			= params.mpName;
	pSym->mType			= params.mType;
	pSym->mStorage		= params.mStorage;
	pSym->mIsVarying	= params.mIsVarying;

	if ( pSrcData )
	{
		pSym->FillData( pSym->AllocData( 1 ), 1, pSrcData );
	}

	return pSym;
}

//==================================================================
void SlSymbolList::AddVoid( const char *pName )
{
	if ( LookupVariableI( pName ) )
	{
		DASSERT( 0 );
		return;
	}
	SlSymbol	*pSym = Grow();
	pSym->mName = pName;
	pSym->mType	= SlSymbol::VOIDD;
}

//==================================================================
}
