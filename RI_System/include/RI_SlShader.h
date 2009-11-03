//==================================================================
/// RI_SlShader.h
///
/// Created by Davide Pasca - 2009/2/19
/// See the file "license.txt" that comes with this project for
/// copyright info.
//==================================================================

#ifndef RI_SLSHADER_H
#define RI_SLSHADER_H

#include "RI_Base.h"
#include "RI_Symbol.h"
#include "RI_Resource.h"
#include "RI_FileManager.h"

//==================================================================
namespace RI
{

//==================================================================
enum OperTypeID
{
	OPRTYPE_NA,
	OPRTYPE_F1 ,
	OPRTYPE_F2 ,
	OPRTYPE_F3 ,
	OPRTYPE_F4 ,
	OPRTYPE_M44,
	OPRTYPE_STR,
	OPRTYPE_ADDR,
	OPRTYPE_N
};

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

	OBT_N
};

//==================================================================
enum OpCodeID
{
	OP_RET
};

//==================================================================
class SlValue
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

	SlValue()
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

	void SetDataRW( void *pData, const Symbol *pSrcSymbol )
	{
		Flags.mCanChange	= 1;
		Data.pVoidValue		= pData;
		mpSrcSymbol			= pSrcSymbol;
	}
};

//==================================================================
struct SlOpCode
{
	u_int	mTableOffset;
	u_short	mOperandCount;
	u_short	mFuncopEndAddr;
	u_int	mDbgLineNum;
};

//==================================================================
struct SlSymbolWord
{
	u_int		mTableOffset;
	bool		mIsVarying;
	Symbol	*mpOrigSymbol;
};

//==================================================================
struct SlAddress
{
	u_int	mOffset;
};

//==================================================================
struct SlImmFloat
{
	float	mValue;
};

//==================================================================
union SlCPUWord
{
	SlOpCode		mOpCode;
	SlSymbolWord	mSymbol;
	SlAddress		mAddress;
	SlImmFloat		mImmFloat;
};

//==================================================================
/// SlShader
//==================================================================
class SlShader : public ResourceBase
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
	DVec<SlCPUWord>		mCode;

	struct CtorParams
	{
		const char	*pName;
		const char	*pSource;
		const char	*pSourceFileName;
		const char	*pAppResDir;

		CtorParams() :
			pName(NULL),
			pSource(NULL),
			pSourceFileName(NULL),
			pAppResDir(NULL)
		{
		}
	};

	SlShader( const CtorParams &params, FileManagerBase &fileManager );
};

//==================================================================
/// SlShaderInst
//==================================================================
class SlShaderInst : public RCBase
{
	friend class SlRunContext;

	RCOwn<SlShader>		moShader;
	size_t				mMaxPointsN;

public:
	SymbolList			mCallSymList;
	SymbolIList			mCallSymIList;

public:
	SlShaderInst( SlShader *pShader, size_t maxPointsN=MP_GRID_MAX_SIZE );
	~SlShaderInst();

	SlShaderInst( const SlShaderInst &right )
	{
		moShader.Borrow( right.moShader.Use() );
		//mCallSymIList	= right.mCallSymIList;
	}

	void operator = ( const SlShaderInst &right )
	{
		moShader.Borrow( right.moShader.Use() );
		//mCallSymIList	= right.mCallSymIList;
	}

	SlValue	*Bind(
			const SymbolList	&globalSyms,
			SymbolIList			&gridSymIList,
			DVec<u_int>			&out_defParamValsStartPCs ) const;

	void Unbind( SlValue * &pDataSegment ) const;

	void Run( class SlRunContext &ctx ) const;

private:
	void runFrom( class SlRunContext &ctx, u_int startPC ) const;
};

//==================================================================
}

#endif
