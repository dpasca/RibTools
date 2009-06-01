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
typedef __declspec(align(64)) VecSIMDf		SlScalar;
typedef __declspec(align(64)) Vec3xSIMDf	SlColor;
typedef __declspec(align(64)) Vec2xSIMDf	SlVec2;
typedef __declspec(align(64)) Vec3xSIMDf	SlVec3;
typedef __declspec(align(64)) Vec4xSIMDf	SlVec4;

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
	void	*mpValArray;
	void	*mpDefaultVal;

	void Reset()
	{
		mName.clear();
		mType = FLOAT;
		mStorage = CONSTANT;
		mIsVarying = false;
		mArraySize = 0;
		mpValArray = NULL;
		mpDefaultVal = NULL;
	}

	SlSymbol()
	{
		mType = FLOAT;
		mStorage = CONSTANT;
		mIsVarying = false;
		mArraySize = 0;
		mpValArray = NULL;
		mpDefaultVal = NULL;
	}

	~SlSymbol()
	{
		FreeClone( mpValArray );
		FreeClone( mpDefaultVal );
	}

	void *AllocData( size_t size );
	void AllocDefault( const void *pSrcData );

	void *AllocClone( size_t size ) const;
	void FreeClone( void *pData ) const;

	void FillDataWithDefault( void *pDestData, size_t size ) const;
	//void FillOwnDataWithDefault();

	const void *GetData() const { return mpValArray; }

	const void *GetConstantData() const
	{
		DASSERT( mStorage == CONSTANT && mIsVarying == false && mpDefaultVal != NULL );
		return mpDefaultVal;
	}

	const void *GetUniformParamData() const
	{
		DASSERT( mStorage == PARAMETER && mIsVarying == false && mpDefaultVal != NULL );
		return mpDefaultVal;
	}

	void *GetRWData()
	{
		DASSERT( mIsVarying == true && mpValArray != NULL );
		return mpValArray;
	}
};

//==================================================================
class SlSymbolList
{
	SlSymbol	*mpSymbols;
	size_t		mSymbolsN;
	size_t		mMaxSymbols;

public:
	SlSymbolList( size_t maxSymbols );
	~SlSymbolList();

	SlSymbolList( const SlSymbolList &from );
	void operator = ( const SlSymbolList &from );

		  SlSymbol *LookupVariable( const char *pName );
	const SlSymbol *LookupVariable( const char *pName ) const;

		  SlSymbol *LookupVariable( const char *pName, SlSymbol::Type type );
	const SlSymbol *LookupVariable( const char *pName, SlSymbol::Type type ) const ;

	void *LookupVariableData( const char *pName, SlSymbol::Type type );
	const void *LookupVariableData( const char *pName, SlSymbol::Type type ) const;

	SlSymbol *Grow()
	{
		DASSTHROW( mSymbolsN < mMaxSymbols, ("Out of bounds !") );

		return &mpSymbols[ mSymbolsN++ ];
	}

	size_t size() const
	{
		return mSymbolsN;
	}

	const SlSymbol &operator [] (size_t i) const{ DASSERT( i < mSymbolsN ); return mpSymbols[i]; }
		  SlSymbol &operator [] (size_t i)		{ DASSERT( i < mSymbolsN ); return mpSymbols[i]; }
};

//==================================================================
}

#endif
