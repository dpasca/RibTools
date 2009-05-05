//==================================================================
/// RI_SlSymbol.h
///
/// Created by Davide Pasca - 2009/2/19
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLSYMBOL_H
#define RI_SLSYMBOL_H

#include "RI_Base.h"

//==================================================================
namespace RI
{

//==================================================================
typedef Vec16f		SlScalar;
typedef Vec3x16f	SlColor;
typedef Vec3x16f	SlVector;

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
		TYPES_N
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
	
	void AllocData();
	void FreeData();
	void *AllocClone( size_t size );
	void FreeClone( void *pData );
	
	const void *GetData() const { return mpDefaultVal; }
};

//==================================================================
class SlSymbolList : public DVec<SlSymbol>
{
public:
	~SlSymbolList();

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

	SlSymbol *LookupVariable(
			const char		*pName,
			SlSymbol::Type	type )
	{
		for (size_t i=0; i < size(); ++i)
		{
			SlSymbol	&symbol = (*this)[i];

			if (symbol.mType == type &&
				0 == strcmp( symbol.mName.c_str(), pName ) )
			{
				// found !!!
				return &symbol;
			}
		}

		return NULL;
	}

	void *LookupVariableData(
			const char		*pName,
			SlSymbol::Type	type,
			bool			isVarying )
	{
		SlSymbol	*pSym = LookupVariable( pName, type, isVarying );
		if NOT( pSym )
			return NULL;
		else
			return pSym->mpDefaultVal;
	}

	void *LookupVariableData(
			const char		*pName,
			SlSymbol::Type	type )
	{
		SlSymbol	*pSym = LookupVariable( pName, type );
		if NOT( pSym )
			return NULL;
		else
			return pSym->mpDefaultVal;
	}
};

//==================================================================
}

#endif
