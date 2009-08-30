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
void *Symbol::AllocClone( size_t size ) const
{
	//DASSTHROW( mpDefaultVal == NULL, ("Shouldn't have a default value !!!") );

	void	*pOutData = NULL;

	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case Symbol::FLOAT:	pOutData = allocStream( sizeof(SlScalar)*blocksN ); break;
	case Symbol::POINT:	pOutData = allocStream( sizeof(SlVec3)*blocksN ); 	break;
	case Symbol::COLOR:	pOutData = allocStream( sizeof(SlColor)*blocksN );	break;
	case Symbol::VECTOR:	pOutData = allocStream( sizeof(SlVec3)*blocksN );	break;
	case Symbol::NORMAL:	pOutData = allocStream( sizeof(SlVec3)*blocksN );	break;
	case Symbol::MATRIX:	pOutData = allocStream( sizeof(Matrix44)*size );	break;
	//case Symbol::STRING:	pOutData = new char		[ size ]; break;
	default:
		DASSERT( 0 );
		break;
	}

	return pOutData;
}

//==================================================================
void Symbol::FillDataWithDefault( void *pDestData, size_t size ) const
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case Symbol::FLOAT:	for (size_t i=0; i < blocksN; ++i) ((SlScalar *)pDestData)[i] = ((const SlScalar *)mpDefaultVal)[0]; break;
	case Symbol::POINT:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case Symbol::COLOR:	for (size_t i=0; i < blocksN; ++i) ((SlColor  *)pDestData)[i] = ((const SlColor  *)mpDefaultVal)[0]; break;
	case Symbol::VECTOR:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case Symbol::NORMAL:	for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case Symbol::MATRIX:	for (size_t i=0; i < blocksN; ++i) ((Matrix44 *)pDestData)[i] = ((const Matrix44 *)mpDefaultVal)[0]; break;
	//case Symbol::STRING: break;
	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void Symbol::FillData( void *pDestData, size_t size, const void *pSrcData ) const
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	switch ( mType )
	{
	case Symbol::FLOAT:	{ SlScalar	tmp(*(const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlScalar*)pDestData)[i] = tmp; } break;
	case Symbol::POINT:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case Symbol::COLOR:	{ SlColor	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlColor	*)pDestData)[i] = tmp; } break;
	case Symbol::VECTOR:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case Symbol::NORMAL:	{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case Symbol::MATRIX:	{ Matrix44	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((Matrix44*)pDestData)[i] = tmp; } break;
	//case Symbol::STRING: break;
	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void *Symbol::AllocData( size_t size )
{
	DASSTHROW( mpValArray == NULL && mpDefaultVal == NULL, ("Already allocated !!!") );

	mArraySize = size;
	mpValArray = AllocClone( mArraySize );
	return mpValArray;
}

//==================================================================
void Symbol::AllocDefault( const void *pSrcData )
{
	DASSTHROW( mpValArray == NULL && mpDefaultVal == NULL, ("Already allocated !!!") );

	mpDefaultVal = AllocClone( 1 );

	switch ( mType )
	{
	case Symbol::FLOAT :
			((SlScalar *)mpDefaultVal)[0] = ((const float *)pSrcData)[0];
			break;

	case Symbol::POINT :
			((SlVec3 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec3 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec3 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case Symbol::COLOR :
			((SlColor *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlColor *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlColor *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case Symbol::VECTOR:
	case Symbol::NORMAL:
			((SlVec3 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec3 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec3 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case Symbol::STRING:
	case Symbol::MATRIX:
			DASSTHROW( mpValArray == NULL, ("Currently unsupported type !") );
			break;
	}
}

//==================================================================
void Symbol::FreeClone( void *pData ) const
{
	if ( pData )
		freeStream( pData );
}

//==================================================================
/// SymbolList
//==================================================================
SymbolList::SymbolList()
{
}

//==================================================================
SymbolList::SymbolList( const SymbolList &from )
{
	mpSymbols = from.mpSymbols;
}

//==================================================================
void SymbolList::operator=( const SymbolList &from )
{
	mpSymbols = from.mpSymbols;
}

//==================================================================
SymbolList::~SymbolList()
{
}

//==================================================================
Symbol * SymbolList::LookupVariable( const char *pName )
{
	for (size_t i=0; i < size(); ++i)
	{
		Symbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
const Symbol * SymbolList::LookupVariableI( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const Symbol	&symbol = (*this)[i];

		if ( 0 == strcasecmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
const Symbol * SymbolList::LookupVariable( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const Symbol	&symbol = (*this)[i];

		if ( 0 == strcmp( symbol.mName.c_str(), pName ) )
		{
			return &symbol;
		}
	}

	return NULL;
}

//==================================================================
Symbol * SymbolList::LookupVariable( const char *pName, Symbol::Type type )
{
	for (size_t i=0; i < size(); ++i)
	{
		Symbol	&symbol = (*this)[i];

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
const Symbol * SymbolList::LookupVariable( const char *pName, Symbol::Type type ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const Symbol	&symbol = (*this)[i];

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
void *SymbolList::LookupVariableData( const char *pName, Symbol::Type type )
{
	Symbol	*pSym = LookupVariable( pName, type );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
const void *SymbolList::LookupVariableData( const char *pName, Symbol::Type type ) const
{
	const Symbol	*pSym = LookupVariable( pName, type );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
Symbol *SymbolList::Add( const SymbolParams &params, const void *pSrcData )
{
	if ( LookupVariableI( params.mpName ) )
	{
		DASSERT( 0 );
		return NULL;
	}
	Symbol	*pSym = Grow();
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
void SymbolList::AddVoid( const char *pName )
{
	if ( LookupVariableI( pName ) )
	{
		DASSERT( 0 );
		return;
	}
	Symbol	*pSym = Grow();
	pSym->mName = pName;
	pSym->mType	= Symbol::VOIDD;
}

//==================================================================
}
