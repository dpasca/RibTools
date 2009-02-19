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
	
	RtToken	mpToken;
	Type	mType;
	Storage	mStorage;
	bool	mIsVarying;
	u_int	mArraySize;
	void	*mpDefaultVal;
};

//==================================================================
class SlSymbolList : public DVec<SlSymbol>
{
public:
	void *LookupVariable(
			const char		*pName,
			SlSymbol::Type	type,
			bool			isVarying )
	{
		for (size_t i=0; i < size(); ++i)
		{
			SlSymbol	&symbol = (*this)[i];

			if (symbol.mType == type &&
				symbol.mIsVarying == isVarying && 
				0 == strcmp( symbol.mpToken, pName ) )
			{
				// found !!!
				return NULL;
			}
		}

		return NULL;
	}
};

//==================================================================
}

#endif
