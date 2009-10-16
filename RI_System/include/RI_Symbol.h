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
class SlStr
{
	static const size_t	MAX_SIZE = 256;

public:
	char	mStr[MAX_SIZE];

	SlStr()
	{
		mStr[0] = 0;
	}

	SlStr( const SlStr *pFrom )
	{
		strcpy_s( mStr, pFrom->mStr );
	}
};

//==================================================================
class Symbol
{
public:
	enum Type
	{
		TYP_UNKNOWN,
		TYP_VOIDD,
		TYP_FLOAT,
		TYP_POINT,
		TYP_VECTOR,
		TYP_NORMAL,
		TYP_HPOINT,
		TYP_COLOR,
		TYP_STRING,
		TYP_MATRIX,
		TYPES_N
	};

	enum Storage
	{
		STOR_CONSTANT,
		STOR_PARAMETER,
		STOR_TEMPORARY,
		STOR_GLOBAL,
	};

	// Uniform = 0000b
	// Varying = 0001b
	// Vertex  = 0011b
	// Constant= 0100b
	static const u_int DET_MSK_UNIFORM	= 0;
	static const u_int DET_MSK_VARYING	= 1;
	static const u_int DET_MSK_VERTEX	= 3;
	static const u_int DET_MSK_CONSTANT	= 4;

	DStr	mName;
	Type	mType;
	Storage	mStorage;
	u_int	mDetail;
	void	*mpDefaultVal;

public:
	//==================================================================
	struct CtorParams
	{
		const char		*mpName;
		Symbol::Type	mType;
		Symbol::Storage	mStorage;
		u_int			mDetail;

		CtorParams() :
			mpName(NULL),
			mType(Symbol::TYP_UNKNOWN),
			mStorage(Symbol::STOR_CONSTANT),
			mDetail(0)
		{
		}
	};

	//==================================================================
	Symbol( const CtorParams &params )
	{
		mName		= params.mpName;
		mType		= params.mType;
		mStorage	= params.mStorage;
		mDetail		= params.mDetail;
		mpDefaultVal = NULL;
	}

	Symbol()
	{
		mType = TYP_UNKNOWN;
		mStorage = STOR_CONSTANT;
		mDetail = 0;
		mpDefaultVal = NULL;
	}

	~Symbol()
	{
		FreeClone( mpDefaultVal );
	}

	void Reset()
	{
		mName.clear();
		mType = TYP_UNKNOWN;
		mStorage = STOR_CONSTANT;
		mDetail = 0;
		mpDefaultVal = NULL;
	}

	void SetVarying()			{	mDetail |= DET_MSK_VARYING; }
	void SetUniform()			{	mDetail &= ~(DET_MSK_VARYING | DET_MSK_VERTEX); }

	bool IsVarying() const		{	return !!(mDetail & DET_MSK_VARYING);	}
	bool IsUniform() const		{	return !(mDetail & DET_MSK_VARYING);	}

	bool IsName( const char *pSrc ) const
	{
		return 0 == strcmp( mName.c_str(), pSrc );
	}

	//void *AllocData( size_t size );
	void AllocDefault( const void *pSrcData );

	void *AllocClone( size_t size ) const;
	void FreeClone( void *pData ) const;

	void FillDataWithDefault( void *pDestData, size_t size ) const;
	void FillData( void *pDestData, size_t size, const void *pSrcData ) const;

	const void *GetConstantData() const { return mpDefaultVal;	}
};

//==================================================================
class SymbolList
{
	DVec<Symbol *>		mpSymbols;

public:
	SymbolList();
	~SymbolList();

	//	  Symbol *LookupVariable( const char *pName );
	const Symbol *LookupVariable( const char *pName ) const;

	Symbol *Add( const Symbol::CtorParams &params, const void *pSrcData=NULL );
	Symbol *Add( const char *pName, const char *pDecl, const void *pSrcData=NULL );
	Symbol *Add( const char *pName );

	size_t size() const
	{
		return mpSymbols.size();
	}

	const Symbol &operator [] (size_t i) const	{ return *mpSymbols[i]; }
		  Symbol &operator [] (size_t i)		{ return *mpSymbols[i]; }

private:
	SymbolList( const SymbolList &from );
	void operator = ( const SymbolList &from );
};

//==================================================================
class SymbolI
{
public:
	const Symbol	*mpSrcSymbol;
	size_t			mArraySize;
	void			*mpValArray;
	//u_int			mDefaultValStartPC;

	SymbolI( const Symbol &symbol, size_t varyingAllocN ) :
		mpSrcSymbol(&symbol),
		mArraySize(0),
		mpValArray(NULL)
		//, mDefaultValStartPC(INVALID_PC)
	{
		mpValArray = 
			mpSrcSymbol->AllocClone(
					mpSrcSymbol->IsVarying() ? varyingAllocN : 1 );
	}

	~SymbolI()
	{
		mpSrcSymbol->FreeClone( mpValArray );
	}

	bool IsVarying() const	{	return mpSrcSymbol->IsVarying();	}

	const void *GetConstantData() const
	{
		DASSERT( mpSrcSymbol->mStorage == Symbol::STOR_CONSTANT && mpSrcSymbol->IsUniform() );
		return mpSrcSymbol->mpDefaultVal;
	}

	const void *GetUniformParamData() const
	{
		DASSERT( mpSrcSymbol->mStorage == Symbol::STOR_PARAMETER && mpSrcSymbol->IsUniform() );
		return mpSrcSymbol->mpDefaultVal;
	}

	const void *GetData() const { return mpValArray; }

	void *GetRWData()
	{
		DASSERT( mpValArray != NULL ); // IsVarying()
		return mpValArray;
	}
};

//==================================================================
class SymbolIList
{
	DVec<SymbolI *>	mpList;

public:
	SymbolIList()
	{
	}

	SymbolIList( const SymbolIList &from )
	{
	}

	~SymbolIList()
	{
		for (size_t i=0; i < mpList.size(); ++i)
		{
			DSAFE_DELETE( mpList[i] );
		}
	}

		  SymbolI *LookupVariable( const char *pName );
	const SymbolI *LookupVariable( const char *pName ) const;

		  void *LookupVariableData( const char *pName );
	const void *LookupVariableData( const char *pName ) const;

	SymbolI *AddInstance( const Symbol &srcSymbol, size_t varyingAllocN )
	{
		SymbolI	*pSymI = DNEW SymbolI( srcSymbol, varyingAllocN );

		mpList.push_back( pSymI );

		return pSymI;
	}

	size_t size() const
	{
		return mpList.size();
	}

	const SymbolI &operator [] (size_t i) const	{ return *mpList[i]; }
		  SymbolI &operator [] (size_t i)		{ return *mpList[i]; }

private:

	void operator = ( const SymbolIList &from )
	{
	}
};

//==================================================================
}

#endif
