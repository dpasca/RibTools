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

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:	pOutData = allocStream( sizeof(SlScalar)*blocksN );	break;
	case Symbol::TYP_POINT:
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:pOutData = allocStream( sizeof(SlVec3)*blocksN );	break;
	case Symbol::TYP_COLOR:	pOutData = allocStream( sizeof(SlColor)*blocksN );	break;
	case Symbol::TYP_MATRIX:pOutData = allocStream( sizeof(Matrix44)*size );	break;
	case Symbol::TYP_STRING:pOutData = allocStream( sizeof(SlStr)*size );		break;
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

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:	for (size_t i=0; i < blocksN; ++i) ((SlScalar *)pDestData)[i] = ((const SlScalar *)mpDefaultVal)[0]; break;
	case Symbol::TYP_POINT:	
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:for (size_t i=0; i < blocksN; ++i) ((SlVec3	  *)pDestData)[i] = ((const SlVec3   *)mpDefaultVal)[0]; break;
	case Symbol::TYP_COLOR:	for (size_t i=0; i < blocksN; ++i) ((SlColor  *)pDestData)[i] = ((const SlColor  *)mpDefaultVal)[0]; break;
	case Symbol::TYP_MATRIX:for (size_t i=0; i < size; ++i)	   ((Matrix44 *)pDestData)[i] = ((const Matrix44 *)mpDefaultVal)[0]; break;
	case Symbol::TYP_STRING:for (size_t i=0; i < size; ++i)	   ((SlStr	  *)pDestData)[i] = ((const SlStr	 *)mpDefaultVal)[0]; break;

	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void Symbol::FillData( void *pDestData, size_t size, const void *pSrcData ) const
{
	size_t	blocksN = RI_GET_SIMD_BLOCKS( size );

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:	{ SlScalar	tmp(*(const float *)pSrcData);for (size_t i=0; i < blocksN; ++i) ((SlScalar *)pDestData)[i] = tmp; } break;
	case Symbol::TYP_POINT:	
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:{ SlVec3	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec3	*)pDestData)[i] = tmp; } break;
	case Symbol::TYP_COLOR:	{ SlColor	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlColor	*)pDestData)[i] = tmp; } break;
	case Symbol::TYP_MATRIX:{ Matrix44	tmp((const float *)pSrcData); for (size_t i=0; i < size; ++i)	 ((Matrix44 *)pDestData)[i] = tmp; } break;
	case Symbol::TYP_STRING:{ SlStr		tmp((const SlStr *)pSrcData); for (size_t i=0; i < size; ++i)	 ((SlStr	*)pDestData)[i] = tmp; } break;

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
	case Symbol::TYP_FLOAT :
			((SlScalar *)mpDefaultVal)[0] = ((const float *)pSrcData)[0];
			break;

	case Symbol::TYP_POINT :
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:
			((SlVec3 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec3 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec3 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case Symbol::TYP_COLOR :
			((SlColor *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlColor *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlColor *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			break;

	case Symbol::TYP_MATRIX:
			((Matrix44 *)mpDefaultVal)[0] = ((const Matrix44 *)pSrcData)[0];
			break;

	case Symbol::TYP_STRING:
			((SlStr *)mpDefaultVal)[0] = ((const SlStr *)pSrcData)[0];
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
	if ( LookupVariable( params.mpName ) )
	{
		DASSERT( 0 );
		return NULL;
	}
	Symbol	*pSym = Grow();
	pSym->mName			= params.mpName;
	pSym->mType			= params.mType;
	pSym->mStorage		= params.mStorage;
	pSym->mDetail		= params.mDetail;

	if ( pSrcData )
	{
		pSym->FillData( pSym->AllocData( 1 ), 1, pSrcData );
	}

	return pSym;
}

//==================================================================
void SymbolList::AddVoid( const char *pName )
{
	if ( LookupVariable( pName ) )
	{
		DASSERT( 0 );
		return;
	}
	Symbol	*pSym = Grow();
	pSym->mName = pName;
	pSym->mType	= Symbol::TYP_VOIDD;
}

//==================================================================
}
