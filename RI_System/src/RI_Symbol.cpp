//==================================================================
/// RI_SVM_Symbol.cpp
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
	//DASSTHROW( mpConstVal == NULL, ("Shouldn't have a default value !!!") );

	void	*pOutData = NULL;

	size_t	blocksN = DMT_SIMD_BLOCKS( size );

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:	pOutData = allocStream( sizeof(Float_)*blocksN );	break;
	case Symbol::TYP_POINT:
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:pOutData = allocStream( sizeof(Float3_)*blocksN );	break;
	case Symbol::TYP_HPOINT:pOutData = allocStream( sizeof(Float4_)*blocksN );	break;
	case Symbol::TYP_COLOR:	pOutData = allocStream( sizeof(SlColor)*blocksN );	break;
	case Symbol::TYP_MATRIX:pOutData = allocStream( sizeof(Matrix44)*size );	break;
	case Symbol::TYP_STRING:pOutData = allocStream( sizeof(SlStr)*size );		break;
	case Symbol::TYP_BOOL:	pOutData = allocStream( sizeof(VecNMask)*size );	break;

	default:
		DASSERT( 0 );
		break;
	}

	return pOutData;
}

//==================================================================
void Symbol::FreeClone( void *pData ) const
{
	if ( pData )
		freeStream( pData );
}

//==================================================================
void Symbol::CopyConstValue( void *pDestData ) const
{
	DASSERT( IsConstant() );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:	((Float_	*)pDestData)[0] = ((const Float_ *)mpConstVal)[0]; break;
	case Symbol::TYP_POINT:	
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:((Float3_	*)pDestData)[0] = ((const Float3_	*)mpConstVal)[0]; break;
	case Symbol::TYP_HPOINT:((Float4_	*)pDestData)[0] = ((const Float4_	*)mpConstVal)[0]; break;
	case Symbol::TYP_COLOR:	((SlColor	*)pDestData)[0] = ((const SlColor	*)mpConstVal)[0]; break;
	case Symbol::TYP_MATRIX:((Matrix44	*)pDestData)[0] = ((const Matrix44	*)mpConstVal)[0]; break;
	case Symbol::TYP_STRING:((SlStr		*)pDestData)[0] = ((const SlStr		*)mpConstVal)[0]; break;
	case Symbol::TYP_BOOL:	((VecNMask	*)pDestData)[0] = ((const VecNMask	*)mpConstVal)[0]; break;

	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void Symbol::InitConstValue( const void *pSrcData )
{
	DASSERT( mpConstVal == NULL );

	mpConstVal = AllocClone( 1 );

	FillDataFromSISD( mpConstVal, 1, pSrcData );	
}

//==================================================================
void Symbol::FillDataFromSISD( void *pDestData, size_t size, const void *pSrcData ) const
{
	size_t	blksN = DMT_SIMD_BLOCKS( size );

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:
		{
			const float *pFlt = (const float *)pSrcData;
			
			Float_	tmp( pFlt[0] );
			
			for (size_t i=0; i < blksN; ++i)
				((Float_	*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_POINT:	
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:
		{
			const float *pFlt = (const float *)pSrcData;

			Float3_	tmp( pFlt[0], pFlt[1], pFlt[2] );

			for (size_t i=0; i < blksN; ++i)
				((Float3_	*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_HPOINT:
		{
			const float *pFlt = (const float *)pSrcData;

			Float4_	tmp( pFlt[0], pFlt[1], pFlt[2], pFlt[3] );

			for (size_t i=0; i < blksN; ++i)
				((Float4_	*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_COLOR:
		{
			DASSERT( NCOLS == 3 );

			const float *pFlt = (const float *)pSrcData;

			SlColor	tmp( pFlt[0], pFlt[1], pFlt[2] );

			for (size_t i=0; i < blksN; ++i)
				((SlColor	*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_MATRIX:
		{ 
			Matrix44	tmp((const float *)pSrcData);
			
			for (size_t i=0; i < size; ++i)
				((Matrix44	*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_STRING:
		{
			SlStr		tmp((const SlStr *)pSrcData);
			
			for (size_t i=0; i < size; ++i)
				((SlStr		*)pDestData)[i] = tmp;
		}
		break;

	case Symbol::TYP_BOOL:
		{
			VecNMask	tmp = *(const VecNMask *)pSrcData;

			for (size_t i=0; i < size; ++i)
				((VecNMask	*)pDestData)[i] = tmp;
		}
		break;

	default:
		DASSERT( 0 );
		break;
	}
}

//==================================================================
void Symbol::FillDataFromSIMD( void *pDestData, size_t size, const void *pSrcData ) const
{
	size_t	blksN = DMT_SIMD_BLOCKS( size );

	// for now, matrix and string can only be uniform (dunno someday about matrices ?)
	DASSERT( !(mType == Symbol::TYP_MATRIX || mType == Symbol::TYP_STRING) || size == 1 );

	switch ( mType )
	{
	case Symbol::TYP_FLOAT:		for (size_t i=0; i < blksN; ++i)((Float_	*)pDestData)[i] = ((Float_	*)pSrcData)[0];	break;
	case Symbol::TYP_POINT:	
	case Symbol::TYP_VECTOR:
	case Symbol::TYP_NORMAL:	for (size_t i=0; i < blksN; ++i)((Float3_	*)pDestData)[i] = ((Float3_		*)pSrcData)[0];	break;
	case Symbol::TYP_HPOINT:	for (size_t i=0; i < blksN; ++i)((Float4_	*)pDestData)[i] = ((Float4_		*)pSrcData)[0];	break;
	case Symbol::TYP_COLOR:		for (size_t i=0; i < blksN; ++i)((SlColor	*)pDestData)[i] = ((SlColor		*)pSrcData)[0];	break;
	case Symbol::TYP_MATRIX:	for (size_t i=0; i < size; ++i)	((Matrix44	*)pDestData)[i] = ((Matrix44	*)pSrcData)[0];	break;
	case Symbol::TYP_STRING:	for (size_t i=0; i < size; ++i)	((SlStr		*)pDestData)[i] = ((SlStr		*)pSrcData)[0];	break;
	case Symbol::TYP_BOOL:		for (size_t i=0; i < size; ++i)	((VecNMask	*)pDestData)[i] = ((VecNMask	*)pSrcData)[0];	break;

	default:
		DASSERT( 0 );
		break;
	}
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
const Symbol * SymbolList::FindSymbol( const char *pName ) const
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
Symbol *SymbolList::AddByParams( const Symbol::CtorParams &params )
{
	const Symbol *pSymbol = FindSymbol( params.mpName );
	if ( pSymbol )
	{
		// allow duplicate declarations for now
		//DASSERT( 0 );
		return NULL;
	}

	Symbol	*pSym;

	mpSymbols.push_back( pSym = DNEW Symbol() );

	pSym->mName			= params.mpName;
	pSym->mType			= params.mType;
	pSym->mStorage		= params.mStorage;
	pSym->mClass		= params.mClass;

	if ( params.mpSrcData )
		pSym->InitConstValue( params.mpSrcData );

	return pSym;
}

//==================================================================
static void newSymParamsFromDecl(
						Symbol::CtorParams &out_params,
						const char *pDecl,
						std::string *out_name,
						u_int defaultClass=Symbol::CLASS_MSK_UNDEFINED )
{
	char buff[ 1024 ];
	strcpy_s( buff, pDecl );

	char *pTokCtx;
	char *pTok;

	int	typCnt = 0;
	int	detCnt = 0;
	int	namCnt = 0;

	bool	expectingName = (out_name != NULL);

	// TODO: not supporting arrays yet !!

	pTok = strtok_r( buff, " \t", &pTokCtx );
	if NOT( pTok )
		throw Exception( "Nothing to declare ?!" );

	char *pType = NULL;

	// class ..
	if ( 0 == strcmp( pTok, "uniform") ) { ++detCnt; out_params.mClass = Symbol::CLASS_MSK_UNIFORM	;	}	else
	if ( 0 == strcmp( pTok, "varying") ) { ++detCnt; out_params.mClass = Symbol::CLASS_MSK_VARYING	;	}	else
	if ( 0 == strcmp( pTok, "vertex" ) ) { ++detCnt; out_params.mClass = Symbol::CLASS_MSK_VERTEX	;	}	else
	if ( 0 == strcmp( pTok, "constant") ){ ++detCnt; out_params.mClass = Symbol::CLASS_MSK_CONSTANT	;	}	else
	{
		if ( defaultClass != Symbol::CLASS_MSK_UNDEFINED )
		{
			out_params.mClass = defaultClass;
		}

		if ( expectingName )
		{
			*out_name = pTok;

			// we got a non-terminal symbol.. then we expect nothing else to follow !
			if ( (pTok = strtok_r(NULL, " \t", &pTokCtx)) )
				throw Exception( "Bad declaration: '%s'", pDecl );

			return;
		}
		else
		{
			// this token then must be the type !
			pType = pTok;

			// if we don't have a default class, and the class is not defined by the current token
			// then we have a problem 8)
			if ( defaultClass == Symbol::CLASS_MSK_UNDEFINED )
				throw Exception( "Bad declaration, expecting symbol class in '%s'", pDecl );
		}
	}

	if NOT( pType )
	{
		pType = strtok_r(NULL, " \t", &pTokCtx);
		if NOT( pType )
			throw Exception( "Bad declaration, expecting symbol type in '%s'", pDecl );
	}

	// type ..
	if ( 0 == strcmp( pType, "float"  ) ) { ++typCnt; out_params.mType = Symbol::TYP_FLOAT	;	}	else
	if ( 0 == strcmp( pType, "point"  ) ) { ++typCnt; out_params.mType = Symbol::TYP_POINT	;	}	else
	if ( 0 == strcmp( pType, "vector" ) ) { ++typCnt; out_params.mType = Symbol::TYP_VECTOR	;	}	else
	if ( 0 == strcmp( pType, "normal" ) ) { ++typCnt; out_params.mType = Symbol::TYP_NORMAL	;	}	else
	if ( 0 == strcmp( pType, "hpoint" ) ) { ++typCnt; out_params.mType = Symbol::TYP_HPOINT	;	}	else
	if ( 0 == strcmp( pType, "color"  ) ) { ++typCnt; out_params.mType = Symbol::TYP_COLOR	;	}	else
	if ( 0 == strcmp( pType, "matrix" ) ) { ++typCnt; out_params.mType = Symbol::TYP_MATRIX	;	}	else
	if ( 0 == strcmp( pType, "string" ) ) { ++typCnt; out_params.mType = Symbol::TYP_STRING	;	}	else
	if ( 0 == strcmp( pType, "bool"   ) ) { ++typCnt; out_params.mType = Symbol::TYP_BOOL	;	}	else
	{
		throw Exception( "Bad declaration, expecting symbol type: '%s'", pDecl );
	}

	pTok = strtok_r(NULL, " \t", &pTokCtx);

	if ( expectingName )
	{
		if NOT( pTok )
			throw Exception( "Bad declaration, expecting symbol name: '%s'", pDecl );

		// found symbol name
		*out_name = pTok;

		// trailing trash ?
		if ( (pTok = strtok_r(NULL, " \t", &pTokCtx)) )
			throw Exception( "Bad declaration, what's after the symbol name ?: '%s'", pDecl );
	}
	else
	{
		if ( pTok != NULL )
			throw Exception( "Bad declaration, what's after the symbol class ?: '%s'", pDecl );
	}
}

//==================================================================
Symbol *SymbolList::Add( const char *pDecl, const char *pName, Symbol::Storage storage, const void *pSrcData, u_int defaultClass )
{
	Symbol::CtorParams	params;
	
	params.mpName	= pName;
	params.mStorage	= storage;
	params.mpSrcData= pSrcData;

	// fill up type and details.. (expect no name..)
	newSymParamsFromDecl( params, pDecl, NULL, defaultClass );

	return AddByParams( params );
}

//==================================================================
Symbol *SymbolList::AddByString( const char *pDeclName, Symbol::Storage storage, const void *pSrcData )
{
	Symbol::CtorParams	params;
	
	params.mStorage	= storage;
	params.mpSrcData= pSrcData;

	// fill up type and details.. (expects name as well)
	std::string	name;
	newSymParamsFromDecl( params, pDeclName, &name );

	params.mpName	= name.c_str();

    if ( params.mType == Symbol::TYP_UNKNOWN )
        DEX_RUNTIME_ERROR(
                "Bad symbol declaration. Type is missing: '%s'",
                pDeclName );

	return AddByParams( params );
}

//==================================================================
/// SymbolIList
//==================================================================
SymbolI * SymbolIList::FindSymbolI( const char *pName )
{
	for (size_t i=0; i < size(); ++i)
	{
		SymbolI	&inst = (*this)[i];

		if ( inst.IsName( pName ) )
		{
			return &inst;
		}
	}

	return NULL;
}

//==================================================================
const SymbolI * SymbolIList::FindSymbolI( const char *pName ) const
{
	for (size_t i=0; i < size(); ++i)
	{
		const SymbolI	&inst = (*this)[i];

		if ( inst.IsName( pName ) )
		{
			return &inst;
		}
	}

	return NULL;
}

//==================================================================
void *SymbolIList::FindSymbolIData( const char *pName )
{
	SymbolI	*pSym = FindSymbolI( pName );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
const void *SymbolIList::FindSymbolIData( const char *pName ) const
{
	const SymbolI	*pSym = FindSymbolI( pName );
	if NOT( pSym )
		return NULL;
	else
		return pSym->mpValArray;
}

//==================================================================
}
