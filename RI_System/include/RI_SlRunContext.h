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
class MicroPolygonGrid;

//==================================================================
/// SlIlluminanceCtx
//==================================================================
class SlIlluminanceCtx
{
public:
	u_int			mBodyStartAddr;
	const SlVec3	*mpAxis	;
	const SlScalar	*mpAngle;

	SlVec3			*mpL;

	size_t			mLightIdx;
	size_t			mLightsN;

	SlIlluminanceCtx() :
		mpAxis(NULL),
		mpAngle(NULL),
		mpL(NULL),
		mLightIdx(DNPOS),
		mLightsN(0)
	{
	}

	void Init(
		u_int			bodyStartAddr,
		const SlVec3	*pAxis	,
		const SlScalar	*pAngle,
		SlVec3			*pL,
		size_t			lightsN
		)
	{
		mLightIdx = 0;
	
		mBodyStartAddr	= bodyStartAddr;

		mpAxis		= pAxis	;
		mpAngle		= pAngle;
		mpL			= pL	;
		mLightsN	= lightsN;
	}

	bool Next()				{	mLightIdx += 1;	return mLightIdx < mLightsN;	}
	void Reset()			{	mLightIdx = DNPOS;	}
	bool IsActive() const	{	return mLightIdx != DNPOS;	}
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
	MicroPolygonGrid		*mpGrid;

	// for light shaders only
	//SlSolarCtx				mSolarCtx;

	// for surface shaders only
	SlIlluminanceCtx		mSlIlluminanceCtx;

	u_int					mBlocksXN;
	u_int					mPointsYN;
	u_int					mPointsN;
	u_int					mBlocksN;
	int						*mpSIMDFlags;
	SlValue					*mpDataSegment;
	const SlShaderInst		*mpShaderInst;
	SymbolIList				*mpGridSymIList;
	const Attributes		*mpAttribs;

	class Cache
	{
	public:
		bool	mAmbientColDone;
		SlColor	mAmbientCol;

		Cache()
		{
			mAmbientColDone = false;
		}
	} mCache;

	SlRunContext( SymbolIList &symsIList, size_t maxPointsN );
	~SlRunContext();

	void Init( MicroPolygonGrid *pGrid );

	void Setup(
			const Attributes	&attribs,
			const SlShaderInst	*pShaderInst,
			u_int				blocksXN,
			u_int				pointsYN,
			size_t				pointsN );

	bool IsInFuncop() const
	{
		return
			//mSolarCtx.IsActive() ||
			mSlIlluminanceCtx.IsActive();
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

	SlScalar *GetVoidRW( SlScalar *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT &&
				 value.Flags.mCanChange != 0 );

		return (SlScalar *)value.Data.pVoidValue;
	}

	SlVec2 *GetVoidRW( SlVec2 *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT &&
				 value.Flags.mCanChange != 0 );

		return (SlVec2 *)value.Data.pVoidValue;
	}

	SlVec3 *GetVoidRW( SlVec3 *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_POINT ||
			 value.mpSrcSymbol->mType == Symbol::TYP_COLOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_VECTOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_NORMAL) &&
			 value.Flags.mCanChange != 0 );

		return (SlVec3 *)value.Data.pVoidValue;
	}

	SlStr *GetVoidRW( SlStr *unused, u_int argc )
	{
		SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_STRING) &&
			 value.Flags.mCanChange != 0 );

		return (SlStr *)value.Data.pVoidValue;
	}

	const SlScalar *GetVoidRO( const SlScalar *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);
		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT );
		return (const SlScalar *)value.Data.pVoidValue;
	}

	const SlVec2 *GetVoidRO( const SlVec2 *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);
		DASSERT( value.mpSrcSymbol->mType == Symbol::TYP_FLOAT );
		return (const SlVec2 *)value.Data.pVoidValue;
	}

	const SlVec3 *GetVoidRO( const SlVec3 *unused, u_int argc ) const
	{
		const SlValue	&value = GetValue(argc);

		DASSERT(
			(value.mpSrcSymbol->mType == Symbol::TYP_POINT ||
			 value.mpSrcSymbol->mType == Symbol::TYP_COLOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_VECTOR ||
			 value.mpSrcSymbol->mType == Symbol::TYP_NORMAL) );

		return (const SlVec3 *)value.Data.pVoidValue;
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
