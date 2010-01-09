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
typedef Float3_	SlColor;

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

	SlStr( const char *pFromCStr )
	{
		strcpy_s( mStr, pFromCStr );
	}

	friend bool operator ==( const SlStr &lval, const SlStr &rval ) { return 0 == strcmp( lval.mStr, rval.mStr ); }
	friend bool operator !=( const SlStr &lval, const SlStr &rval ) { return 0 != strcmp( lval.mStr, rval.mStr ); }
};

//==================================================================
class Symbol
{
public:
	enum Type
	{
		TYP_UNKNOWN,
		TYP_VOID,
		TYP_FLOAT,
		TYP_POINT,
		TYP_VECTOR,
		TYP_NORMAL,
		TYP_HPOINT,
		TYP_COLOR,
		TYP_MATRIX,
		TYP_STRING,
		TYP_BOOL,
		TYPES_N
	};

	enum Storage
	{
		STOR_PARAMETER,
		STOR_TEMPORARY,
		STOR_GLOBAL,
	};

	// Uniform = 0000b
	// Varying = 0001b
	// Vertex  = 0011b
	// Constant= 0100b
	static const u_int CLASS_MSK_UNIFORM	= 0;
	static const u_int CLASS_MSK_VARYING	= 1;
	static const u_int CLASS_MSK_VERTEX		= 3;
	static const u_int CLASS_MSK_CONSTANT	= 4;

	DStr	mName;
	Type	mType;
	Storage	mStorage;
	u_int	mClass;
	void	*mpConstVal;

public:
	//==================================================================
	struct CtorParams
	{
		const char		*mpName;
		Symbol::Type	mType;
		Symbol::Storage	mStorage;
		u_int			mClass;

		CtorParams() :
			mpName(NULL),
			mType(Symbol::TYP_UNKNOWN),
			mStorage(Symbol::STOR_TEMPORARY),
			mClass(0)
		{
		}
	};

	//==================================================================
	Symbol( const CtorParams &params )
	{
		mName		= params.mpName;
		mType		= params.mType;
		mStorage	= params.mStorage;
		mClass		= params.mClass;
		mpConstVal = NULL;
	}

	Symbol()
	{
		mType = TYP_UNKNOWN;
		mStorage = STOR_TEMPORARY;
		mClass = 0;
		mpConstVal = NULL;
	}

	~Symbol()
	{
		FreeClone( mpConstVal );
	}

	void SetVarying()			{	mClass |= CLASS_MSK_VARYING; }
	void SetUniform()			{	mClass &= ~(CLASS_MSK_VARYING | CLASS_MSK_VERTEX); }

	bool IsVarying() const		{	return !!(mClass & CLASS_MSK_VARYING);	}
	bool IsUniform() const		{	return !(mClass & CLASS_MSK_VARYING);	}

	bool IsConstant() const		{	return !!(mClass & CLASS_MSK_CONSTANT);	}

	bool IsName( const char *pSrc ) const
	{
		return 0 == strcmp( mName.c_str(), pSrc );
	}

	const char *GetNameChr() const	{	return mName.c_str(); }

	void *AllocClone( size_t size ) const;
	void FreeClone( void *pData ) const;

	void CopyConstValue( void *pDestData ) const;
	void InitConstValue( const void *pSrcData );

	const void *GetConstantData() const { return mpConstVal;	}

	void FillDataFromSISD( void *pDestData, size_t size, const void *pSrcData ) const;
	void FillDataFromSIMD( void *pDestData, size_t size, const void *pSrcData ) const;
};

//==================================================================
class SymbolList
{
	DVec<Symbol *>		mpSymbols;

public:
	SymbolList();
	~SymbolList();

	//	  Symbol *FindSymbol( const char *pName );
	const Symbol *FindSymbol( const char *pName ) const;

	Symbol *Add( const Symbol::CtorParams &params, const void *pSrcData=NULL );
	Symbol *Add( const char *pDecl, const char *pName, Symbol::Storage storage, const void *pSrcData=NULL );
	Symbol *Add( const char *pDeclName, Symbol::Storage storage, const void *pSrcData=NULL );

	Symbol *AddGlob( const char *pDeclName, const void *pSrcData=NULL )
	{
		return Add( pDeclName, Symbol::STOR_GLOBAL, pSrcData );
	}

	Symbol *AddGlob( const char *pDecl, const char *pName, const void *pSrcData=NULL )
	{
		return Add( pDecl, pName, Symbol::STOR_GLOBAL, pSrcData );
	}

	Symbol *AddGVoid( const char *pName )
	{
		Symbol::CtorParams params;
		params.mpName	= pName;
		params.mType	= Symbol::TYP_VOID;
		params.mClass	= Symbol::CLASS_MSK_CONSTANT;
		params.mStorage	= Symbol::STOR_GLOBAL;
		return Add( params );
	}

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
	const Symbol	*mpSrcSymbol;
public:
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

	const Symbol *GetSrcSymbol() const
	{
		return mpSrcSymbol;
	}

	bool IsName( const char *pSrc ) const
	{
		return 0 == strcmp( mpSrcSymbol->mName.c_str(), pSrc );
	}

	const void *GetConstantData() const
	{
		if ( mpValArray )
		{
			// an assert similar to this should happen if we decide to add a specific data
			// class for instanced symbols as well - DASSERT( mpSrcSymbol->IsConstant() );
			return mpValArray;
		}
		else
		{
			DASSERT( mpSrcSymbol->IsConstant() );
			return mpSrcSymbol->mpConstVal;
		}
	}

/*
	const void *GetUniformParamData() const
	{
		DASSERT( mpSrcSymbol->mStorage == Symbol::STOR_PARAMETER && mpSrcSymbol->IsUniform() );
		return mpSrcSymbol->mpConstVal;
	}
*/

	const void *GetData() const { DASSERT( mpValArray != NULL ); return mpValArray; }

	void *GetRWData()
	{
		DASSERT( mpValArray != NULL );
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

		  SymbolI *FindSymbolI( const char *pName );
	const SymbolI *FindSymbolI( const char *pName ) const;

		  void *FindSymbolIData( const char *pName );
	const void *FindSymbolIData( const char *pName ) const;

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
