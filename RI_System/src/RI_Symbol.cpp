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
	case Symbol::TYP_HPOINT:pOutData = allocStream( sizeof(SlVec4)*blocksN );	break;
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
	case Symbol::TYP_HPOINT:for (size_t i=0; i < blocksN; ++i) ((SlVec4	  *)pDestData)[i] = ((const SlVec4   *)mpDefaultVal)[0]; break;
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
	case Symbol::TYP_HPOINT:{ SlVec4	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlVec4	*)pDestData)[i] = tmp; } break;
	case Symbol::TYP_COLOR:	{ SlColor	tmp((const float *)pSrcData); for (size_t i=0; i < blocksN; ++i) ((SlColor	*)pDestData)[i] = tmp; } break;
	case Symbol::TYP_MATRIX:{ Matrix44	tmp((const float *)pSrcData); for (size_t i=0; i < size; ++i)	 ((Matrix44 *)pDestData)[i] = tmp; } break;
	case Symbol::TYP_STRING:{ SlStr		tmp((const SlStr *)pSrcData); for (size_t i=0; i < size; ++i)	 ((SlStr	*)pDestData)[i] = tmp; } break;

	default:
		DASSERT( 0 );
		break;
	}
}

/*
//==================================================================
void *Symbol::AllocData( size_t size )
{
	DASSTHROW( mpValArray == NULL && mpDefaultVal == NULL, ("Already allocated !!!") );

	mArraySize = size;
	mpValArray = AllocClone( mArraySize );
	return mpValArray;
}
*/

//==================================================================
void Symbol::AllocDefault( const void *pSrcData )
{
	DASSTHROW( mpDefaultVal == NULL, ("Already allocated !!!") );

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

	case Symbol::TYP_HPOINT:
			((SlVec4 *)mpDefaultVal)[0][0] = ((const float *)pSrcData)[0];
			((SlVec4 *)mpDefaultVal)[0][1] = ((const float *)pSrcData)[1];
			((SlVec4 *)mpDefaultVal)[0][2] = ((const float *)pSrcData)[2];
			((SlVec4 *)mpDefaultVal)[0][3] = ((const float *)pSrcData)[3];
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
	for (size_t i=0; i < mpSymbols.size(); ++i)
	{
		DSAFE_DELETE( mpSymbols[i] );
	}
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
Symbol *SymbolList::Add( const Symbol::CtorParams &params, const void *pSrcData )
{
	if ( LookupVariable( params.mpName ) )
	{
		DASSERT( 0 );
		return NULL;
	}

	Symbol	*pSym;

	mpSymbols.push_back( pSym = DNEW Symbol() );

	pSym->mName			= params.mpName;
	pSym->mType			= params.mType;
	pSym->mStorage		= params.mStorage;
	pSym->mDetail		= params.mDetail;

	if ( pSrcData )
	{
		pSym->mpDefaultVal = pSym->AllocClone( 1 );

		pSym->FillData( pSym->mpDefaultVal, 1, pSrcData );
	}

	return pSym;
}

//==================================================================
Symbol *SymbolList::Add( const char *pName )
{
	Symbol::CtorParams	params;

	params.mpName	= pName;
	params.mStorage	= Symbol::STOR_GLOBAL;	// this is decided here.. for now
	params.mType	= Symbol::TYP_VOIDD;
	params.mDetail	= Symbol::DET_MSK_CONSTANT;

	return Add( params );
}

//==================================================================
static void newSymParamsFromDecl( Symbol::CtorParams &out_params, const char *pDecl )
{
	char buff[ 1024 ];
	strcpy_s( buff, pDecl );

	char *pTokCtx;
	char *pTok;

	int	typCnt = 0;
	int	detCnt = 0;

	// TODO: not supporting arrays yet !!
	if ( pTok = strtok_r( buff, " \t", &pTokCtx ) )
	{
		do
		{
			// type ..
			if ( 0 == strcmp( pTok, "float"	 ) ) { ++typCnt; out_params.mType = Symbol::TYP_FLOAT	;	}	else
			if ( 0 == strcmp( pTok, "point"	 ) ) { ++typCnt; out_params.mType = Symbol::TYP_POINT	;	}	else
			if ( 0 == strcmp( pTok, "vector" ) ) { ++typCnt; out_params.mType = Symbol::TYP_VECTOR	;	}	else
			if ( 0 == strcmp( pTok, "normal" ) ) { ++typCnt; out_params.mType = Symbol::TYP_NORMAL	;	}	else
			if ( 0 == strcmp( pTok, "hpoint" ) ) { ++typCnt; out_params.mType = Symbol::TYP_HPOINT	;	}	else
			if ( 0 == strcmp( pTok, "color"	 ) ) { ++typCnt; out_params.mType = Symbol::TYP_COLOR	;	}	else
			if ( 0 == strcmp( pTok, "string" ) ) { ++typCnt; out_params.mType = Symbol::TYP_STRING	;	}	else
			if ( 0 == strcmp( pTok, "matrix" ) ) { ++typCnt; out_params.mType = Symbol::TYP_MATRIX	;	}	else
			// storage ..
			if ( 0 == strcmp( pTok, "uniform") ) { ++detCnt; out_params.mDetail = Symbol::DET_MSK_UNIFORM	;	}	else
			if ( 0 == strcmp( pTok, "varying") ) { ++detCnt; out_params.mDetail = Symbol::DET_MSK_VARYING	;	}	else
			if ( 0 == strcmp( pTok, "vertex" ) ) { ++detCnt; out_params.mDetail = Symbol::DET_MSK_VERTEX	;	}	else
			if ( 0 == strcmp( pTok, "constant") ){ ++detCnt; out_params.mDetail = Symbol::DET_MSK_CONSTANT	;	}

		} while ( pTok = strtok_r(NULL, " \t", &pTokCtx) );
	}

	if ( typCnt == 0 )	{ DASSTHROW( 0, ("Bad declaration, missing type") );	} else
	if ( typCnt >  1 )	{ DASSTHROW( 0, ("Bad declaration, multiple types") );	}

	if ( detCnt == 0 )	{ DASSTHROW( 0, ("Bad declaration, missing storage indentifier") );		} else
	if ( detCnt >  1 )	{ DASSTHROW( 0, ("Bad declaration, multiple storage indentifiers") );	}
}

//==================================================================
Symbol *SymbolList::Add( const char *pName, const char *pDecl, const void *pSrcData )
{
	Symbol::CtorParams	params;
	
	params.mpName	= pName;
	params.mStorage	= Symbol::STOR_GLOBAL;	// this is decided here.. for now

	// fill up type and details..
	newSymParamsFromDecl( params, pDecl );

	return Add( params, pSrcData );
}

//==================================================================
/// SymbolIList
//==================================================================
SymbolI * SymbolIList::LookupVariable( const char *pName )
{
	for (size_t i=0; i < size(); ++i)
	{
		SymbolI	&inst = (*this)[i];

		if ( 0 == strcmp( inst.mpSrcSymbol->mName.c_str(), pName ) )
		{
			return &inst;
		}
	}

	return NULL;
}

//==================================================================
const SymbolI * SymbolIList::LookupVariable( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const SymbolI	&inst = (*this)[i];

		if ( 0 == strcmp( inst.mpSrcSymbol->mName.c_str(), pName ) )
		{
			return &inst;
		}
	}

	return NULL;
}

//==================================================================
void *SymbolIList::LookupVariableData( const char *pName )
{
	SymbolI	*pSym = LookupVariable( pName );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
const void *SymbolIList::LookupVariableData( const char *pName ) const
{
	const SymbolI	*pSym = LookupVariable( pName );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
}
