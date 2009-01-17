/*
 *  RI_Symbol.cpp
 *  RibTools
 *
 *  Created by Davide Pasca on 09/01/16.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_SYMBOL_H
#define RI_SYMBOL_H

#include "RI_Base.h"

//==================================================================
namespace RI
{

//==================================================================
struct SymbolBase	{	const char *pName;					};
struct SymbolVoid	: public SymbolBase {					};
struct SymbolFloat	: public SymbolBase { float		value;	};
struct SymbolInt	: public SymbolBase { int		value;	};
struct SymbolPoint	: public SymbolBase { RtPoint	value;	};
struct SymbolColor	: public SymbolBase { RtColor	value;	};
struct SymbolBasis	: public SymbolBase { RtBasis	value;	};

//==================================================================
typedef const SymbolBase*	CPSymBase;
typedef const SymbolVoid*	CPSymVoid;
typedef const SymbolFloat*	CPSymFloat;
typedef const SymbolInt*	CPSymInt;
typedef const SymbolPoint*	CPSymPoint;
typedef const SymbolColor*	CPSymColor;
typedef const SymbolBasis*	CPSymBasis;

//==================================================================
class SymbolList
{
	DVec<SymbolVoid>	mVoidLst	;
	DVec<SymbolFloat>	mFloatLst	;
	DVec<SymbolInt>		mIntLst		;
	DVec<SymbolPoint>	mPointLst	;
	DVec<SymbolColor>	mColorLst	;
	DVec<SymbolBasis>	mBasisLst	;

public:
	enum Type {
		T_VOID,
		T_FLOAT,
		T_INT,
		T_POINT,
		T_COLOR,
		T_BASIS,
	};

public:
	SymbolList(){}

	void FindOrAdd( const char *pName, const char *pDescr=NULL );
	void FindOrAdd( const char *pName, const RtBasis &basis );

	const SymbolBase  *Find( Type type, const char *pName ) const;
	const SymbolVoid  *FindVoid(  const char *pName ) const { return (const SymbolVoid *)Find( T_VOID, pName );	}
	const SymbolBasis *FindBasis( const char *pName ) const { return (const SymbolBasis *)Find( T_BASIS, pName );	}
};

//==================================================================
}

#endif
