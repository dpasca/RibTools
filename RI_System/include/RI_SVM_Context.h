//==================================================================
/// RI_SVM_Context.h
///
/// Created by Davide Pasca - 2009/2/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SVM_CONTEXT_H
#define RI_SVM_CONTEXT_H

#include "RI_SVM_Shader.h"

//==================================================================
namespace RI
{
class Attributes;
class SymbolList;
class WorkGrid;

//==================================================================
namespace SVM
{

//==================================================================
/// SlIlluminanceCtx
//==================================================================
class SlIlluminanceCtx
{
public:
	u_int			mBodyStartAddr;
	const Float3_	*mpAxis	;
	const Float_	*mpAngle;

	Float3_			*mpL;

	size_t			mActLightIdx;
	size_t			mActLightsN;

	SlIlluminanceCtx() :
		mpAxis(NULL),
		mpAngle(NULL),
		mpL(NULL),
		mActLightIdx(DNPOS),
		mActLightsN(0)
	{
	}

	void Init(
		u_int			bodyStartAddr,
		const Float3_	*pAxis	,
		const Float_	*pAngle,
		Float3_			*pL,
		size_t			lightsN
		)
	{
		mActLightIdx = 0;
	
		mBodyStartAddr	= bodyStartAddr;

		mpAxis		= pAxis	;
		mpAngle		= pAngle;
		mpL			= pL	;
		mActLightsN	= lightsN;
	}

	void Increment()		{	mActLightIdx += 1;				}
	void Reset()			{	mActLightIdx = DNPOS;			}
	bool IsActive() const	{	return mActLightIdx != DNPOS;	}
};


//==================================================================
class SRC_FuncopStack
{
public:
	static const u_int	FLG_IFTRUE			= 1 << 1;
	static const u_int	FLG_IFTRUE_TRUE		= 1 << 2;
	static const u_int	FLG_IFTRUE_VARY		= 1 << 3;
	static const u_int	FLG_ORELSE			= 1 << 4;
	static const u_int	FLG_SOLAR			= 1 << 5;
	static const u_int	FLG_ILLUMINANCE		= 1 << 6;

	static const size_t	MAX_N = 16;

	u_int	mStack[MAX_N];
	size_t	mStackN;

public:
	SRC_FuncopStack() :
		mStackN(0)
	{
	}

	void push( u_int id )
	{
		DASSTHROW( mStackN < MAX_N, ("Out of bounds with funcop stack !") );
		mStack[ mStackN++ ] = id;
	}

	void pop()
	{
		DASSTHROW( mStackN > 0, ("Out of bounds with funcop stack !") );
		--mStackN;
	}

	u_int top() const
	{
		DASSTHROW( mStackN > 0, ("Out of bounds with funcop stack !") );
		return mStack[ mStackN - 1 ];
	}

	void clear()
	{
		mStackN = 0;
	}

	size_t size() const
	{
		return mStackN;
	}
};

//==================================================================
/// Context
//==================================================================
class Context
{
	static const u_int		MAX_PROG_STACK = 32;

	const size_t			mMaxPointsN;

public:
	u_int					mProgramCounterIdx;
	u_int					mProgramCounter[MAX_PROG_STACK];

	DVec<u_int>				mDefParamValsStartPCs;

public:
	WorkGrid				*mpGrid;

	SRC_FuncopStack			mFopStack;

	// for surface shaders only
	SlIlluminanceCtx		mIlluminanceCtx;

	u_int					mBlocksXN;
	u_int					mPointsYN;
	u_int					mPointsN;
	u_int					mBlocksN;
	int						*mpSIMDFlags;
	Value					*mpDataSegment;
	const ShaderInst		*mpShaderInst;
	SymbolIList				*mpGridSymIList;
	const Attributes		*mpAttribs;

	DVec<Context *>	mpActLightsCtxs;

	class Cache
	{
	public:
		const Attributes	*mpAttribs;
		bool				mAmbientColDone;
		SlColor				mAmbientCol;

		Cache() :
			mpAttribs(NULL)
		{
			mAmbientColDone = false;
		}

		void Reset( const Attributes *pAttribs )
		{
			mpAttribs		= pAttribs;
			mAmbientColDone = false;
		}

	} mCache;

	Context( SymbolIList &symsIList, size_t maxPointsN );
	~Context();

	void Init( WorkGrid *pGrid );

	void SetupIfChanged(
			const Attributes	&attribs,
			const ShaderInst	*pShaderInst,
			u_int				blocksXN,
			u_int				pointsYN,
			size_t				pointsN=0 );

	Context *GetActLightCtx( size_t actLightIdx )
	{
		return mpActLightsCtxs[ actLightIdx ];
	}

	void ActLightsCtxs_CheckInit();
	void FreeActLightsCtxs();

	const Shader *GetShader() const
	{
		return mpShaderInst->moShader.Use();
	}

	bool IsInFuncop() const
	{
		return mFopStack.size() != 0;
	}

	const CPUWord *GetOp( u_int argc ) const
	{
		return &mpShaderInst->moShader->mCode[mProgramCounter[mProgramCounterIdx] + argc];
	}
	
	u_int	GetOpCount() const
	{
		return GetOp(0)->mOpCode.mOperandCount;
	}

	Value			&GetValue( u_int argc )
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpDataSegment[tableOff];
	}
	const Value	&GetValue( u_int argc ) const
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpDataSegment[tableOff];
	}

	Symbol &GetSymbol( u_int argc )
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return *mpShaderInst->moShader->mpShaSyms[tableOff ];
	}

	float	GetImmFloat( u_int argc )
	{
		return GetOp(argc)->mImmFloat.mValue;
	}

	bool IsSymbolVarying( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying;
	}

	int GetSymbolVaryingStep( u_int argc ) const
	{
		return GetOp(argc)->mSymbol.mIsVarying ? 1 : 0;
	}

	// -----
public:
	bool isOpcodeTypeCompatible( u_int argc, Symbol::Type matchType ) const
	{
		const Value	&value = GetValue( argc );

		return
			Symbol::IsTypeCompatible(
							value.mpSrcSymbol->mType,
							matchType );
	}

	// -----
	inline void *GetRW( u_int argc )
	{
		Value	&value = GetValue( argc );
		DASSERT( value.Flags.mCanChange != 0 );
		return value.Data.pVoidValue;
	}

	// -----
	inline const void *GetRO( u_int argc ) const
	{
		const Value	&value = GetValue(argc);
		return value.Data.pVoidValue;
	}

	void InitializeSIMD( size_t samplesN );
	
	//bool IsProcessorActive( u_int i ) const { return mpSIMDFlags[i] == 0; }
	VecNMask GetWriteMask( u_int i ) const	{ return VecNMaskFull; }

	void EnableProcessor( u_int i )			{ mpSIMDFlags[i] -= 1; }
	void DisableProcessor( u_int i )		{ mpSIMDFlags[i] += 1; }
	u_int GetProcessorsN() const			{ return mPointsN; }
	void NextInstruction()					{ mProgramCounter[mProgramCounterIdx] += GetOpCount() + 1; }
	u_int GetCurPC() const					{ return mProgramCounter[mProgramCounterIdx]; }
	
	void GotoInstruction( u_int address )
	{
		DASSERT( address < mpShaderInst->moShader->mCode.size() );
		mProgramCounter[mProgramCounterIdx] = address;
	}
};

#define SLRUNCTX_BLKWRITECHECK(_I_)	\
			VecNMask	blkMask = ctx.GetWriteMask( _I_ );	\
			if ( blkMask == VecNMaskEmpty )	\
				continue


//==================================================================
}
//==================================================================
}

#endif
