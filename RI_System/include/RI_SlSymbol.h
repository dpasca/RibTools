/*
 *  RI_SlSymbol.h
 *  RibTools
 *
 *  Created by Davide Pasca on 09/02/19.
 *  Copyright 2009 Davide Pasca. All rights reserved.
 *
 */

#ifndef RI_SLSYMBOL_H
#define RI_SLSYMBOL_H

#include "RI_Base.h"

//==================================================================
namespace RI
{

//==================================================================
class SlSymbol
{
public:
	enum Type
	{
		FLOAT,
		POINT,
		COLOR,
		STRING,
		VECTOR,
		NORMAL,
		MATRIX,
	};
	
	enum Storage
	{
		CONSTANT,
		PARAMETER,
		TEMPORARY,
		GLOBAL,
	};
	
	DStr	mName;
	Type	mType;
	Storage	mStorage;
	bool	mIsVarying;
	u_int	mArraySize;
	void	*mpDefaultVal;
	
	void Reset()
	{
		mName.clear();
		mType = FLOAT;
		mStorage = CONSTANT;
		mIsVarying = false;
		mArraySize = 0;
		mpDefaultVal = NULL;
	}
};

//==================================================================
class SlSymbolList : public DVec<SlSymbol>
{
public:
	SlSymbol *LookupVariable(
			const char		*pName,
			SlSymbol::Type	type,
			bool			isVarying )
	{
		for (size_t i=0; i < size(); ++i)
		{
			SlSymbol	&symbol = (*this)[i];

			if (symbol.mType == type &&
				symbol.mIsVarying == isVarying && 
				0 == strcmp( symbol.mName.c_str(), pName ) )
			{
				// found !!!
				return &symbol;
			}
		}

		return NULL;
	}
};

//==================================================================
}

#endif
