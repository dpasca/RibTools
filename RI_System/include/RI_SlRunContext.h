//==================================================================
/// RI_SlRunContext.h
///
/// Created by Davide Pasca - 2009/2/20
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#ifndef RI_SLRUNCONTEXT_H
#define RI_SLRUNCONTEXT_H

#include "RI_SlShader.h"

//==================================================================
namespace RI
{

class Attributes;
class SymbolList;
class WorkGrid;

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
/// SlRunContext
//==================================================================
class SlRunContext
{
	static const u_int		MAX_PROG_STACK = 32;

	const size_t			mMaxPointsN;

public:
	u_int					mProgramCounterIdx;
	u_int					mProgramCounter[MAX_PROG_STACK];

	DVec<u_int>				mDefParamValsStartPCs;

public:
	WorkGrid		*mpGrid;

	// for light shaders only
	bool					mIsInSolar;

	// for surface shaders only
	SlIlluminanceCtx		mIlluminanceCtx;

	u_int					mBlocksXN;
	u_int					mPointsYN;
	u_int					mPointsN;
	u_int					mBlocksN;
	int						*mpSIMDFlags;
	SlValue					*mpDataSegment;
	const SlShaderInst		*mpShaderInst;
	SymbolIList				*mpGridSymIList;
	const Attributes		*mpAttribs;

	DVec<SlRunContext *>	mpActLightsCtxs;

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

	SlRunContext( SymbolIList &symsIList, size_t maxPointsN );
	~SlRunContext();

	void Init( WorkGrid *pGrid );

	void SetupIfChanged(
			const Attributes	&attribs,
			const SlShaderInst	*pShaderInst,
			u_int				blocksXN,
			u_int				pointsYN,
			size_t				pointsN=0 );

	SlRunContext *GetActLightCtx( size_t actLightIdx )
	{
		return mpActLightsCtxs[ actLightIdx ];
	}

	void ActLightsCtxs_CheckInit();
	void FreeActLightsCtxs();

	const SlShader *GetShader() const
	{
		return mpShaderInst->moShader.Use();
	}

	bool IsInFuncop() const
	{
		return
			mIsInSolar ||
			mIlluminanceCtx.IsActive();
	}

	const SlCPUWord *GetOp( u_int argc ) const
	{
		return &mpShaderInst->moShader->mCode[mProgramCounter[mProgramCounterIdx] + argc];
	}
	
	u_int	GetOpCount() const
	{
		return GetOp(0)->mOpCode.mOperandCount;
	}

	SlValue			&GetValue( u_int argc )
	{
		u_int	tableOff = GetOp(argc)->mSymbol.mTableOffset;
		return mpDataSegment[tableOff];
	}
	const SlValue	&GetValue( u_int argc ) const
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

	Float_ *GetVoidRW( Float_ *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT &&
				 value.Flags.mCanChange != 0 );

		return (Float_ *)value.Data.pVoidValue;
	}

	Float2_ *GetVoidRW( Float2_ *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT &&
				 value.Flags.mCanChange != 0 );

		return (Float2_ *)value.Data.pVoidValue;
	}

	Float3_ *GetVoidRW( Float3_ *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_POINT ||
			 value.mpSrcSymbol->mType == Symbol::TYP_COLOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_VECTOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_NORMAL) &&
			 value.Flags.mCanChange != 0 );

		return (Float3_ *)value.Data.pVoidValue;
	}

	SlStr *GetVoidRW( SlStr *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_STRING) &&
			 value.Flags.mCanChange != 0 );

		return (SlStr *)value.Data.pVoidValue;
	}

	const Float_ *GetVoidRO( const Float_ *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);
		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT );
		return (const Float_ *)value.Data.pVoidValue;
	}

	const Float2_ *GetVoidRO( const Float2_ *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);
		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT );
		return (const Float2_ *)value.Data.pVoidValue;
	}

	const Float3_ *GetVoidRO( const Float3_ *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_POINT ||
			 value.mpSrcSymbol->mType == Symbol::TYP_COLOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_VECTOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_NORMAL) );

		return (const Float3_ *)value.Data.pVoidValue;
	}

	const SlStr *GetVoidRO( const SlStr *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_STRING) );

		return (const SlStr *)value.Data.pVoidValue;
	}

	void InitializeSIMD( size_t samplesN );
	
	//bool IsProcessorActive( u_int i ) const { return mpSIMDFlags[i] == 0; }
	bool IsProcessorActive( u_int i ) const { return true; }
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

//==================================================================
}

#endif
