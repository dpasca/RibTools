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
	OPRTYPE_ADDR,
	OPRTYPE_N
};

//==================================================================
static const u_int OPERANDS_VEC_MSK	= 0x00000003;

//==================================================================
enum OpBaseTypeID
{
	OBT_MOV	,
	OBT_ABS	,
	OBT_ADD	,
	OBT_SUB	,
	OBT_MUL	,
	OBT_DIV	,

	OBT_N
};

//==================================================================
enum OpCodeID
{
	// following need to be multiple of 4 and start from 0
	OP_SS_MOV	,
	OP_VS_MOV	,
	OP_VV_MOV	,

	OP_SS_ABS	,
	OP_VS_ABS	,
	OP_VV_ABS	,

	OP_SSS_ADD	,
	OP_VVS_ADD	,
	OP_VSV_ADD	,
	OP_VVV_ADD	,

	OP_SSS_SUB	,
	OP_VVS_SUB	,
	OP_VSV_SUB	,
	OP_VVV_SUB	,

	OP_SSS_MUL	,
	OP_VVS_MUL	,
	OP_VSV_MUL	,
	OP_VVV_MUL	,

	OP_SSS_DIV	,
	OP_VVS_DIV	,
	OP_VSV_DIV	,
	OP_VVV_DIV	,

	// no need for index alignment from here on
	OP_VS3_MOV	,

	OP_LDS	,
	OP_LDV	,

	OP_CMPLT	,

	OP_VV_NOISE11	,
	OP_VV_NOISE12	,
	OP_VV_NOISE13	,

	OP_VV_NOISE31	,
	OP_VV_NOISE32	,
	OP_VV_NOISE33	,

	OP_SV_XCOMP	,
	OP_SV_YCOMP	,
	OP_SV_ZCOMP	,
	OP_VS_SETXCOMP	,
	OP_VS_SETYCOMP	,
	OP_VS_SETZCOMP	,

	OP_NRM		,
	OP_FCFWD	,
	OP_DIFS		,
	OP_AMB		,
	OP_CALCNORM	,

	OP_RET		,

	OP_N
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
	u_short	mDestOpType;
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
	Type			mType;
	DStr			mShaderName;
	DVec<Symbol*>	mSymbols;
	u_int			mStartPC;
	DVec<SlCPUWord>	mCode;

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
/// SlShaderInstance
//==================================================================
class SlShaderInstance
{
	friend class SlRunContext;

	ResOwn<SlShader>	moShader;
	SlShader			*mpShader;	// direct pointer for convenience
	size_t				mMaxPointsN;

public:
	//DVec<SlParameter>	mCallingParams;
	SymbolList		mCallingParams;	// $$$ should get these from attributes and Surface params at some point !!

public:
	SlShaderInstance( size_t maxPointsN );
	~SlShaderInstance();

	void Set( SlShader *pShader )
	{
		moShader.Borrow( pShader );
		mpShader = pShader;
	}

	bool IsSet() const
	{
		return mpShader != NULL;
	}

	SlShaderInstance( const SlShaderInstance &right )
	{
		moShader.Borrow( right.moShader.Use() );
		mpShader		= right.mpShader;
		mCallingParams	= right.mCallingParams;
	}

	void operator = ( const SlShaderInstance &right )
	{
		moShader.Borrow( right.moShader.Use() );
		mpShader		= right.mpShader;
		mCallingParams	= right.mCallingParams;
	}

	void SetParameter(
				const char		*pParamName,
				Symbol::Type	type,
				bool			isVarying,
				void			*pValue );

	SlValue	*Bind( const SymbolList &gridSymbols ) const;
	void Unbind( SlValue * &pDataSegment ) const;

	void Run( class SlRunContext &ctx ) const;
};

//==================================================================
}

#endif
