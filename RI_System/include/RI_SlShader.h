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
#include "RI_SlSymbol.h"
#include "RI_Resource.h"

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
	OPRTYPE_N
};

//==================================================================
static const u_int OPERANDS_VEC_MSK	= 0x00000003;

//==================================================================
enum OpCodeID
{
	OP_SS_MOV	,
	OP_SV_MOV	,
	OP_VS_MOV	,
	OP_VV_MOV	,

	OP_SS_ABS	,
	OP_SV_ABS	,
	OP_VS_ABS	,
	OP_VV_ABS	,

	OP_SS_ADD	,
	OP_SV_ADD	,
	OP_VS_ADD	,
	OP_VV_ADD	,

	OP_SS_SUB	,
	OP_SV_SUB	,
	OP_VS_SUB	,
	OP_VV_SUB	,

	OP_SS_MUL	,
	OP_SV_MUL	,
	OP_VS_MUL	,
	OP_VV_MUL	,

	OP_SS_DIV	,
	OP_SV_DIV	,
	OP_VS_DIV	,
	OP_VV_DIV	,

	OP_VV_NOISE11	,
	OP_VV_NOISE12	,
	OP_VV_NOISE13	,

	OP_VV_NOISE31	,
	OP_VV_NOISE32	,
	OP_VV_NOISE33	,

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

	const SlSymbol	*mpSrcSymbol;

	SlValue()
	{
		Flags.mOwnData = 0;
		Flags.mCanChange = 0;
		Data.pVoidValue = NULL;
		mpSrcSymbol = NULL;
	}

	void SetDataR( const void *pData, const SlSymbol *pSrcSymbol )
	{
		Flags.mCanChange		= 0;
		Data.pConstVoidValue	= pData;
		mpSrcSymbol				= pSrcSymbol;
	}

	void SetDataRW( void *pData, const SlSymbol *pSrcSymbol )
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
	SlSymbol	*mpOrigSymbol;
};

//==================================================================
struct SlAddress
{
	u_int	mOffset;
};

//==================================================================
union SlCPUWord
{
	SlOpCode		mOpCode;
	SlSymbolWord	mSymbol;
	SlAddress		mAddress;
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
	DVec<SlSymbol*>	mSymbols;
	u_int			mStartPC;
	DVec<SlCPUWord>	mCode;

	struct CtorParams
	{
		const char	*pName;
		const char	*pSource;
		const char	*pSourceFileName;
		
		CtorParams() :
			pName(NULL),
			pSource(NULL),
			pSourceFileName(NULL)
		{
		}
	};
	
	SlShader( const CtorParams &params );
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
	SlSymbolList		mCallingParams;	// $$$ should get these from attributes and Surface params at some point !!

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

	SlShaderInstance( const SlShaderInstance &right ) :
		mCallingParams(32)
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
				SlSymbol::Type	type,
				bool			isVarying,
				void			*pValue );

	SlValue	*Bind( const SlSymbolList &gridSymbols ) const;
	void Unbind( SlValue * &pDataSegment ) const;

	void Run( SlRunContext &ctx ) const;
};

//==================================================================
}

#endif
