/*
 *  RI_Symbol.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/16.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#include "RI_Base.h"
#include "RI_Symbol.h"

//==================================================================
namespace RI
{

//==================================================================
template <class T>
size_t findIdxByName( const DVec<T> &vec, const char *pName )
{
	for (size_t i=0; i < vec.size(); ++i)
		if ( 0 == strcmp( pName, vec[i].pName ) )
			return i;

	return NPOS;
}

//==================================================================
template <class T>
const T *findElemByName( const DVec<T> &vec, const char *pName )
{
	for (size_t i=0; i < vec.size(); ++i)
		if ( 0 == strcmp( pName, vec[i].pName ) )
			return &vec[i];

	return NULL;
}

//==================================================================
void SymbolList::FindOrAdd( const char *pName, const char *pDescr )
{
	if NOT( pDescr )
	{
		if ( NPOS == findIdxByName( mVoidLst, pName ) )
		{
			SymbolVoid	*pNew = mVoidLst.grow();
			pNew->pName = pName;
		}
	}
}

//==================================================================
void SymbolList::FindOrAdd( const char *pName, const RtBasis &basis )
{
	SymbolBasis	*pSym = NULL;

	size_t	idx = findIdxByName( mBasisLst, pName );

	if ( NPOS == idx )
	{
		pSym = mBasisLst.grow();
		pSym->pName = pName;
	}

	pSym->value = basis;
}

//==================================================================
const SymbolBase *SymbolList::Find( Type type, const char *pName ) const
{
	size_t	idx;

	switch ( type )
	{
	case	T_VOID:		return (const SymbolBase *)findElemByName( mVoidLst	, pName );
	case	T_FLOAT:	return (const SymbolBase *)findElemByName( mFloatLst, pName );
	case	T_INT:		return (const SymbolBase *)findElemByName( mIntLst	, pName );
	case	T_POINT:	return (const SymbolBase *)findElemByName( mPointLst, pName );
	case	T_COLOR:	return (const SymbolBase *)findElemByName( mColorLst, pName );
	case	T_BASIS:	return (const SymbolBase *)findElemByName( mBasisLst, pName );
	}

	DASSERT( 0 );
	return NULL;
}

//==================================================================
}