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
#if defined(_MSC_VER)
typedef __declspec(align(64)) VecSIMDf		SlScalar;
typedef __declspec(align(64)) Vec3xSIMDf	SlColor;
typedef __declspec(align(64)) Vec2xSIMDf	SlVec2;
typedef __declspec(align(64)) Vec3xSIMDf	SlVec3;
typedef __declspec(align(64)) Vec4xSIMDf	SlVec4;

#elif defined(__GNUC__)
typedef VecSIMDf	SlScalar	__attribute__ ((aligned(64)));
typedef Vec3xSIMDf	SlColor		__attribute__ ((aligned(64)));
typedef Vec2xSIMDf	SlVec2		__attribute__ ((aligned(64)));
typedef Vec3xSIMDf	SlVec3		__attribute__ ((aligned(64)));
typedef Vec4xSIMDf	SlVec4		__attribute__ ((aligned(64)));

#endif

//==================================================================
class SlSymbol
{
public:
	enum Type
	{
		UNKNOWN,
		VOIDD,
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
		mType = UNKNOWN;
		mStorage = CONSTANT;
		mIsVarying = false;
		mArraySize = 0;
		mpValArray = NULL;
		mpDefaultVal = NULL;
	}

	SlSymbol()
	{
		mType = UNKNOWN;
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

	bool IsNameI( const char *pSrc ) const
	{
		return 0 == strcasecmp( mName.c_str(), pSrc );
	}

	void *AllocData( size_t size );
	void AllocDefault( const void *pSrcData );

	void *AllocClone( size_t size ) const;
	void FreeClone( void *pData ) const;

	void FillDataWithDefault( void *pDestData, size_t size ) const;
	void FillData( void *pDestData, size_t size, const void *pSrcData ) const;

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
	DVec<SlSymbol> mpSymbols;

public:
	struct SymbolParams
	{
		const char			*mpName;
		SlSymbol::Type		mType;
		SlSymbol::Storage	mStorage;
		bool				mIsVarying;

		SymbolParams() :
			mpName(NULL),
			mType(SlSymbol::UNKNOWN),
			mStorage(SlSymbol::CONSTANT),
			mIsVarying(false)
		{
		}
	};

public:
	SlSymbolList();
	~SlSymbolList();

	SlSymbolList( const SlSymbolList &from );
	void operator = ( const SlSymbolList &from );

		  SlSymbol *LookupVariable( const char *pName );
	const SlSymbol *LookupVariable( const char *pName ) const;
	const SlSymbol *LookupVariableI( const char *pName ) const;

		  SlSymbol *LookupVariable( const char *pName, SlSymbol::Type type );
	const SlSymbol *LookupVariable( const char *pName, SlSymbol::Type type ) const ;

	//	  SlSymbol *LookupVoid( const char *pName, SlSymbol::Type type );
	const SlSymbol *LookupVoid( const char *pName ) const { return LookupVariable( pName, SlSymbol::VOIDD ); }

	void *LookupVariableData( const char *pName, SlSymbol::Type type );
	const void *LookupVariableData( const char *pName, SlSymbol::Type type ) const;

	SlSymbol *Grow()
	{
		return mpSymbols.grow();
	}

	SlSymbol *Add( const SymbolParams &params, const void *pSrcData=NULL );

	void AddVoid( const char *pName );

	size_t size() const
	{
		return mpSymbols.size();
	}

	const SlSymbol &operator [] (size_t i) const{ return mpSymbols[i]; }
		  SlSymbol &operator [] (size_t i)		{ return mpSymbols[i]; }
};

//==================================================================
}

#endif
