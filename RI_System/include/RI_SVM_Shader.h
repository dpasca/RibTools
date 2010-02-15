//==================================================================
/// RI_SVM_Shader.h
///
/// Created by Davide Pasca - 2009/2/19
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef RI_SVM_SHADER_H
#define RI_SVM_SHADER_H

#include "RI_Base.h"
#include "RI_Symbol.h"
#include "RI_Resource.h"
#include "DSystem/include/DIO_FileManager.h"

//==================================================================
namespace RI
{

//==================================================================
namespace RRASM
{
	struct OpCodeDef;
}

//==================================================================
namespace SVM
{

//==================================================================
enum OpBaseTypeID
{
	OBT_MOV	,
	OBT_ABS	,
	OBT_SIGN,
	OBT_ADD	,
	OBT_SUB	,
	OBT_MUL	,
	OBT_DIV	,

	OBT_SETLE	,
	OBT_SETGE	,
	OBT_SETLT	,
	OBT_SETGT	,
	OBT_SETEQ	,
	OBT_SETNEQ	,

	OBT_MIN	,
	OBT_MAX	,

	OBT_N
};

//==================================================================
enum OpCodeID
{
	OP_RET
};

//==================================================================
class Value
{
public:
	struct
	{
	   unsigned mOwnData	: 1;
	   unsigned mCanChange	: 1;
	} Flags;

	union
	{
		void		*pVoidValue;
		const void	*pConstVoidValue;
	} Data;

	const Symbol	*mpSrcSymbol;

	Value()
	{
		Flags.mOwnData = 0;
		Flags.mCanChange = 0;
		Data.pVoidValue = NULL;
		mpSrcSymbol = NULL;
	}

	void SetDataR( const void *pData, const Symbol *pSrcSymbol )
	{
		Flags.mCanChange		= 0;
		Data.pConstVoidValue	= pData;
		mpSrcSymbol				= pSrcSymbol;
	}

	void AllocFillConstData( const Symbol *pSrcSymbol, size_t samplesN, const void *pSrcConstData )
	{
		void *pVaryingData = pSrcSymbol->AllocClone( samplesN );

		Flags.mOwnData = 1;
		SetDataR( pVaryingData, pSrcSymbol );
		pSrcSymbol->FillDataFromSIMD( pVaryingData, samplesN, pSrcConstData );
	}

	void SetDataRW( void *pData, const Symbol *pSrcSymbol )
	{
		Flags.mCanChange	= 1;
		Data.pVoidValue		= pData;
		mpSrcSymbol			= pSrcSymbol;
	}
};

//==================================================================
struct OpCode
{
	static const u_short	INVALID_ADDR = 0xffff;

	u_int	mTableOffset;	// Instruction index in the instruction table
	u_short	mOperandCount;	// How many operands ?
	u_short	mFuncopEndAddr;	// end of if/else, solar, etc, address
	u_int	mDbgLineNum;	// what line in the RRASM code ?
};

//==================================================================
struct SymbolWord
{
	u_int	mTableOffset;
	bool	mIsVarying;
	Symbol	*mpOrigSymbol;
};

//==================================================================
struct Address
{
	u_int	mOffset;
};

//==================================================================
struct ImmFloat
{
	float	mValue;
};

//==================================================================
union CPUWord
{
	OpCode		mOpCode;
	SymbolWord	mSymbol;
	Address		mAddress;
	ImmFloat	mImmFloat;
};

//==================================================================
/// Shader
//==================================================================
class Shader : public ResourceBase
{
public:
	enum Type
	{
		TYPE_UNKNOWN,
		TYPE_LIGHT,
		TYPE_SURFACE,
		TYPE_VOLUME,
		TYPE_DISPLACEMENT,
		TYPE_TRANSFORMATION,
		TYPE_IMAGER,
		TYPE_N
	};
public:
	Type				mType;
	DStr				mShaderName;
	DVec<Symbol	*>		mpShaSyms;
	DVec<u_int>			mpShaSymsStartPCs;
	u_int				mStartPC;
	DVec<CPUWord>		mCode;
	bool				mHasDirPosInstructions;

	struct CtorParams
	{
		const char	*pName;
		const char	*pSource;
		const char	*pSourceFileName;
		const char	*pBaseIncDir;

		CtorParams() :
			pName(NULL),
			pSource(NULL),
			pSourceFileName(NULL),
			pBaseIncDir(NULL)
		{
		}
	};

	Shader( const CtorParams &params, DIO::FileManagerBase &fileManager );
};

//==================================================================
/// ShaderInst
//==================================================================
class ShaderInst : public RCBase
{
	friend class Context;

	RCOwn<Shader>		moShader;
	size_t				mMaxPointsN;

public:
	SymbolList			mCallSymList;
	SymbolIList			mCallSymIList;

public:
	ShaderInst( Shader *pShader, size_t maxPointsN=MP_GRID_MAX_SIZE );
	~ShaderInst();

	ShaderInst( const ShaderInst &right )
	{
		DASSERT( right.moShader.Use() != NULL );
		moShader.Borrow( right.moShader );
		//mCallSymIList	= right.mCallSymIList;
	}

	void operator = ( const ShaderInst &right )
	{
		DASSERT( right.moShader.Use() != NULL );
		moShader.Borrow( right.moShader );
		//mCallSymIList	= right.mCallSymIList;
	}

	Value	*Bind(
			const SymbolList	&globalSyms,
			SymbolIList			&gridSymIList,
			DVec<u_int>			&out_defParamValsStartPCs ) const;

	void Unbind( Value * &pDataSegment ) const;

	void Run( class Context &ctx ) const;

private:
	bool verifyOpParams( Context &ctx, const RRASM::OpCodeDef &opCodeDef ) const;
	void runFrom( class Context &ctx, u_int startPC ) const;
};

//==================================================================
}
//==================================================================
}

#endif
